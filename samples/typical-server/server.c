/**
    server.c  -- AppWeb main program

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.

    usage: appweb [options]
    or:    appweb [options] [documents] [[ip][:port] ...]
            --config configFile     # Use given config file instead
            --debugger              # Disable timeouts to make debugging easier
            --home path             # Set the home working directory
            --log logFile:level     # Log to file file at verbosity level
            --name uniqueName       # Name for this instance
            --version               # Output version information
            -v                      # Same as --log stderr:2
 */

/********************************* Includes ***********************************/

#include    "appweb.h"
#include    "esp.h"

/********************************** Locals ************************************/
/*
    Global application object. Provides the top level roots of all data objects for the Garbage collector.
    Appweb uses garbage collection and all memory must have a reference to stop being reclaimed by the GC.
 */
typedef struct AppwebApp {
    Mpr         *mpr;
    cchar       *documents;
    cchar       *home;
    cchar       *configFile;
    char        *pathVar;
} AppwebApp;

static AppwebApp *app;

/***************************** Forward Declarations ***************************/

static int changeRoot(cchar *jail);
static int checkEnvironment(cchar *program);
static int createEndpoints(int argc, char **argv);
static int findAppwebConf();
static void manageApp(AppwebApp *app, int flags);
static void usageError();

#if ME_UNIX_LIKE
#elif ME_WIN_LIKE
    static long msgProc(HWND hwnd, uint msg, uint wp, long lp);
#endif

/*********************************** Code *************************************/

MAIN(appweb, int argc, char **argv, char **envp)
{
    Mpr     *mpr;
    cchar   *argp, *jail;
    char    *logSpec, *traceSpec;
    int     argind, status, verbose;

    jail = 0;
    verbose = 0;
    logSpec = 0;
    traceSpec = 0;

    if ((mpr = mprCreate(argc, argv, MPR_USER_EVENTS_THREAD)) == NULL) {
        exit(1);
    }
    mprSetAppName(ME_NAME, ME_TITLE, ME_VERSION);

    if (httpCreate(HTTP_CLIENT_SIDE | HTTP_SERVER_SIDE) == 0) {
        exit(2);
    }

    /*
        Allocate the top level application object. ManageApp is the GC manager function and is called
        by the GC to mark references in the app object.
     */
    if ((app = mprAllocObj(AppwebApp, manageApp)) == NULL) {
        exit(3);
    }
    mprAddRoot(app);
    mprAddStandardSignals();

    app->mpr = mpr;
    app->configFile = sclone("appweb.conf");
    app->home = mprGetCurrentPath();
    app->documents = app->home;
    argc = mpr->argc;
    argv = (char**) mpr->argv;

    for (argind = 1; argind < argc; argind++) {
        argp = argv[argind];
        if (*argp != '-') {
            break;
        }
        if (smatch(argp, "--config") || smatch(argp, "--conf")) {
            if (argind >= argc) {
                usageError();
            }
            app->configFile = sclone(argv[++argind]);

#if ME_UNIX_LIKE
        } else if (smatch(argp, "--chroot")) {
            if (argind >= argc) {
                usageError();
            }
            jail = mprGetAbsPath(argv[++argind]);
#endif

        } else if (smatch(argp, "--debugger") || smatch(argp, "-D")) {
            mprSetDebugMode(1);

        } else if (smatch(argp, "--exe")) {
            if (argind >= argc) {
                usageError();
            }
            mpr->argv[0] = mprGetAbsPath(argv[++argind]);
            mprSetAppPath(mpr->argv[0]);
            mprSetModuleSearchPath(NULL);

        } else if (smatch(argp, "--home")) {
            if (argind >= argc) {
                usageError();
            }
            app->home = mprGetAbsPath(argv[++argind]);
            if (chdir(app->home) < 0) {
                mprError("%s: Cannot change directory to %s", mprGetAppName(), app->home);
                exit(4);
            }

        } else if (smatch(argp, "--log") || smatch(argp, "-l")) {
            if (argind >= argc) {
                usageError();
            }
            logSpec = argv[++argind];

        } else if (smatch(argp, "--name") || smatch(argp, "-n")) {
            if (argind >= argc) {
                usageError();
            }
            mprSetAppName(argv[++argind], 0, 0);

        } else if (smatch(argp, "--trace") || smatch(argp, "-t")) {
            if (argind >= argc) {
                usageError();
            }
            traceSpec = argv[++argind];

        } else if (smatch(argp, "--verbose") || smatch(argp, "-v")) {
            if (!logSpec) {
                logSpec = sfmt("stderr:2");
            }
            if (!traceSpec) {
                traceSpec = sfmt("stderr:2");
            }

        } else if (*argp == '-' && isdigit((uchar) argp[1])) {
            if (!logSpec) {
                logSpec = sfmt("stderr:%d", (int) stoi(&argp[1]));
            }
            if (!traceSpec) {
                traceSpec = sfmt("stderr:%d", (int) stoi(&argp[1]));
            }

        } else if (smatch(argp, "--version") || smatch(argp, "-V")) {
            mprPrintf("%s\n", ME_VERSION);
            exit(0);

        } else if (smatch(argp, "-?") || scontains(argp, "-help")) {
            usageError();
            exit(5);

        } else if (*argp == '-') {
            mprLog("error appweb", 0, "Unknown switch \"%s\"", argp);
            usageError();
            exit(5);
        }
    }
    if (logSpec) {
        mprStartLogging(logSpec, MPR_LOG_DETAILED | MPR_LOG_CONFIG | MPR_LOG_CMDLINE);
    }
    if (traceSpec) {
        httpStartTracing(traceSpec);
    }
    /*
        Start the multithreaded portable runtime (MPR)
     */
    if (mprStart() < 0) {
        mprError("Cannot start MPR for %s", mprGetAppName());
        mprDestroy();
        return MPR_ERR_CANT_INITIALIZE;
    }
    if (checkEnvironment(argv[0]) < 0) {
        exit(6);
    }
    if (findAppwebConf() < 0) {
        exit(7);
    }
    if (jail && changeRoot(jail) < 0) {
        exit(8);
    }
    /*
        Open the sockets to listen on
     */
    if (createEndpoints(argc - argind, &argv[argind]) < 0) {
        return MPR_ERR_CANT_INITIALIZE;
    }
    if (httpStartEndpoints() < 0) {
        mprError("Cannot start HTTP service, exiting.");
        exit(9);
    }
    mprServiceEvents(-1, 0);

    status = mprGetExitStatus();
    mprLog("info appweb", 1, "Stopping Appweb ...");
    mprDestroy();
    return status;
}


