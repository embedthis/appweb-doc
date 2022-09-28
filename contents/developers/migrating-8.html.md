{
    title:  'Migrating',
    crumbs: [
        { "Developer's Guide": '../developers/' },
    ],
}

# Migrating to Appweb 8

## Architectural Changes

Appweb 8 includes some major architectural changes to support the [HTTP/2](https://en.wikipedia.org/wiki/HTTP/2) protocol. HTTP/2 is a higher performance binary protocol that supports multiplexing multiple request streams over a single network connection and offers decreased latency and improved efficiency. This necessitates some API changes and so handlers and filters may need some minor refactoring to work with Appweb 8. ESP, CGI and PHP applications should not need any changes.

The architectural changes in Appweb 8 are:

* HTTP/2 protocol support and filter
* Improved pipeline handling to support HTTP/2 multiplexing
* New HttpNet structure defines the network socket that is shared among requests.
* The Upload filter is enabled by default and always part of pipeline
* Upload directory now not per route — needs to be defined before routing
* New Auth Type "app" for application defined authentication
* Removed the Send Connector as all I/O must now go through the Net Connector
* Improved packet level trace
* Improved OpenSSL support for either 1.0 or 1.2
* Improved PHP support for either PHP 5 or 7
* ESP improvements to make compiling more flexible and manage larger applications

## Changed APIs

To assist with migrating to Appweb 8 you can set the preprocessor constant ME_COMPAT to "1" which will map prior definitions into the Appweb 8 equivalents. You should use this as a transitional aid only as these mappings will be removed in future releases.

### Networks and Connections

The previous HttpConn structure contained information regarding the network connection and the current request. With HTTP/2 a single network connection must support multiple simultaneous requests. To support this, the HttpNet structure is introduced and it assumes the management of the network connection. The HttpConn structure is renamed HttpStream for clarity as it is no-longer responsible for the connection. It retains responsibility for a single request / response exchange with the peer.


For compatibility, a CPP define is provided for HttpConn which maps to HttpStream. Similarly, defines are provided which map legacy HttpConn APIs to their HttpStream equivalents. However, you should refactor your applications and rename all HttpConn references to HttpStream. You should also change your "conn" variable declarations to be "stream" for clarity.

Some HttpConn APIs have been migrated to HttpNet and consequently now take a HttpNet* parameter as their argument. Note: the HttpNet object owns the event dispatcher which is now shared across all HttpStream instances so that all activity on a network socket is serialized and non-blocking.

Here is a list of the API changes. Please let us know at [dev@embedthis.com](mailto:dev@embedthis.com) if we have missed any impacting change and we'll update the list.

- Added HttpNet structure and httpNet APIs including: httpCreateNet, httpDestroyNet, ...
- The httpConn APIs are renamed to httpStream.
- These APIs which previously operated on HttpConn/HttpStream objects now operating on HttpNet instances: httpSetAsync, httpServiceQueues, httpEnableConnEvents, httpIOEvent, httpStealSocket.
- Changed httpRequest signature to add a protocol argument to nominate HTTP/1 or HTTP/2 for client requests. See [httpRequest](http://127.0.0.1:4000/ref/api/http.html#group___http_tx_1gae53c8deff659f3aa5aec8aaeb9d7673e).
- Changed httpTrace first argument from HttpConn to HttpTrace so that this API can be used for both HttpNet and HttpStream related trace. Use HttpStream.trace or HttpNet.trace as the trace instance for this API. See [httpTrace](http://127.0.0.1:4000/ref/api/http.html#group___http_trace_1ga5f8b026696d0a7285da9a60046483348).
- The httpCreateConn API is renamed to httpCreateStream and its arguments are changed to take a HttpNet argument and a desired protocol. An HttpNet instance must be created prior to creating a HttpStream. See [httpCreateconn](http://127.0.0.1:4000/ref/api/http.html#group___http_stream_1ga2203fa59ea98e0a35417dde387123d29).
- Web sockets ESP callbacks must now be prepared to handle multiple packets on their READABLE events. Previously Appweb 7 would only provide one packet per call.
- The "AddConnector" configuration directive is removed. The net connector is the only supported connector.
- Added the httpSetAuthStoreVerify API for globally setting the auth verify callback. See [httpSetAuthStoreVerify](http://127.0.0.1:4000/ref/api/http.html#group___http_auth_1gaceb42561636cf7ff37c67b167c8df128).
- Removed HttpTx.queues[]. These were aliases to HttpStream.readq and HttpStream.writeq which should be used instead.
- HttpQueue.conn is renamed HttpQueue.stream. User code that references q->conn should be changed to q->stream.
- The httpWriteBlock API when called with HTTP_BUFFER may yield. This is required to support HTTP/2 which uses flow control packets that must be received to permit output to flow.
- The espRender, espRenderString, espRenderBlock APIs may yield.
- The httpFinalize* APIs takes queue instead of HttpStream. Applications should use the queue HttpStream.writeq when calling httpFinalize APIs.
- All handlers must call httpFinalizeOutput / httpFinalize to finalize output.
- The httpNeedRetry API changed the URL argument to cchar* from char*.
- The httpSetRetries API and HttpConn.retries field are removed.
- The HttpTx.writeBlocked is migrated to HttpNet.writeBlocked.
- The define ME_MAX_BUFFER should be migrated to use ME_BUFSIZE instead.
- The httpIOEvent(stream) API now takes a HttpNet argument.
- The httpTracePacket API "body" argument is changed to be a HttpPacket reference.
- The Upload filter directory now not per route, as it must be defined before routing can proceed.
- The Upload filter always defined — to remove, do Reset pipeline and define route configuration. The PHP handler will do this automatically as PHP implements its own file upload.

## ESP Changes

Here is a list of ESP changes:

- The esp.json "esp.app.source" configuration directive can specify a list of source files to build. Previously, ESP would only compile app.c.
- Support "esp.app.tokens" collection to specify CFLAGS, DFLAGS, LDFLAGS for compiler definitions and libraries.
- Handle source file names with "-" in the filename. These are mapped to underscore.

## ME_COMPAT Definitions

To enable the compatibility mappings, configure with compat set to true.
<pre class="ui code segment">./configure --set compat=true</pre>

If you are building with Make, run make with ME_COMPAT=1.
<pre class="ui code segment">make ME_COMPAT=1</pre>

These are the mappings defined via ME_COMPAT:

<pre class="ui code segment">
#define conn stream
#define HttpConn HttpStream
#define httpCreateConn httpCreateStream
#define httpDestroyConn httpDestoryStream
#define httpDisconnectConn httpDisconnectStream
#define httpResetClientConn httpResetClientStream
#define httpPrepClientConn httpPrepClientStream

#define httpGetConnContext httpGetStreamContext
#define httpGetConnEventMask httpGetStreamEventMask
#define httpGetConnHost httpGetStreamHost
#define httpSetConnContext httpSetStreamContext
#define httpSetConnHost httpSetStreamHost
#define httpSetConnData httpSetStreamData
#define httpSetConnNotifier httpSetStreamNotifier
#define httpSetConnUser httpSetStreamUser

#define httpEnableConnEvents(stream) httpEnableNetEvents(stream->net)
#define httpClientConn(stream) httpClientStream(stream)
#define httpServerConn(stream) httpServerStream(stream)
#define httpDisconnect(stream) httpDisconnectStream(stream)
</pre>

You should use the ME_COMPAT mappings only as a transitional aid as they may be removed in a future release.
