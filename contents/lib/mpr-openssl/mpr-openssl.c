/*
    mpr-openssl.c - Support for secure sockets via OpenSSL

    This is the interface between the MPR Socket layer and the OpenSSL stack.
    This code expects at least OpenSSL 1.1.1.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

#if ME_COM_OPENSSL

#if ME_UNIX_LIKE
    /*
        Mac OS X OpenSSL stack is deprecated. Suppress those warnings.
     */
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

/* Clashes with WinCrypt.h */
#undef OCSP_RESPONSE

#ifndef  ME_MPR_SSL_HANDSHAKES
    #define ME_MPR_SSL_HANDSHAKES 0     /* Defaults to infinite */
#endif
#ifndef  ME_MPR_SSL_RENEGOTIATE
    #define ME_MPR_SSL_RENEGOTIATE 1
#endif

 /*
    Indent includes to bypass MakeMe dependencies
  */
 #include    <openssl/opensslv.h>
 #include    <openssl/ssl.h>
 #include    <openssl/evp.h>
 #include    <openssl/rand.h>
 #include    <openssl/err.h>
 #include    <openssl/dh.h>
 #include    <openssl/rsa.h>
 #include    <openssl/bio.h>

#if OPENSSL_VERSION_NUMBER >= 0x10002000L
    #include    <openssl/x509v3.h>
#ifndef OPENSSL_NO_ENGINE
    #include    <openssl/engine.h>
    #define MPR_HAS_CRYPTO_ENGINE 1
#endif
#endif

/************************************* Defines ********************************/
/*
    Default ciphers from Mozilla (https://wiki.mozilla.org/Security/Server_Side_TLS).
    See cipher mappings at: https://wiki.mozilla.org/Security/Server_Side_TLS#Cipher_names_correspondence_table

    Rationale:
    * TLSv1.3 ciphers are preferred and listed first. 
    * TLSv1 and v2 ciphers are legacy and only for backward compatibility with legacy systems.
    * For TLSv2, AES256-GCM is prioritized above its 128 bits variant, and ChaCha20 because we assume that most modern
      devices support AESNI instructions and thus benefit from fast and constant time AES.
    * For TLSv2, We recommend ECDSA certificates with P256 as other curves may not be supported everywhere. RSA signatures
      on ECDSA certificates are permitted because very few CAs sign with ECDSA at the moment.
    * For TLSv2, DHE is removed entirely because it is slow in comparison with ECDHE, and all modern clients support
      elliptic curve key exchanges.
    * For TLSv2, SHA1 signature algorithm is removed in favor of SHA384 for AES256 and SHA256 for AES128.
    * Recommended RSA and DH parameter size: 2048 bits.
 */
#define OPENSSL_TLSV3_DEFAULT_CIPHERS "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256"
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    #define OPENSSL_DEFAULT_CIPHERS OPENSSL_TLSV3_DEFAULT_CIPHERS ":ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256"
#else
    #define OPENSSL_DEFAULT_CIPHERS OPENSSL_TLSV3_DEFAULT_CIPHERS ":ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-SHA256:ECDHE-RSA-AES256-SHA:ECDHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA256:DHE-RSA-AES128-SHA256:DHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:ECDHE-RSA-DES-CBC3-SHA:EDH-RSA-DES-CBC3-SHA:AES256-GCM-SHA384:AES128-GCM-SHA256:AES256-SHA256:AES128-SHA256:AES256-SHA:AES128-SHA:DES-CBC3-SHA:HIGH:!aNULL:!eNULL:!EXPORT:!DES:!MD5:!PSK:!RC4:!SSLv3"
#endif

/*
    Map Iana names to OpenSSL names
 */
typedef struct CipherMap {
    int     code;
    cchar   *name;
    cchar   *ossName;
} CipherMap;

