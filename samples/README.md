Embedthis Appweb Samples
===

These samples are configured to use a locally built Appweb or Appweb installed to the default location
(usually /usr/local/lib/apppweb). The Makefiles assume GCC on Linux or Mac. You will need to adjust to
build on Windows.

* Appweb - [https://www.embedthis.com/appweb/download.html](https://www.embedthis.com/appweb/download.html)

The following samples are available:

* [chroot-server](chroot-server/README.md)                  Configuring a secure chroot jail for the server.
* [cpp-handler](cpp-handler/README.md)                      C++ Handler
* [cpp-module](cpp-module/README.md)                        C++ Module
* [deploy-server](deploy-server/README.md)                  Deploy Appweb files for copying to a target.
* [esp-hosted](esp-hosted/README.md)                        Host an ESP application.
* [esp-upload](esp-upload/README.md)                        File upload with an ESP application.
* [login-basic](login-basic/README.md)                      Login using Basic or Digest authentication (not recommended).
* [login-database](login-database/README.md)                Login and verify user/password using a custom App auth store.
* [login-form](login-form/README.md)                        Login using Web Forms (recommended).
* [max-server](max-server/README.md)                        Maximum configuration in appweb.conf.
* [min-server](min-server/README.md)                        Minimum configuration in appweb.conf.
* [non-blocking-client](non-blocking-client/README.md)      Using the client HTTP API to retrieve a document.
* [secure-server](secure-server/README.md)                  Secure server using SSL, secure login, chroot and sandbox limits.
* [simple-action](simple-action/README.md)                  Action callback. Binding C function to URI.
* [simple-handler](simple-handler/README.md)                Simple Appweb URL handler.
* [simple-module](simple-module/README.md)                  Simple Appweb loadable module.
* [simple-server](simple-server/README.md)                  Simple Http server.
* [spy-fliter](spy-filter/README.md)                        Simple HTTP pipeline filter.
* [ssl-server](ssl-server/README.md)                        SSL server.
* [thread-comms](thread-comms/README.md)                    Inter-thread communications and thread-safe APIs.
* [threaded-client](threaded-client/README.md)              Using the client HTTP API with threads.
* [tiny-server](tiny-server/README.md)                      Configure Appweb to be tiny.
* [typical-client](typical-client/README.md)                Using the client HTTP API to retrieve a document.
* [typical-server](typical-server/README.md)                A more fully featured server main program.
* [websockets-chat](websockets-chat/README.md)              WebSockets chat server using an ESP controller.
* [websockets-echo](websockets-echo/README.md)              WebSockets echo server using an ESP controller.
* [websockets-output](websockets-output/README.md)          Using WebSockets to send a large file.

### SSL Certificates

Some samples require SSL certificates and keys.

### Building

To build the samples, see the per-sample README instructions. Many can run without extra build steps.

To build all, use:

    make build

### Documentation

The full product documentation is supplied in HTML format under the doc directory. This is also available online at:

* https://www.embedthis.com/appweb/doc/index.html

Licensing
---

Please see:

* https://www.embedthis.com/licensing/index.html


Support
---
Embedthis provides support for Appweb for commercial customers. If you are interested in commercial support,
please contact Embedthis at:

* sales@embedthis.com


Copyright
---

Copyright (c) Embedthis Software. All Rights Reserved. Embedthis and Appweb are trademarks of
Embedthis Software, LLC. Other brands and their products are trademarks of their respective holders.
