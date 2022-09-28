ESP WebSockets Sample
===

This sample demonstrates writing large, streaming response without blocking,
buffering or consuming excessive memory. This sends a large file as a single
web socket message using multiple frames.

The sample is implemented as an ESP controler with one action. A test web
page initiates the client WebSocket request to retrieve the file. To run,
browse to:

    http://localhost:8080/index.html

Requirements
---

* [Appweb](https://www.embedthis.com/appweb/download.html)

To build:
---
    make

To run:
---
    make run

The server listens on port 8080. Browse to:

     http://localhost:8080/index.html

This opens a web socket and and listens for WebSocket data sent by the server.
It will display the received data in the browser window.

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [cache](cache) - Directory for compiled ESP modules
* [appweb.conf](appweb.conf) - Appweb server configuration file
* [output.c](output.c) - WebSockets echo server code
* [web](web) - Directory containing the index.html web page

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [ESP Directives](https://www.embedthis.com/appweb/doc/users/dir/esp.html)
* [ESP Tour](https://www.embedthis.com/esp/doc/start/tour.html)
* [ESP Controllers](https://www.embedthis.com/esp/doc/users/controllers.html)
* [ESP APIs](https://www.embedthis.com/esp/doc/ref/native.html)
* [ESP Guide](https://www.embedthis.com/esp/doc/users/index.html)
* [ESP Overview](https://www.embedthis.com/esp/doc/index.html)

See Also:
---
* [esp-angular-mvc - ESP Angular MVC Application](../esp-angular-mvc/README.md)
* [esp-controller - Serving ESP controllers](../esp-controller/README.md)
* [esp-html-mvc - ESP MVC Application](../esp-html-mvc/README.md)
* [esp-page - Serving ESP pages](../esp-page/README.md)
* [secure-server - Secure server](../secure-server/README.md)
* [simple-server - Simple server and embedding API](../simple-server/README.md)
* [typical-server - Fully featured server and embedding API](../typical-server/README.md)