static CipherMap cipherMap[] = {
    { 0x0004, "TLS_RSA_WITH_RC4_128_MD5", "RC4-MD5" },
    { 0x0005, "TLS_RSA_WITH_RC4_128_SHA", "RC4-SHA" },
    { 0x0007, "TLS_RSA_WITH_IDEA_CBC_SHA", "IDEA-CBC-SHA" },
    { 0x0009, "TLS_RSA_WITH_DES_CBC_SHA", "DES-CBC-SHA" },
    { 0x000A, "TLS_RSA_WITH_3DES_EDE_CBC_SHA", "DES-CBC3-SHA" },
    { 0x000C, "TLS_DH_DSS_WITH_DES_CBC_SHA", "DH-DSS-DES-CBC-SHA" },
    { 0x000D, "TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA", "DH-DSS-DES-CBC3-SHA" },
    { 0x000F, "TLS_DH_RSA_WITH_DES_CBC_SHA", "DH-RSA-DES-CBC-SHA" },
    { 0x0010, "TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA", "DH-RSA-DES-CBC3-SHA" },
    { 0x0012, "TLS_DHE_DSS_WITH_DES_CBC_SHA", "EDH-DSS-DES-CBC-SHA" },
    { 0x0013, "TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA", "EDH-DSS-DES-CBC3-SHA" },
    { 0x0015, "TLS_DHE_RSA_WITH_DES_CBC_SHA", "EDH-RSA-DES-CBC-SHA" },
    { 0x0016, "TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA", "EDH-RSA-DES-CBC3-SHA" },
    { 0x002F, "TLS_RSA_WITH_AES_128_CBC_SHA", "AES128-SHA" },
    { 0x0030, "TLS_DH_DSS_WITH_AES_128_CBC_SHA", "DH-DSS-AES128-SHA" },
    { 0x0031, "TLS_DH_RSA_WITH_AES_128_CBC_SHA", "DH-RSA-AES128-SHA" },
    { 0x0032, "TLS_DHE_DSS_WITH_AES_128_CBC_SHA", "DHE-DSS-AES128-SHA" },
    { 0x0033, "TLS_DHE_RSA_WITH_AES_128_CBC_SHA", "DHE-RSA-AES128-SHA" },
    { 0x0035, "TLS_RSA_WITH_AES_256_CBC_SHA", "AES256-SHA" },
    { 0x0036, "TLS_DH_DSS_WITH_AES_256_CBC_SHA", "DH-DSS-AES256-SHA" },
    { 0x0037, "TLS_DH_RSA_WITH_AES_256_CBC_SHA", "DH-RSA-AES256-SHA" },
    { 0x0038, "TLS_DHE_DSS_WITH_AES_256_CBC_SHA", "DHE-DSS-AES256-SHA" },
    { 0x0039, "TLS_DHE_RSA_WITH_AES_256_CBC_SHA", "DHE-RSA-AES256-SHA" },
    { 0x003C, "TLS_RSA_WITH_AES_128_CBC_SHA256", "AES128-SHA256" },
    { 0x003D, "TLS_RSA_WITH_AES_256_CBC_SHA256", "AES256-SHA256" },
    { 0x003E, "TLS_DH_DSS_WITH_AES_128_CBC_SHA256", "DH-DSS-AES128-SHA256" },
    { 0x003F, "TLS_DH_RSA_WITH_AES_128_CBC_SHA256", "DH-RSA-AES128-SHA256" },
    { 0x0040, "TLS_DHE_DSS_WITH_AES_128_CBC_SHA256", "DHE-DSS-AES128-SHA256" },
    { 0x0041, "TLS_RSA_WITH_CAMELLIA_128_CBC_SHA", "CAMELLIA128-SHA" },
    { 0x0042, "TLS_DH_DSS_WITH_CAMELLIA_128_CBC_SHA", "DH-DSS-CAMELLIA128-SHA" },
    { 0x0043, "TLS_DH_RSA_WITH_CAMELLIA_128_CBC_SHA", "DH-RSA-CAMELLIA128-SHA" },
    { 0x0044, "TLS_DHE_DSS_WITH_CAMELLIA_128_CBC_SHA", "DHE-DSS-CAMELLIA128-SHA" },
    { 0x0045, "TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA", "DHE-RSA-CAMELLIA128-SHA" },
    { 0x0067, "TLS_DHE_RSA_WITH_AES_128_CBC_SHA256", "DHE-RSA-AES128-SHA256" },
    { 0x0068, "TLS_DH_DSS_WITH_AES_256_CBC_SHA256", "DH-DSS-AES256-SHA256" },
    { 0x0069, "TLS_DH_RSA_WITH_AES_256_CBC_SHA256", "DH-RSA-AES256-SHA256" },
    { 0x006A, "TLS_DHE_DSS_WITH_AES_256_CBC_SHA256", "DHE-DSS-AES256-SHA256" },
    { 0x006B, "TLS_DHE_RSA_WITH_AES_256_CBC_SHA256", "DHE-RSA-AES256-SHA256" },
    { 0x0084, "TLS_RSA_WITH_CAMELLIA_256_CBC_SHA", "CAMELLIA256-SHA" },
    { 0x0085, "TLS_DH_DSS_WITH_CAMELLIA_256_CBC_SHA", "DH-DSS-CAMELLIA256-SHA" },
    { 0x0086, "TLS_DH_RSA_WITH_CAMELLIA_256_CBC_SHA", "DH-RSA-CAMELLIA256-SHA" },
    { 0x0087, "TLS_DHE_DSS_WITH_CAMELLIA_256_CBC_SHA", "DHE-DSS-CAMELLIA256-SHA" },
    { 0x0088, "TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA", "DHE-RSA-CAMELLIA256-SHA" },
    { 0x008A, "TLS_PSK_WITH_RC4_128_SHA", "PSK-RC4-SHA" },
    { 0x008B, "TLS_PSK_WITH_3DES_EDE_CBC_SHA", "PSK-3DES-EDE-CBC-SHA" },
    { 0x008C, "TLS_PSK_WITH_AES_128_CBC_SHA", "PSK-AES128-CBC-SHA" },
    { 0x008D, "TLS_PSK_WITH_AES_256_CBC_SHA", "PSK-AES256-CBC-SHA" },
    { 0x0096, "TLS_RSA_WITH_SEED_CBC_SHA", "SEED-SHA" },
    { 0x0097, "TLS_DH_DSS_WITH_SEED_CBC_SHA", "DH-DSS-SEED-SHA" },
    { 0x0098, "TLS_DH_RSA_WITH_SEED_CBC_SHA", "DH-RSA-SEED-SHA" },
    { 0x0099, "TLS_DHE_DSS_WITH_SEED_CBC_SHA", "DHE-DSS-SEED-SHA" },
    { 0x009A, "TLS_DHE_RSA_WITH_SEED_CBC_SHA", "DHE-RSA-SEED-SHA" },
    { 0x009C, "TLS_RSA_WITH_AES_128_GCM_SHA256", "AES128-GCM-SHA256" },
    { 0x009D, "TLS_RSA_WITH_AES_256_GCM_SHA384", "AES256-GCM-SHA384" },
    { 0x009E, "TLS_DHE_RSA_WITH_AES_128_GCM_SHA256", "DHE-RSA-AES128-GCM-SHA256" },
    { 0x009F, "TLS_DHE_RSA_WITH_AES_256_GCM_SHA384", "DHE-RSA-AES256-GCM-SHA384" },
    { 0x00A0, "TLS_DH_RSA_WITH_AES_128_GCM_SHA256", "DH-RSA-AES128-GCM-SHA256" },
    { 0x00A1, "TLS_DH_RSA_WITH_AES_256_GCM_SHA384", "DH-RSA-AES256-GCM-SHA384" },
    { 0x00A2, "TLS_DHE_DSS_WITH_AES_128_GCM_SHA256", "DHE-DSS-AES128-GCM-SHA256" },
    { 0x00A3, "TLS_DHE_DSS_WITH_AES_256_GCM_SHA384", "DHE-DSS-AES256-GCM-SHA384" },
    { 0x00A4, "TLS_DH_DSS_WITH_AES_128_GCM_SHA256", "DH-DSS-AES128-GCM-SHA256" },
    { 0x00A5, "TLS_DH_DSS_WITH_AES_256_GCM_SHA384", "DH-DSS-AES256-GCM-SHA384" },
    { 0xC002, "TLS_ECDH_ECDSA_WITH_RC4_128_SHA", "ECDH-ECDSA-RC4-SHA" },
    { 0xC003, "TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA", "ECDH-ECDSA-DES-CBC3-SHA" },
    { 0xC004, "TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA", "ECDH-ECDSA-AES128-SHA" },
    { 0xC005, "TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA", "ECDH-ECDSA-AES256-SHA" },
    { 0xC007, "TLS_ECDHE_ECDSA_WITH_RC4_128_SHA", "ECDHE-ECDSA-RC4-SHA" },
    { 0xC008, "TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA", "ECDHE-ECDSA-DES-CBC3-SHA" },
    { 0xC009, "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA", "ECDHE-ECDSA-AES128-SHA" },
    { 0xC00A, "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA", "ECDHE-ECDSA-AES256-SHA" },
    { 0xC00C, "TLS_ECDH_RSA_WITH_RC4_128_SHA", "ECDH-RSA-RC4-SHA" },
    { 0xC00D, "TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA", "ECDH-RSA-DES-CBC3-SHA" },
    { 0xC00E, "TLS_ECDH_RSA_WITH_AES_128_CBC_SHA", "ECDH-RSA-AES128-SHA" },
    { 0xC00F, "TLS_ECDH_RSA_WITH_AES_256_CBC_SHA", "ECDH-RSA-AES256-SHA" },
    { 0xC011, "TLS_ECDHE_RSA_WITH_RC4_128_SHA", "ECDHE-RSA-RC4-SHA" },
    { 0xC012, "TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA", "ECDHE-RSA-DES-CBC3-SHA" },
    { 0xC013, "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA", "ECDHE-RSA-AES128-SHA" },
    { 0xC014, "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA", "ECDHE-RSA-AES256-SHA" },
    { 0xC01A, "TLS_SRP_SHA_WITH_3DES_EDE_CBC_SHA", "SRP-3DES-EDE-CBC-SHA" },
    { 0xC01B, "TLS_SRP_SHA_RSA_WITH_3DES_EDE_CBC_SHA", "SRP-RSA-3DES-EDE-CBC-SHA" },
    { 0xC01C, "TLS_SRP_SHA_DSS_WITH_3DES_EDE_CBC_SHA", "SRP-DSS-3DES-EDE-CBC-SHA" },
    { 0xC01D, "TLS_SRP_SHA_WITH_AES_128_CBC_SHA", "SRP-AES-128-CBC-SHA" },
    { 0xC01E, "TLS_SRP_SHA_RSA_WITH_AES_128_CBC_SHA", "SRP-RSA-AES-128-CBC-SHA" },
    { 0xC01F, "TLS_SRP_SHA_DSS_WITH_AES_128_CBC_SHA", "SRP-DSS-AES-128-CBC-SHA" },
    { 0xC020, "TLS_SRP_SHA_WITH_AES_256_CBC_SHA", "SRP-AES-256-CBC-SHA" },
    { 0xC021, "TLS_SRP_SHA_RSA_WITH_AES_256_CBC_SHA", "SRP-RSA-AES-256-CBC-SHA" },
    { 0xC022, "TLS_SRP_SHA_DSS_WITH_AES_256_CBC_SHA", "SRP-DSS-AES-256-CBC-SHA" },
    { 0xC023, "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256", "ECDHE-ECDSA-AES128-SHA256" },
    { 0xC024, "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384", "ECDHE-ECDSA-AES256-SHA384" },
    { 0xC025, "TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256", "ECDH-ECDSA-AES128-SHA256" },
    { 0xC026, "TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384", "ECDH-ECDSA-AES256-SHA384" },
    { 0xC027, "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256", "ECDHE-RSA-AES128-SHA256" },
    { 0xC028, "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384", "ECDHE-RSA-AES256-SHA384" },
    { 0xC029, "TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256", "ECDH-RSA-AES128-SHA256" },
    { 0xC02A, "TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384", "ECDH-RSA-AES256-SHA384" },
    { 0xC02B, "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256", "ECDHE-ECDSA-AES128-GCM-SHA256" },
    { 0xC02C, "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384", "ECDHE-ECDSA-AES256-GCM-SHA384" },
    { 0xC02D, "TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256", "ECDH-ECDSA-AES128-GCM-SHA256" },
    { 0xC02E, "TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384", "ECDH-ECDSA-AES256-GCM-SHA384" },
    { 0xC02F, "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256", "ECDHE-RSA-AES128-GCM-SHA256" },
    { 0xC030, "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384", "ECDHE-RSA-AES256-GCM-SHA384" },
    { 0xC031, "TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256", "ECDH-RSA-AES128-GCM-SHA256" },
    { 0xC032, "TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384", "ECDH-RSA-AES256-GCM-SHA384" },
    { 0x0000, 0 },
};

/*
    Configuration for a route/host
 */
typedef struct OpenConfig {
    SSL_CTX         *ctx;
    DH              *dhKey;
    cchar           *alpn;
    int             clearFlags;
    int             maxHandshakes;
    long            setFlags;
} OpenConfig;

typedef struct OpenSocket {
    MprSocket       *sock;
    OpenConfig      *cfg;
    char            *requiredPeerName;
    SSL             *handle;
    BIO             *bio;
    int             handshakes;
} OpenSocket;

typedef struct RandBuf {
    MprTime     now;
    int         pid;
} RandBuf;

static MprSocketProvider *openProvider;
static OpenConfig *defaultOpenConfig;

#if OPENSSL_VERSION_NUMBER < 0x10100000L
/*
    OpenSSL 1.0 uses shared data and will crash if multithread locks are not used
 */
static int      numLocks;
static MprMutex **olocks;

struct CRYPTO_dynlock_value {
    MprMutex    *mutex;
};
typedef struct CRYPTO_dynlock_value DynLock;
#endif

/*
    Used for OpenSSL versions < 1.0.2
 */
#ifndef ME_MPR_SSL_CURVE
    #define ME_MPR_SSL_CURVE "prime256v1"
#endif

/*
    Certificate and key formats
 */
#define FORMAT_PEM 1
#define FORMAT_DER 2

/***************************** Forward Declarations ***************************/

