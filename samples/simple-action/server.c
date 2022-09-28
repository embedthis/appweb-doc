/*
    server.c - Appweb server to demonstrate actions

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/******************************* Includes *****************************/

#include    "appweb.h"

/********************************* Code *******************************/
/*
    This method is run when the action form is called from the web page.
 */

static void myaction(HttpStream *stream)
{
    HttpQueue   *q;

    q = stream->writeq;
    /*
        Set the HTTP response status
     */
    httpSetStatus(stream, 200);

    /*
        Add desired headers. "Set" will overwrite, add will create if not already defined.
     */
    httpAddHeaderString(stream, "Content-Type", "text/html");
    httpSetHeaderString(stream, "Cache-Control", "no-cache");

    httpWrite(q, "<html><title>simpleAction</title><body>\r\n");
    httpWrite(q, "<p>Name: %s</p>\n", httpGetParam(stream, "name", "unspecified"));
    httpWrite(q, "<p>Address: %s</p>\n", httpGetParam(stream, "address", "unspecified"));
    httpWrite(q, "</body></html>\r\n");

    /*
        Call finalize output and close the request.
        Delay closing if you want to do asynchronous output and close later.
     */
    httpFinalize(stream);

#if POSSIBLE
    /*
        Useful things to do in actions
     */
    httpRedirect(stream, 302, "/other-uri");
    httpError(stream, 409, "My message : %d", 5);
#endif
}


/*
    Create a simple stand-alone web server
 */
int main(int argc, char **argv, char **envp)
{
    Mpr         *mpr;
    int         rc;

    rc = MPR_ERR_CANT_CREATE;
    if ((mpr = mprCreate(0, NULL, MPR_USER_EVENTS_THREAD)) == 0) {
        mprError("Cannot create runtime");
        return -1;
    }
    if (httpCreate(HTTP_CLIENT_SIDE | HTTP_SERVER_SIDE) == 0) {
        mprError("Cannot create the HTTP services");
        return -1;
    }
    mprStart();

    if (maParseConfig("appweb.conf") < 0) {
        mprError("Cannot parse the config file %s", "appweb.conf");
        return -1;
    }
    httpDefineAction("/action/myaction", myaction);

    if (httpStartEndpoints() < 0) {
        mprError("Cannot start the web server");
        return -1;
    }
    mprServiceEvents(-1, 0);
    mprDestroy();
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