static void manageApp(AppwebApp *app, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(app->documents);
        mprMark(app->configFile);
        mprMark(app->pathVar);
        mprMark(app->home);
    }
}


/*
    Move into a chroot jail
 */
static int changeRoot(cchar *jail)
{
#if ME_UNIX_LIKE
    if (chdir(app->home) < 0) {
        mprError("%s: Cannot change directory to %s", mprGetAppName(), app->home);
        return MPR_ERR_CANT_INITIALIZE;
    }
    if (chroot(jail) < 0) {
        if (errno == EPERM) {
            mprError("%s: Must be super user to use the --chroot option", mprGetAppName());
        } else {
            mprError("%s: Cannot change change root directory to %s, errno %d", mprGetAppName(), jail, errno);
        }
        return MPR_ERR_CANT_INITIALIZE;
    }
#endif
    return 0;
}


/*
    Create the listening endoints
 */
static int createEndpoints(int argc, char **argv)
{
    cchar   *ip;
    int     argind, port, secure;

    ip = 0;
    port = -1;
    argind = 0;

    if (argc > argind) {
        mprParseSocketAddress(argv[argind++], &ip, &port, &secure, port);
    }
    if (maConfigureServer(app->configFile, app->home, app->documents, ip, port) < 0) {
        return MPR_ERR_CANT_CREATE;
    }
    return 0;
}


/*
    Find the appweb.conf file
 */
static int findAppwebConf()
{
    cchar   *userPath;

    userPath = app->configFile;
    if (app->configFile == 0) {
        app->configFile = mprJoinPathExt(mprGetAppName(), ".conf");
    }
    if (!mprPathExists(app->configFile, R_OK)) {
        if (!userPath) {
            app->configFile = mprJoinPath(app->home, "appweb.conf");
            if (!mprPathExists(app->configFile, R_OK)) {
                app->configFile = mprJoinPath(mprGetAppDir(), sfmt("%s.conf", mprGetAppName()));
            }
        }
        if (!mprPathExists(app->configFile, R_OK)) {
            mprError("Cannot open config file \"%s\"", app->configFile);
            return MPR_ERR_CANT_OPEN;
        }
    }
    return 0;
}


static void usageError(Mpr *mpr)
{
    cchar   *name;

    name = mprGetAppName();

    mprEprintf("\n%s Usage:\n\n"
        "  %s [options]\n"
        "  %s [options] documents ip[:port] ...\n\n"
        "  Without [documents ip:port], %s will read the appweb.conf configuration file.\n\n"
        "  Options:\n"
        "    --config configFile    # Use named config file instead appweb.conf\n"
        "    --chroot directory     # Change root directory to run more securely (Unix)\n"
        "    --debugger             # Disable timeouts to make debugging easier\n"
        "    --exe path             # Set path to Appweb executable on Vxworks\n"
        "    --home directory       # Change to directory to run\n"
        "    --log logFile:level    # Log to file file at verbosity level\n"
        "    --name uniqueName      # Unique name for this instance\n"
        "    --verbose              # Same as --log stderr:2\n"
        "    --version              # Output version information\n\n",
        mprGetAppTitle(), name, name, name);
    exit(10);
}


/*
    Security checks. Make sure we are staring with a safe environment
 */
static int checkEnvironment(cchar *program)
{
#if ME_UNIX_LIKE
    app->pathVar = sjoin("PATH=", getenv("PATH"), ":", mprGetAppDir(), NULL);
    putenv(app->pathVar);
#endif
    return 0;
}


/*
    Copyright (c) Embedthis Software. All Rights Reserved.
    This software is distributed under commercial and open source licenses.
    You may use the Embedthis Open Source license or you may acquire a
    commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.
 */