static void     closeOss(MprSocket *sp, bool gracefully);
static int      checkPeerCertName(MprSocket *sp);
static int      configOss(MprSsl *ssl, int flags, char **errorMsg);
static DH       *dhcallback(SSL *ssl, int isExport, int keyLength);
static void     disconnectOss(MprSocket *sp);
static ssize    flushOss(MprSocket *sp);
static DH       *getDhKey();
static char     *getOssSession(MprSocket *sp);
static char     *getOssState(MprSocket *sp);
static char     *getOssError(MprSocket *sp);
static X509     *getPeerCert(SSL *handle);
static void     infoCallback(const SSL *ssl, int where, int rc);
static void     manageOpenConfig(OpenConfig *cfg, int flags);
static void     manageOpenProvider(MprSocketProvider *provider, int flags);
static void     manageOpenSocket(OpenSocket *ssp, int flags);
static cchar    *mapCipherNames(cchar *ciphers);
static int      preloadOss(MprSsl *ssl, int flags);
static ssize    readOss(MprSocket *sp, void *buf, ssize len);
static void     setSecured(MprSocket *sp);
static int      setCertFile(SSL_CTX *ctx, cchar *certFile);
static int      setKeyFile(SSL_CTX *ctx, cchar *keyFile);
static int      sniHostname(SSL *ssl, int *al, void *arg);
static int      upgradeOss(MprSocket *sp, MprSsl *ssl, cchar *requiredPeerName);
static int      verifyPeerCertificate(int ok, X509_STORE_CTX *xctx);
static ssize    writeOss(MprSocket *sp, cvoid *buf, ssize len);

#if ME_MPR_HAS_ALPN
static cchar    *makeAlpn(MprSsl *ssl);
static int      selectAlpn(SSL *ssl, cuchar **out, uchar *outlen, cuchar *in, uint inlen, void *arg);
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L
static DynLock  *sslCreateDynLock(cchar *file, int line);
static void     sslDynLock(int mode, DynLock *dl, cchar *file, int line);
static void     sslDestroyDynLock(DynLock *dl, cchar *file, int line);
static void     sslStaticLock(int mode, int n, cchar *file, int line);
static ulong    sslThreadId(void);
#endif

#if MPR_HAS_CRYPTO_ENGINE
static int initEngine(MprSsl *ssl);
#endif

/************************************* Code ***********************************/
/*
    Initialize the MPR SSL layer
 */
PUBLIC int mprSslInit(void *unused, MprModule *module)
{
    RandBuf     randBuf;

    randBuf.now = mprGetTime();
    randBuf.pid = getpid();
    RAND_seed((void*) &randBuf, sizeof(randBuf));
#if ME_UNIX_LIKE
    RAND_load_file("/dev/urandom", 256);
#endif

    if ((openProvider = mprAllocObj(MprSocketProvider, manageOpenProvider)) == NULL) {
        return MPR_ERR_MEMORY;
    }
    openProvider->name = sclone("openssl");
    openProvider->preload = preloadOss;
    openProvider->upgradeSocket = upgradeOss;
    openProvider->closeSocket = closeOss;
    openProvider->disconnectSocket = disconnectOss;
    openProvider->flushSocket = flushOss;
    openProvider->socketState = getOssState;
    openProvider->readSocket = readOss;
    openProvider->writeSocket = writeOss;
    mprSetSslProvider(openProvider);

    /*
        Configure the SSL library. Use the crypto ID as a one-time test. This allows
        users to configure the library and have their configuration used instead.
     */
    mprGlobalLock();
    if (CRYPTO_get_id_callback() == 0) {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
        int     i;
        numLocks = CRYPTO_num_locks();
        if ((olocks = mprAlloc(numLocks * sizeof(MprMutex*))) == 0) {
            return MPR_ERR_MEMORY;
        }
        for (i = 0; i < numLocks; i++) {
            olocks[i] = mprCreateLock();
        }
        CRYPTO_set_id_callback(sslThreadId);
        CRYPTO_set_locking_callback(sslStaticLock);
        CRYPTO_set_dynlock_create_callback(sslCreateDynLock);
        CRYPTO_set_dynlock_destroy_callback(sslDestroyDynLock);
        CRYPTO_set_dynlock_lock_callback(sslDynLock);
#endif

#if !ME_WIN_LIKE
        OpenSSL_add_all_algorithms();
#endif
        /*
            WARNING: SSL_library_init() is not reentrant. Caller must ensure safety.
         */
        SSL_library_init();
        SSL_load_error_strings();
#if MPR_HAS_CRYPTO_ENGINE
        ENGINE_load_builtin_engines();
        ENGINE_add_conf_module();
        CONF_modules_load_file(NULL, NULL, 0);
#endif
    }
    mprGlobalUnlock();
    return 0;
}


/*
    MPR Garbage collector manager callback for OpenConfig. Called on each GC sweep.
 */
static void manageOpenConfig(OpenConfig *cfg, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(cfg->alpn);

    } else if (flags & MPR_MANAGE_FREE) {
        if (cfg->ctx != 0) {
            SSL_CTX_free(cfg->ctx);
            cfg->ctx = 0;
        }
        if (cfg == defaultOpenConfig) {
            if (cfg->dhKey) {
                DH_free(cfg->dhKey);
                cfg->dhKey = 0;
            }
        }
#if MPR_HAS_CRYPTO_ENGINE
        ENGINE_cleanup();
#endif
    }
}


/*
    MPR Garbage collector manager callback for MprSocketProvider. Called on each GC sweep.
 */
static void manageOpenProvider(MprSocketProvider *provider, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
        /* Mark global locks */
        int     i;
        if (olocks) {
            mprMark(olocks);
            for (i = 0; i < numLocks; i++) {
                mprMark(olocks[i]);
            }
        }
#endif
        mprMark(defaultOpenConfig);
        mprMark(provider->name);

    } else if (flags & MPR_MANAGE_FREE) {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
        olocks = 0;
#endif
    }
}


#if OPENSSL_VERSION_NUMBER < 0x10100000L

static ulong sslThreadId()
{
    return (long) mprGetCurrentOsThread();
}


static void sslStaticLock(int mode, int n, cchar *file, int line)
{
    assert(0 <= n && n < numLocks);

    if (olocks) {
        if (mode & CRYPTO_LOCK) {
            mprLock(olocks[n]);
        } else {
            mprUnlock(olocks[n]);
        }
    }
}


static DynLock *sslCreateDynLock(cchar *file, int line)
{
    DynLock     *dl;

    dl = mprAllocZeroed(sizeof(DynLock));
    dl->mutex = mprCreateLock();
    mprHold(dl->mutex);
    return dl;
}


static void sslDestroyDynLock(DynLock *dl, cchar *file, int line)
{
    MprMutex    *mutex;

    if (dl->mutex) {
        mutex = dl->mutex;
        dl->mutex = 0;
        mprRelease(mutex);
    }
}


static void sslDynLock(int mode, DynLock *dl, cchar *file, int line)
{
    if (mode & CRYPTO_LOCK) {
        mprLock(dl->mutex);
    } else {
        mprUnlock(dl->mutex);
    }
}
#endif

/*
    Create and initialize an SSL configuration for a route. This configuration is used by all requests for
    a given route. An application can have different SSL configurations for different routes. There is also
    a default SSL configuration that is used when a route does not define a configuration and one for clients.
 */
static int configOss(MprSsl *ssl, int flags, char **errorMsg)
{
    OpenConfig      *cfg;
    X509_STORE      *store;
    SSL_CTX         *ctx;
    cchar           *key;
    uchar           resume[16];
    int             rc, verifyMode;

    assert(ssl);
    if (errorMsg) {
        *errorMsg = 0;
    }
    if (ssl->config && !ssl->changed) {
        return 0;
    }
    if ((ssl->config = mprAllocObj(OpenConfig, manageOpenConfig)) == 0) {
        return MPR_ERR_MEMORY;
    }
    cfg = ssl->config;

    if ((ctx = SSL_CTX_new(SSLv23_method())) == 0) {
        mprLog("error openssl", 0, "Unable to create SSL context");
        return MPR_ERR_CANT_INITIALIZE;
    }
#if MPR_HAS_CRYPTO_ENGINE
    if (initEngine(ssl) < 0) {
        // Continue without engine
    }
#endif
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    SSL_CTX_set_ex_data(ctx, 0, (void*) ssl);
#else
    SSL_CTX_set_app_data(ctx, (void*) ssl);
#endif
    if (ssl->verifyPeer && !(ssl->caFile || ssl->caPath)) {
        *errorMsg = sfmt("Cannot verify peer due to undefined CA certificates");
        SSL_CTX_free(ctx);
        return MPR_ERR_CANT_INITIALIZE;
    }

    /*
        Configure the certificates
     */
    if (ssl->certFile) {
        if (setCertFile(ctx, ssl->certFile) < 0) {
            SSL_CTX_free(ctx);
            return MPR_ERR_CANT_INITIALIZE;
        }
        key = (ssl->keyFile == 0) ? ssl->certFile : ssl->keyFile;
        if (key) {
            if (setKeyFile(ctx, key) < 0) {
                SSL_CTX_free(ctx);
                return MPR_ERR_CANT_INITIALIZE;
            }
            if (!SSL_CTX_check_private_key(ctx)) {
                mprLog("error openssl", 0, "Check of private key file failed: %s", key);
                SSL_CTX_free(ctx);
                return MPR_ERR_CANT_INITIALIZE;
            }
        }
    }
    if (ssl->ciphers) {
        ssl->ciphers = mapCipherNames(ssl->ciphers);
    }
    if (!ssl->ciphers && (flags & MPR_SOCKET_SERVER)) {
        ssl->ciphers = sclone(OPENSSL_DEFAULT_CIPHERS);
    }
    if (ssl->ciphers) {
        mprLog("info openssl", 5, "Using SSL ciphers: %s", ssl->ciphers);
#if OPENSSL_VERSION_NUMBER >= 0x1010100L
        /*
            Set TLSv1.3 ciphers and <1.2 ciphers via (cipher_list).
            ciphersuites() fails if any cipher fails, cipher_list() fails if all ciphers fail.
            This fails if we can't get any cipher configured.
        */
        rc = 1;
        if (ssl->protocols & MPR_PROTO_TLSV1_3) {
            // v1.3 required
            if (SSL_CTX_set_ciphersuites(ctx, ssl->ciphers) != 1 && SSL_CTX_set_cipher_list(ctx, ssl->ciphers) != 1) {
                //  Fail
                rc = 0;
            }
        } else {
            //  No v1.3
            rc = SSL_CTX_set_cipher_list(ctx, ssl->ciphers);
        }
#else
        //  Pre v1.3
        rc = SSL_CTX_set_cipher_list(ctx, ssl->ciphers);
#endif
        if (rc != 1) {
            *errorMsg = sfmt("Unable to set cipher list \"%s\"", ssl->ciphers);
            SSL_CTX_free(ctx);
            return MPR_ERR_CANT_INITIALIZE;
        }
    }
    verifyMode = !ssl->verifyPeer ? SSL_VERIFY_NONE : SSL_VERIFY_PEER;
    if (verifyMode != SSL_VERIFY_NONE) {
        if (!(ssl->caFile || ssl->caPath)) {
            *errorMsg = sclone("No defined certificate authority file");
            SSL_CTX_free(ctx);
            return MPR_ERR_CANT_INITIALIZE;
        }
        if ((!SSL_CTX_load_verify_locations(ctx, (char*) ssl->caFile, (char*) ssl->caPath)) ||
                (!SSL_CTX_set_default_verify_paths(ctx))) {
            *errorMsg = sfmt("Unable to set certificate locations: %s: %s", ssl->caFile, ssl->caPath);
            SSL_CTX_free(ctx);
            return MPR_ERR_CANT_INITIALIZE;
        }
        if (ssl->caFile) {
            STACK_OF(X509_NAME) *certNames;
            certNames = SSL_load_client_CA_file(ssl->caFile);
            if (certNames) {
                /*
                    Define the list of CA certificates to send to the client
                    before they send their client certificate for validation
                 */
                SSL_CTX_set_client_CA_list(ctx, certNames);
            }
        }
        store = SSL_CTX_get_cert_store(ctx);
        if (ssl->revoke && !X509_STORE_load_locations(store, ssl->revoke, 0)) {
            mprLog("error openssl", 0, "Cannot load certificate revoke list: %s", ssl->revoke);
            SSL_CTX_free(ctx);
            return MPR_ERR_CANT_INITIALIZE;
        }
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
        X509_STORE_set_ex_data(store, 0, (void*) ssl);
#endif
        if (flags & MPR_SOCKET_SERVER) {
            SSL_CTX_set_verify_depth(ctx, ssl->verifyDepth);
        }
        SSL_CTX_set_verify(ctx, verifyMode, verifyPeerCertificate);
    }

    /*
        Define callbacks
     */
    if (flags & MPR_SOCKET_SERVER) {
        SSL_CTX_set_tlsext_servername_callback(ctx, sniHostname);
    }

    /*
        Configure DH parameters
     */
    SSL_CTX_set_tmp_dh_callback(ctx, dhcallback);
    cfg->dhKey = getDhKey();

    /*
        Define default OpenSSL options
        Ensure we generate a new private key for each connection
        Disable SSLv2, SSLv3 and TLSv1 by default -- they are insecure.
     */
    cfg->setFlags = SSL_OP_ALL | SSL_OP_SINGLE_DH_USE | SSL_OP_SINGLE_ECDH_USE | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1;

    /*
        Configuration time controls
     */
#if UNUSED
    if (!(ssl->protocols & MPR_PROTO_TLSV1)) {
        /* Disable all TLS V1.* */
#ifdef SSL_OP_NO_TLSv1
        cfg->setFlags |= SSL_OP_NO_TLSv1;
#endif
#ifdef SSL_OP_NO_TLSv1_1
        cfg->setFlags |= SSL_OP_NO_TLSv1_1;
#endif
#ifdef SSL_OP_NO_TLSv1_2
        cfg->setFlags |= SSL_OP_NO_TLSv1_2;
#endif
#ifdef SSL_OP_NO_TLSv1_3
        cfg->setFlags |= SSL_OP_NO_TLSv1_3;
#endif
    }
#endif /* UNUSED */

#ifdef SSL_OP_NO_SSLv2
    if (!(ssl->protocols & MPR_PROTO_SSLV2)) {
        cfg->setFlags |= SSL_OP_NO_SSLv2;
    }
#endif
#ifdef SSL_OP_NO_SSLv3
    if (!(ssl->protocols & MPR_PROTO_SSLV3)) {
        cfg->setFlags |= SSL_OP_NO_SSLv3;
    }
#endif
#ifdef SSL_OP_NO_TLSv1
    if (!(ssl->protocols & MPR_PROTO_TLSV1_0)) {
        cfg->setFlags |= SSL_OP_NO_TLSv1;
    }
#endif
#ifdef SSL_OP_NO_TLSv1_1
    if (!(ssl->protocols & MPR_PROTO_TLSV1_1)) {
        cfg->setFlags |= SSL_OP_NO_TLSv1_1;
    }
#endif
#ifdef SSL_OP_NO_TLSv1_2
    if (!(ssl->protocols & MPR_PROTO_TLSV1_2)) {
        cfg->setFlags |= SSL_OP_NO_TLSv1_2;
    }
#endif
#ifdef SSL_OP_NO_TLSv1_3
    if (!(ssl->protocols & MPR_PROTO_TLSV1_3)) {
        cfg->setFlags |= SSL_OP_NO_TLSv1_3;
    }
#endif
#ifdef SSL_OP_MSIE_SSLV2_RSA_PADDING
    cfg->setFlags |= SSL_OP_MSIE_SSLV2_RSA_PADDING;
#endif
#if defined(SSL_OP_NO_TICKET)
    /*
        Ticket based session reuse is enabled by default
     */
    #if defined(ME_MPR_SSL_TICKET)
        if (ME_MPR_SSL_TICKET) {
            cfg->clearFlags |= SSL_OP_NO_TICKET;
        } else {
            cfg->setFlags |= SSL_OP_NO_TICKET;
        }
    #else
        cfg->clearFlags |= SSL_OP_NO_TICKET;
    #endif
#endif

#if defined(SSL_OP_NO_COMPRESSION)
    /*
        CRIME attack targets compression
     */
    cfg->clearFlags |= SSL_OP_NO_COMPRESSION;
#endif

#if defined(SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION)
    /*
        Force a new session on renegotiation. Default to true.
        This is required when using SNI and changing context during the SSL hello
     */
    #if defined(ME_MPR_SSL_RENEGOTIATE)
        if (ME_MPR_SSL_RENEGOTIATE) {
            cfg->clearFlags |= SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION;
        } else {
            cfg->setFlags |= SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION;
        }
    #else
        cfg->setFlags |= SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION;
    #endif
#endif

#if defined(ME_MPR_SSL_HANDSHAKES)
    cfg->maxHandshakes = ME_MPR_SSL_HANDSHAKES;
#endif

#if defined(SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS)
    /*
        Disables a countermeasure against a SSL 3.0/TLS 1.0 protocol vulnerability affecting CBC ciphers.
        Defaults to true.
     */
    #if defined(ME_MPR_SSL_EMPTY_FRAGMENTS)
        if (ME_MPR_SSL_EMPTY_FRAGMENTS) {
            /* SSL_OP_ALL disables empty fragments. Only needed for ancient browsers like IE-6 on SSL-3.0/TLS-1.0 */
            cfg->clearFlags |= SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
        } else {
            cfg->setFlags |= SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
        }
    #else
        cfg->setFlags |= SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
    #endif
#endif

    /*
        Define a session reuse context
     */
    RAND_bytes(resume, sizeof(resume));
    SSL_CTX_set_session_id_context(ctx, resume, sizeof(resume));

    /*
        Elliptic Curve initialization
     */
#if SSL_OP_SINGLE_ECDH_USE
    #ifdef SSL_CTX_set_ecdh_auto
        // This is supported in OpenSSL 1.0.2
        SSL_CTX_set_ecdh_auto(ctx, 1);
    #else
        {
            EC_KEY  *ecdh;
            cchar   *name;
            int      nid;

            name = ME_MPR_SSL_CURVE;
            if ((nid = OBJ_sn2nid(name)) == 0) {
                *errorMsg = sfmt("Unknown curve name \"%s\"", name);
                SSL_CTX_free(ctx);
                return MPR_ERR_CANT_INITIALIZE;
            }
            if ((ecdh = EC_KEY_new_by_curve_name(nid)) == 0) {
                *errorMsg = sfmt("Unable to create curve \"%s\"", name);
                SSL_CTX_free(ctx);
                return MPR_ERR_CANT_INITIALIZE;
            }
            SSL_CTX_set_tmp_ecdh(ctx, ecdh);
            EC_KEY_free(ecdh);
        }
    #endif
#endif

    SSL_CTX_set_mode(ctx, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_AUTO_RETRY | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
#ifdef SSL_MODE_RELEASE_BUFFERS
    SSL_CTX_set_mode(ctx, SSL_MODE_RELEASE_BUFFERS);
#endif
#ifdef SSL_OP_CIPHER_SERVER_PREFERENCE
    SSL_CTX_set_mode(ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
#endif

#if defined(ME_MPR_SSL_CACHE)
    /*
        Set the number of sessions supported. Default in OpenSSL is 20K.
     */
    SSL_CTX_sess_set_cache_size(ctx, ME_MPR_SSL_CACHE);
#else
    SSL_CTX_sess_set_cache_size(ctx, 1024);
#endif

    SSL_CTX_set_options(ctx, cfg->setFlags);
    SSL_CTX_clear_options(ctx, cfg->clearFlags);

    cfg->ctx = ctx;
    ssl->changed = 0;
    ssl->config = cfg;

#if ME_MPR_HAS_ALPN
    if (ssl->alpn) {
        cfg->alpn = makeAlpn(ssl);
        SSL_CTX_set_alpn_protos(ctx, (cuchar*) cfg->alpn, (int) slen(cfg->alpn));
        SSL_CTX_set_alpn_select_cb(ctx, selectAlpn, NULL);
    }
#endif
    return 0;
}

#if MPR_HAS_CRYPTO_ENGINE
static int initEngine(MprSsl *ssl)
{
    ENGINE  *engine;

    if (ssl->device) {
        mprLog("mpr ssl openssl info", 0, "Initializing engine %s", ssl->device);
        if (!(engine = ENGINE_by_id(ssl->device))) {
            mprLog("mpr ssl openssl error", 0, "Cannot find crypto device %s", ssl->device);
            return MPR_ERR_CANT_FIND;
        }
        if (!ENGINE_set_default(engine, ENGINE_METHOD_ALL)) {
            mprLog("mpr ssl openssl error", 0, "Cannot find crypto device %s", ssl->device);
            ENGINE_free(engine);
            return MPR_ERR_CANT_FIND;
        }
        mprLog("mpr ssl openssl info", 0, "Loaded crypto device %s", ssl->device);
        ENGINE_free(engine);
    }
    return 0;
}
#endif


#if ME_MPR_HAS_ALPN
static int selectAlpn(SSL *ssl, cuchar **out, uchar *outlen, cuchar *in, uint inlen, void *arg)
{
    OpenSocket  *osp;
    cchar       *alpn;

    if ((osp = (OpenSocket*) SSL_get_app_data(ssl)) == 0) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    alpn = osp->cfg->alpn;
    if (alpn == 0) {
        return SSL_TLSEXT_ERR_NOACK;
    }

    /*
        WARNING: this appalling API expects pbuf to be static / persistent and sets *out to refer to it.
     */
    if (SSL_select_next_proto((uchar **) out, outlen, (cuchar*) alpn, (int) slen(alpn), in, inlen) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}
#endif


#if ME_MPR_HAS_ALPN
static cchar *makeAlpn(MprSsl *ssl)
{
    cchar   *proto;
    char    *dp, *pbuf;
    ssize   len, plen;
    int     next;

    len = 1;
    for (ITERATE_ITEMS(ssl->alpn, proto, next)) {
        len += slen(proto) + 1;
    }
    pbuf = mprAlloc(len);
    dp = pbuf;
    for (ITERATE_ITEMS(ssl->alpn, proto, next)) {
        plen = slen(proto);
        *dp++ = (uchar) plen;
        scopy((char*) dp, len - (dp - pbuf), proto);
        dp += plen;
    }
    return pbuf;
}
#endif

/*
    MPR Garbage collector manager callback for OpenSocket. Called on each GC sweep.
 */
static void manageOpenSocket(OpenSocket *osp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(osp->sock);
        mprMark(osp->cfg);
        mprMark(osp->requiredPeerName);

    } else if (flags & MPR_MANAGE_FREE) {
        if (osp->handle) {
            SSL_set_shutdown(osp->handle, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
            SSL_free(osp->handle);
            osp->handle = 0;
        }
    }
}


static void closeOss(MprSocket *sp, bool gracefully)
{
    OpenSocket    *osp;

    osp = sp->sslSocket;
    if (osp->handle) {
        SSL_shutdown(osp->handle);
    }
    sp->service->standardProvider->closeSocket(sp, gracefully);
    if (osp->handle) {
        SSL_free(osp->handle);
        osp->handle = 0;
    }
}


static int preloadOss(MprSsl *ssl, int flags)
{
    char    *errorMsg;

    assert(ssl);

    if (ssl == 0) {
        ssl = mprCreateSsl(flags & MPR_SOCKET_SERVER);
    }
    lock(ssl);
    if (configOss(ssl, flags, &errorMsg) < 0) {
        mprLog("error mpr ssl openssl", 4, "Cannot configure SSL %s", errorMsg);
        unlock(ssl);
        return MPR_ERR_CANT_INITIALIZE;
    }
    unlock(ssl);
    return 0;
}


/*
    Upgrade a standard socket to use SSL/TLS. Used by both clients and servers to upgrade a socket for SSL.
    If a client, this may block while connecting.
 */
static int upgradeOss(MprSocket *sp, MprSsl *ssl, cchar *requiredPeerName)
{
    OpenSocket      *osp;
    OpenConfig      *cfg;
    int             rc;

    assert(sp);

    if (ssl == 0) {
        ssl = mprCreateSsl(sp->flags & MPR_SOCKET_SERVER);
    }
    if ((osp = (OpenSocket*) mprAllocObj(OpenSocket, manageOpenSocket)) == 0) {
        return MPR_ERR_MEMORY;
    }
    osp->sock = sp;
    sp->sslSocket = osp;
    sp->ssl = ssl;

    lock(ssl);
    if (configOss(ssl, sp->flags, &sp->errorMsg) < 0) {
        unlock(ssl);
        return MPR_ERR_CANT_INITIALIZE;
    }
    unlock(ssl);

    /*
        Create and configure the SSL struct
     */
    cfg = osp->cfg = sp->ssl->config;
    if ((osp->handle = (SSL*) SSL_new(cfg->ctx)) == 0) {
        return MPR_ERR_BAD_STATE;
    }
    SSL_set_app_data(osp->handle, (void*) osp);

    /*
        Create a socket bio. We don't use the BIO except as storage for the fd
     */
    if ((osp->bio = BIO_new_socket((int) sp->fd, BIO_NOCLOSE)) == 0) {
        return MPR_ERR_BAD_STATE;
    }
    SSL_set_bio(osp->handle, osp->bio, osp->bio);

    if (sp->flags & MPR_SOCKET_SERVER) {
        SSL_set_accept_state(osp->handle);
    } else {
        if (requiredPeerName) {
            osp->requiredPeerName = sclone(requiredPeerName);
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
            X509_VERIFY_PARAM *param = param = SSL_get0_param(osp->handle);
            X509_VERIFY_PARAM_set_hostflags(param, 0);
            X509_VERIFY_PARAM_set1_host(param, requiredPeerName, 0);
#endif
            //MOB
            SSL_set_tlsext_host_name(osp->handle, requiredPeerName);
        }
        /*
            Block while connecting
         */
        mprSetSocketBlockingMode(sp, 1);
        sp->errorMsg = 0;
        if ((rc = SSL_connect(osp->handle)) < 1) {
            if (sp->errorMsg) {
                mprLog("info mpr ssl openssl", 4, "Connect failed: %s", sp->errorMsg);
            } else {
                mprLog("info mpr ssl openssl", 4, "Connect failed: error %s", getOssError(sp));
            }
            return MPR_ERR_CANT_CONNECT;
        }
        if (rc > 0 && checkPeerCertName(sp) < 0) {
            return MPR_ERR_CANT_CONNECT;
        }
        setSecured(sp);
        mprSetSocketBlockingMode(sp, 0);
    }
    if (ME_MPR_SSL_HANDSHAKES) {
        SSL_CTX_set_info_callback(cfg->ctx, infoCallback);
    }
    return 0;
}


static void infoCallback(const SSL *ssl, int where, int rc)
{
    OpenSocket  *osp;

    if (where & SSL_CB_HANDSHAKE_START) {
        if ((osp = (OpenSocket*) SSL_get_app_data(ssl)) == 0) {
            return;
        }
        osp->handshakes++;
    }
}


static void disconnectOss(MprSocket *sp)
{
    sp->service->standardProvider->disconnectSocket(sp);
}


/*
    Return the number of bytes read. Return -1 on errors and EOF. Distinguish EOF via mprIsSocketEof.
    If non-blocking, may return zero if no data or still handshaking.
 */
static ssize readOss(MprSocket *sp, void *buf, ssize len)
{
    OpenSocket      *osp;
    int             rc, error, retries, i;

    osp = (OpenSocket*) sp->sslSocket;
    assert(osp);

    if (osp->handle == 0) {
        return MPR_ERR_BAD_STATE;
    }
    /*
        Limit retries on WANT_READ. If non-blocking and no data, then this could spin forever.
     */
    retries = 5;
    for (i = 0; i < retries; i++) {
        rc = SSL_read(osp->handle, buf, (int) len);
        if (rc <= 0) {
            error = SSL_get_error(osp->handle, rc);
            if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_CONNECT || error == SSL_ERROR_WANT_ACCEPT) {
                continue;
            }
            mprLog("info mpr ssl openssl", 5, "SSL_read %s", getOssError(sp));
            sp->flags |= MPR_SOCKET_EOF | MPR_SOCKET_ERROR;
        }
        break;
    }
    if (osp->cfg->maxHandshakes && osp->handshakes > osp->cfg->maxHandshakes) {
        mprLog("error mpr ssl openssl", 4, "TLS renegotiation attack");
        rc = -1;
        sp->flags |= MPR_SOCKET_EOF | MPR_SOCKET_ERROR;
        return MPR_ERR_BAD_STATE;
    }
    if (rc <= 0) {
        error = SSL_get_error(osp->handle, rc);
        if (error == SSL_ERROR_WANT_READ) {
            rc = 0;
        } else if (error == SSL_ERROR_WANT_WRITE) {
            rc = 0;
        } else if (error == SSL_ERROR_ZERO_RETURN) {
            sp->flags |= MPR_SOCKET_EOF;
            rc = -1;
        } else if (error == SSL_ERROR_SYSCALL) {
            sp->flags |= MPR_SOCKET_EOF | MPR_SOCKET_ERROR;
            rc = -1;
        } else if (error != SSL_ERROR_ZERO_RETURN) {
            /* SSL_ERROR_SSL */
            mprLog("info mpr ssl openssl", 4, "%s", getOssError(sp));
            rc = -1;
            sp->flags |= MPR_SOCKET_EOF | MPR_SOCKET_ERROR;
        }
    } else {
        if (!sp->secured) {
            if (sp->flags & MPR_SOCKET_SERVER) {
                if (sp->ssl && smatch(sp->ssl->verifyPeer, "require") && !sp->peerCert) {
                    sp->errorMsg = sfmt("Required certificate not presented by client");
                    sp->flags |= MPR_SOCKET_EOF | MPR_SOCKET_CERT_ERROR;
                    return MPR_ERR_BAD_STATE;
                }
            } else if (checkPeerCertName(sp) < 0) {
                sp->flags |= MPR_SOCKET_EOF | MPR_SOCKET_CERT_ERROR;
                return MPR_ERR_BAD_STATE;
            }
            setSecured(sp);
        }
        mprHiddenSocketData(sp, SSL_pending(osp->handle), MPR_READABLE);
    }
    return rc;
}


/*
    Write data. Return the number of bytes written or -1 on errors.
 */
static ssize writeOss(MprSocket *sp, cvoid *buf, ssize len)
{
    OpenSocket  *osp;
    ssize       totalWritten;
    int         error, rc;

    osp = (OpenSocket*) sp->sslSocket;

    if (osp->bio == 0 || osp->handle == 0 || len <= 0) {
        return MPR_ERR_BAD_STATE;
    }
    totalWritten = 0;
    ERR_clear_error();
    error = 0;

    do {
        rc = SSL_write(osp->handle, buf, (int) len);
        mprLog("info mpr ssl openssl", 7, "Wrote %d, requested len %zd", rc, len);
        if (rc <= 0) {
            error = SSL_get_error(osp->handle, rc);
            if (error == SSL_ERROR_WANT_WRITE) {
                break;
            }
            return MPR_ERR_CANT_WRITE;
        } else if (osp->cfg->maxHandshakes && osp->handshakes > osp->cfg->maxHandshakes) {
            mprLog("error mpr ssl openssl", 4, "TLS renegotiation attack");
            rc = -1;
            sp->flags |= MPR_SOCKET_EOF;
            return MPR_ERR_BAD_STATE;
        }
        totalWritten += rc;
        buf = (void*) ((char*) buf + rc);
        len -= rc;
        mprLog("info mpr ssl openssl", 7, "write len %zd, written %d, total %zd", len, rc, totalWritten);
    } while (len > 0);

    if (totalWritten == 0 && error == SSL_ERROR_WANT_WRITE) {
        mprSetError(EAGAIN);
        return MPR_ERR_NOT_READY;
    }
    return totalWritten;
}


static ssize flushOss(MprSocket *sp)
{
    return 0;
}


static char *getOssSession(MprSocket *sp)
{
    SSL_SESSION     *sess;
    OpenSocket      *osp;
    MprBuf          *buf;
    int             i;

    osp = sp->sslSocket;

    if ((sess = SSL_get0_session(osp->handle)) != 0) {
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
        uint len;
        cuchar *id = SSL_SESSION_get_id(sess, &len);
        if (len == 0) {
            return sclone("ticket");
        }
        buf = mprCreateBuf((len * 2) + 1, 0);
        for (i = 0; i < (int) len ; i++) {
            mprPutToBuf(buf, "%02X", (uchar) id[i]);
        }
#else
        if (sess->session_id_length == 0 && osp->handle->tlsext_ticket_expected) {
            return sclone("ticket");
        }
        buf = mprCreateBuf((sess->session_id_length * 2) + 1, 0);
        for (i = 0; i < (int) sess->session_id_length; i++) {
            mprPutToBuf(buf, "%02X", (uchar) sess->session_id[i]);
        }
#endif
        return mprBufToString(buf);
    }
    return 0;
}


#if DEPRECATED_JSON_STATE
/*
    Parse the cert info and write properties to the buffer. Modifies the info argument.
 */
static void parseCertFields(MprBuf *buf, char *info)
{
    char    c, *cp, *term, *key, *value;

    if (info) {
        key = 0;
        term = cp = info;
        do {
            c = *cp;
            if (c == '/' || c == '\0') {
                *cp = '\0';
                key = ssplit(term, "=", &value);
                if (smatch(key, "emailAddress")) {
                    key = "email";
                }
                mprPutToBuf(buf, "\"%s\":\"%s\",", key, value);
                term = &cp[1];
                *cp = c;
            }
        } while (*cp++ != '\0');
        if (key) {
            mprAdjustBufEnd(buf, -1);
        }
    }
}


/*
    Get the SSL state of the socket in a buffer
 */
static char *getOssState(MprSocket *sp)
{
    OpenSocket      *osp;
    MprBuf          *buf;
    X509            *cert;
    char            subject[512], issuer[512];

    osp = sp->sslSocket;
    buf = mprCreateBuf(0, 0);

    mprPutToBuf(buf, "{\"provider\":\"openssl\",\"cipher\":\"%s\",\"session\":\"%s\",",
        SSL_get_cipher(osp->handle), sp->session);
    mprPutToBuf(buf, "\"peer\":\"%s\",", sp->peerName);
    mprPutToBuf(buf, "\"%s\":{", sp->acceptIp ? "client" : "server");

    if ((cert = getPeerCert(osp->handle)) != 0) {
        X509_NAME_oneline(X509_get_issuer_name(cert), issuer, sizeof(issuer) -1);
        mprPutToBuf(buf, "\"issuer\": {");
        parseCertFields(buf, &issuer[1]);
        mprPutToBuf(buf, "},");

        X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject) -1);
        mprPutToBuf(buf, "\"subject\": {");
        parseCertFields(buf, &subject[1]);
        mprPutToBuf(buf, "},");
        X509_free(cert);
    }
    if ((cert = SSL_get_certificate(osp->handle)) != 0) {
        mprPutToBuf(buf, "\"issuer\": {");
        X509_NAME_oneline(X509_get_issuer_name(cert), issuer, sizeof(issuer) -1);
        parseCertFields(buf, &issuer[1]);
        mprPutToBuf(buf, "},");

        mprPutToBuf(buf, "\"subject\": {");
        X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject) -1);
        parseCertFields(buf, &subject[1]);
        mprPutToBuf(buf, "},");
        /* Don't call X509_free on own cert */
    }
    mprAdjustBufEnd(buf, -1);
    mprPutToBuf(buf, "}}");
    return mprBufToString(buf);
}
#else

/*
    Parse the cert info and write properties to the buffer. Modifies the info argument.
 */
static void parseCertFields(MprBuf *buf, cchar *prefix, cchar *prefix2, char *info)
{
    char    c, *cp, *term, *key, *value;

    if (info) {
        term = cp = info;
        do {
            c = *cp;
            if (c == '/' || c == '\0') {
                *cp = '\0';
                key = ssplit(term, "=", &value);
                if (smatch(key, "emailAddress")) {
                    key = "EMAIL";
                }
                mprPutToBuf(buf, "%s%s%s=%s,", prefix, prefix2, key, value);
                term = &cp[1];
                *cp = c;
            }
        } while (*cp++ != '\0');
    }
}


static char *getOssState(MprSocket *sp)
{
    OpenSocket      *osp;
    MprBuf          *buf;
    X509_NAME       *xSubject;
    X509            *cert;
    char            *prefix, subject[512], issuer[512], peer[512];

    osp = sp->sslSocket;
    buf = mprCreateBuf(0, 0);

    mprPutToBuf(buf, "PROVIDER=openssl,CIPHER=%s,SESSION=%s,", SSL_get_cipher(osp->handle), sp->session);

    if ((cert = getPeerCert(osp->handle)) == 0) {
        mprPutToBuf(buf, "%s=none,", sp->acceptIp ? "CLIENT_CERT" : "SERVER_CERT");

    } else {
        xSubject = X509_get_subject_name(cert);
        X509_NAME_get_text_by_NID(xSubject, NID_commonName, peer, sizeof(peer) - 1);
        mprPutToBuf(buf, "PEER=%s,", peer);

        prefix = sp->acceptIp ? "CLIENT_" : "SERVER_";
        X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject) -1);
        parseCertFields(buf, prefix, "S_", &subject[1]);

        X509_NAME_oneline(X509_get_issuer_name(cert), issuer, sizeof(issuer) -1);
        parseCertFields(buf, prefix, "I_", &issuer[1]);
        X509_free(cert);
    }
    if ((cert = SSL_get_certificate(osp->handle)) != 0) {
        prefix =  sp->acceptIp ? "SERVER_" : "CLIENT_";
        X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject) -1);
        parseCertFields(buf, prefix, "S_", &subject[1]);

        X509_NAME_oneline(X509_get_issuer_name(cert), issuer, sizeof(issuer) -1);
        parseCertFields(buf, prefix, "I_", &issuer[1]);
        // Don't call X509_free on own cert 
    }
    mprAdjustBufEnd(buf, -1);
    return mprGetBufStart(buf);
}

#endif // DEPRECATE_JSON_STATE


/*
    Check the certificate peer name validates and matches the desired name
    Used for client side
 */
static int checkPeerCertName(MprSocket *sp)
{
    OpenSocket  *osp;
    X509        *cert;
    X509_NAME   *xSubject;
    char        subject[512], issuer[512], peerName[512];

    osp = (OpenSocket*) sp->sslSocket;

    if ((cert = getPeerCert(osp->handle)) == 0) {
        peerName[0] = '\0';
    } else {
        xSubject = X509_get_subject_name(cert);
        X509_NAME_oneline(xSubject, subject, sizeof(subject) -1);
        X509_NAME_oneline(X509_get_issuer_name(cert), issuer, sizeof(issuer) -1);
        X509_NAME_get_text_by_NID(xSubject, NID_commonName, peerName, sizeof(peerName) - 1);
        sp->peerName = sclone(peerName);
        sp->peerCert = sclone(subject);
        sp->peerCertIssuer = sclone(issuer);
        X509_free(cert);
    }
#if OPENSSL_VERSION_NUMBER < 0x10002000L
    MprSsl  *ssl;

    ssl = sp->ssl;
    if (ssl->verifyPeer && osp->requiredPeerName) {
        char    *target, *certName, *tp;

        target = osp->requiredPeerName;
        certName = peerName;

        if (target == 0 || *target == '\0' || strchr(target, '.') == 0) {
            sp->errorMsg = sfmt("Bad peer name");
            sp->flags |= MPR_SOCKET_CERT_ERROR;
            return MPR_ERR_BAD_VALUE;
        }
        if (!smatch(certName, "localhost")) {
            if (strchr(certName, '.') == 0) {
                sp->errorMsg = sfmt("Peer certificate must have a domain: \"%s\"", certName);
                sp->flags |= MPR_SOCKET_CERT_ERROR;
                return MPR_ERR_BAD_VALUE;
            }
            if (*certName == '*' && certName[1] == '.') {
                /* Wildcard cert */
                certName = &certName[2];
                if (strchr(certName, '.') == 0) {
                    /* Peer must be of the form *.domain.tld. i.e. *.com is not valid */
                    sp->errorMsg = sfmt("Peer CN is not valid %s", peerName);
                    sp->flags |= MPR_SOCKET_CERT_ERROR;
                    return MPR_ERR_BAD_VALUE;
                }
                if ((tp = strchr(target, '.')) != 0 && strchr(&tp[1], '.')) {
                    /* Strip host name if target has a host name */
                    target = &tp[1];
                }
            }
        }
        if (!smatch(target, certName)) {
            sp->errorMsg = sfmt("Certificate common name mismatch CN \"%s\" vs required \"%s\"", peerName,
                osp->requiredPeerName);
            sp->flags |= MPR_SOCKET_CERT_ERROR;
            return MPR_ERR_BAD_VALUE;
        }
    }
#endif
    return 0;
}


/*
    Load a certificate into the context from the supplied buffer. Type indicates the desired format. The path is only used for errors.
 */
static int loadCert(SSL_CTX *ctx, cchar *buf, ssize len, int type, cchar *path)
{
    X509    *cert;
    BIO     *bio;
    bool    loaded;

    assert(ctx);
    assert(buf);
    assert(type);
    assert(path && *path);

    cert = 0;
    loaded = 0;

    if ((bio = BIO_new_mem_buf(buf, (int) len)) == 0) {
        mprLog("error openssl", 0, "Unable to allocate memory for certificate %s", path);
    } else {
        if (type == FORMAT_PEM) {
            if ((cert = PEM_read_bio_X509(bio, NULL, 0, NULL)) == 0) {
                /* Error reported by caller if loading all formats fail */
            }
        } else if (type == FORMAT_DER) {
            if ((cert = d2i_X509_bio(bio, NULL)) == 0) {
                /* Error reported by caller */
            }
        }
        if (cert) {
            if (SSL_CTX_use_certificate(ctx, cert) != 1) {
                mprLog("error openssl", 0, "Unable to use certificate %s", path);
            } else {
                loaded = 1;
            }
        }
    }
    if (bio) {
        BIO_free(bio);
    }
    if (cert) {
        X509_free(cert);
    }
    return loaded ? 0 : MPR_ERR_CANT_LOAD;
}


static X509 *getPeerCert(SSL *handle)
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    return SSL_get1_peer_certificate(handle);
#else
    return SSL_get_peer_certificate(handle);
#endif
}


/*
    Load a certificate file in either PEM or DER format
 */
static int setCertFile(SSL_CTX *ctx, cchar *certFile)
{
    cchar   *buf;
    ssize   len;
    int     rc;

    assert(ctx);
    assert(certFile);

    rc = 0;

    if (ctx == NULL || certFile == NULL) {
        return rc;
    }
    if ((buf = mprReadPathContents(certFile, &len)) == 0) {
        mprLog("error openssl", 0, "Unable to read certificate %s", certFile);
        rc = MPR_ERR_CANT_READ;
    } else {
        if (loadCert(ctx, buf, len, FORMAT_PEM, certFile) < 0 && loadCert(ctx, buf, len, FORMAT_DER, certFile) < 0) {
            mprLog("error openssl", 0, "Unable to load certificate %s", certFile);
            rc = MPR_ERR_CANT_LOAD;
        }
    }
    return rc;
}


/*
    Load a key into the context from the supplied buffer. Type indicates the key format.  Path only used for diagnostics.
 */
static int loadKey(SSL_CTX *ctx, cchar *buf, ssize len, int type, cchar *path)
{
    RSA     *key;
    BIO     *bio;
    bool    loaded;

    assert(ctx);
    assert(buf);
    assert(type);
    assert(path && *path);

    key = 0;
    loaded = 0;

    if ((bio = BIO_new_mem_buf(buf, (int) len)) == 0) {
        mprLog("error openssl", 0, "Unable to allocate memory for key %s", path);
    } else {
        if (type == FORMAT_PEM) {
            if ((key = PEM_read_bio_RSAPrivateKey(bio, NULL, 0, NULL)) == 0) {
                /* Error reported by caller if loading all formats fail */
            }
        } else if (type == FORMAT_DER) {
            if ((key = d2i_RSAPrivateKey_bio(bio, NULL)) == 0) {
                /* Error reported by caller if loading all formats fail */
            }
        }
    }
    if (key) {
        if (SSL_CTX_use_RSAPrivateKey(ctx, key) != 1) {
            mprLog("error openssl", 0, "Unable to use key %s", path);
        } else {
            loaded = 1;
        }
    }
    if (bio) {
        BIO_free(bio);
    }
    if (key) {
        RSA_free(key);
    }
    return loaded ? 0 : MPR_ERR_CANT_LOAD;
}


#if MPR_HAS_CRYPTO_ENGINE
/*  
	Key path format is: engine:NAME:key
 */
static int loadEngineKey(SSL_CTX *ctx, cchar *path)
{
    ENGINE      *engine;
    EVP_PKEY    *pkey;
    char        *key, *name;
    
    stok(sclone(path), ":", &name);
    stok(name, ":", &key);

    engine = ENGINE_by_id(name);
    if (!engine) {
        mprLog("openssl error", 0, "Cannot find engine %s", name);
        ENGINE_free(engine);
        return MPR_ERR_CANT_FIND;
    }
    if (!ENGINE_set_default(engine, ENGINE_METHOD_ALL)) {
        mprLog("mpr ssl openssl error", 0, "Cannot find crypto device %s", name);
        ENGINE_free(engine);
        return MPR_ERR_CANT_FIND;
    }
    if (!ENGINE_init(engine)) {
        mprLog("openssl error", 0, "Cannot initialize engine %s", name);
        ENGINE_free(engine);
        return MPR_ERR_CANT_INITIALIZE;
    }
    if ((pkey = ENGINE_load_private_key(engine, key, 0, 0)) == 0) {
        mprLog("openssl error", 0, "Cannot read private key %s", key);
        ENGINE_finish(engine);
        ENGINE_free(engine);
        return MPR_ERR_CANT_READ;
    }
    SSL_CTX_use_PrivateKey(ctx, pkey);
    ENGINE_finish(engine);
    ENGINE_free(engine);
    return 0;
}
#endif


/*
    Load a key file in either PEM or DER format
 */
static int setKeyFile(SSL_CTX *ctx, cchar *keyFile)
{
    RSA     *key;
    char    *buf;
    ssize   len;
    int     rc;

    assert(ctx);
    assert(keyFile);

    key = 0;
    buf = 0;
    rc = 0;

    if (ctx == NULL || keyFile == NULL) {
        ;
#if MPR_HAS_CRYPTO_ENGINE
    } else if (sstarts(keyFile, "engine:")) {
        loadEngineKey(ctx, keyFile);
#endif
    } else if ((buf = mprReadPathContents(keyFile, &len)) == 0) {
        mprLog("error openssl", 0, "Unable to read key %s", keyFile);
        rc = MPR_ERR_CANT_READ;
    } else {
        if (loadKey(ctx, buf, len, FORMAT_PEM, keyFile) < 0 && loadKey(ctx, buf, len, FORMAT_DER, keyFile) < 0) {
            mprLog("error openssl", 0, "Unable to load key %s", keyFile);
            rc = MPR_ERR_CANT_LOAD;
        }
    }
    if (key) {
        RSA_free(key);
    }
    return rc;
}


static int verifyPeerCertificate(int ok, X509_STORE_CTX *xctx)
{
    X509            *cert;
    SSL             *handle;
    OpenSocket      *osp;
    MprSocket       *sp;
    MprSsl          *ssl;
    char            subject[512], issuer[512], peerName[512];
    int             error, depth;

    subject[0] = issuer[0] = '\0';
    handle = (SSL*) X509_STORE_CTX_get_ex_data(xctx, SSL_get_ex_data_X509_STORE_CTX_idx());
    osp = (OpenSocket*) SSL_get_app_data(handle);
    sp = osp->sock;
    ssl = sp->ssl;

    cert = X509_STORE_CTX_get_current_cert(xctx);
    depth = X509_STORE_CTX_get_error_depth(xctx);
    error = X509_STORE_CTX_get_error(xctx);

    ok = 1;
    if (X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject) - 1) < 0) {
        sp->errorMsg = sclone("Cannot get subject name");
        ok = 0;
    }
    sp->peerCert = sclone(subject);

    if (X509_NAME_oneline(X509_get_issuer_name(cert), issuer, sizeof(issuer) - 1) < 0) {
        sp->errorMsg = sclone("Cannot get issuer name");
        ok = 0;
    }
    sp->peerCertIssuer = sclone(issuer);

    if (X509_NAME_get_text_by_NID(X509_get_subject_name(cert), NID_commonName, peerName, sizeof(peerName) - 1) == 0) {
        sp->errorMsg = sclone("Cannot get peer name");
        ok = 0;
    }
    sp->peerName = sclone(peerName);
    
    if (ok && ssl->verifyDepth < depth) {
        if (error == 0) {
            error = X509_V_ERR_CERT_CHAIN_TOO_LONG;
        }
    }
    switch (error) {
    case X509_V_OK:
        break;
    case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
    case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
        /* Normal self signed certificate */
        if (ssl->verifyIssuer) {
            sp->errorMsg = sclone("Self-signed certificate");
            ok = 0;
        }
        break;

    case X509_V_ERR_CERT_UNTRUSTED:
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
        if (ssl->verifyIssuer) {
            /* Issuer cannot be verified */
            sp->errorMsg = sclone("Certificate not trusted");
            ok = 0;
        }
        break;

    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
    case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
        if (ssl->verifyIssuer) {
            /* Issuer cannot be verified */
            sp->errorMsg = sclone("Certificate not trusted");
            ok = 0;
        }
        break;

    case X509_V_ERR_CERT_HAS_EXPIRED:
        sp->errorMsg = sfmt("Certificate has expired");
        ok = 0;
        break;

#ifdef X509_V_ERR_HOSTNAME_MISMATCH
    case X509_V_ERR_HOSTNAME_MISMATCH:
        sp->errorMsg = sfmt("Certificate hostname mismatch. Expecting %s got %s", osp->requiredPeerName, peerName);
        ok = 0;
        break;
#endif
    case X509_V_ERR_CERT_CHAIN_TOO_LONG:
    case X509_V_ERR_CERT_NOT_YET_VALID:
    case X509_V_ERR_CERT_REJECTED:
    case X509_V_ERR_CERT_SIGNATURE_FAILURE:
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
    case X509_V_ERR_INVALID_CA:
    default:
        sp->errorMsg = sfmt("Certificate verification error %d", error);
        ok = 0;
        break;
    }
    if (!ok) {
        sp->flags |= MPR_SOCKET_CERT_ERROR;
    }
    return ok;
}


static int sniHostname(SSL *handle, int *ad, void *arg)
{
    MprSocket       *sp;
    MprSsl          *ssl;
    OpenSocket      *osp;
    OpenConfig      *cfg;
    SSL_CTX         *ctx;
    cchar           *hostname;

    if (!handle) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    osp = (OpenSocket*) SSL_get_app_data(handle);
    sp = osp->sock;

    hostname = SSL_get_servername(handle, TLSEXT_NAMETYPE_host_name);

    /*
        Select the appropriate SSL for this hostname
     */
    if ((ssl = (sp->ssl->matchSsl)(sp, hostname)) == 0) {
        return SSL_TLSEXT_ERR_ALERT_FATAL;
    }
    lock(ssl);
    if (configOss(ssl, sp->flags, &sp->errorMsg) < 0) {
        unlock(ssl);
        return SSL_TLSEXT_ERR_ALERT_FATAL;
    }
    unlock(ssl);

    sp->ssl = ssl;
    cfg = ssl->config;
    ctx = cfg->ctx;
    SSL_set_SSL_CTX(handle, ctx);

    return SSL_TLSEXT_ERR_OK;
}


static void setSecured(MprSocket *sp)
{
    OpenSocket    *osp;

    sp->secured = 1;
    osp = sp->sslSocket;
    sp->cipher = sclone(SSL_get_cipher(osp->handle));
    sp->protocol = sclone(SSL_get_version(osp->handle));
    sp->session = getOssSession(sp);
}


static char *getOssError(MprSocket *sp)
{
    char    ebuf[ME_BUFSIZE];
    ulong   error;

    error = ERR_get_error();
    ERR_error_string_n(error, ebuf, sizeof(ebuf) - 1);
    sp->errorMsg = sclone(ebuf);
    return sp->errorMsg;
}


/*
    Map iana names to OpenSSL names so users can provide IANA names as well as OpenSSL cipher names
 */
static cchar *mapCipherNames(cchar *ciphers)
{
    MprBuf      *buf;
    CipherMap   *cp;
    char        *cipher, *next;

    if (!ciphers || *ciphers == 0) {
        return 0;
    }
    buf = mprCreateBuf(0, 0);
    for (next = sclone(ciphers); (cipher = stok(next, ":, \t", &next)) != 0; ) {
        for (cp = cipherMap; cp->name; cp++) {
            if (smatch(cp->name, cipher)) {
                mprPutToBuf(buf, "%s:", cp->ossName);
                break;
            }
        }
        if (cp->name == 0) {
            mprPutToBuf(buf, "%s:", cipher);
        }
    }
    return mprBufToString(buf);
}


/*
    DH Parameters from RFC3526
    Alternatively, replace with your own DH params generated via:
    openssl dhparam --out dh.c -C 2048
 */

static DH *getDhKey()
{
    static unsigned char dh2048_p[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
        0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
        0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
        0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
        0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
        0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
        0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
        0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
        0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
        0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
        0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
        0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
        0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
        0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
        0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
        0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    };
    static unsigned char dh2048_g[] = {
        0x02,
    };
    DH      *dh;

    if ((dh = DH_new()) == 0) {
        return 0;
    }
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
{
    BIGNUM  *p, *g;
    p = BN_bin2bn(dh2048_p, sizeof(dh2048_p), NULL);
    g = BN_bin2bn(dh2048_g, sizeof(dh2048_g), NULL);
    if (!DH_set0_pqg(dh, p, NULL, g)) {
        BN_free(p);
        BN_free(g);
        DH_free(dh);
        return 0;
    }
}
#else
    dh->p = BN_bin2bn(dh2048_p, sizeof(dh2048_p), NULL);
    dh->g = BN_bin2bn(dh2048_g, sizeof(dh2048_g), NULL);
    if ((dh->p == 0) || (dh->g == 0)) {
        DH_free(dh);
        return 0;
    }
#endif
    return dh;
}


/*
    Set the ephemeral DH key
 */
static DH *dhcallback(SSL *handle, int isExport, int keyLength)
{
    OpenSocket      *osp;
    OpenConfig      *cfg;

    osp = (OpenSocket*) SSL_get_app_data(handle);
    cfg = osp->sock->ssl->config;
    return cfg->dhKey;
}


#endif /* ME_COM_OPENSSL */

/*
    Copyright (c) Embedthis Software. All Rights Reserved.
    This software is distributed under a commercial license. Consult the LICENSE.md
    distributed with this software for full details and copyrights.
 */
