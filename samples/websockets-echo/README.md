ESP WebSockets Sample
===

This sample shows how to create a WebSockets echo server. This application
echos incoming packets back to the client. The application source is in echo.c.
This is a simple ESP controler with one action that is run in response
to the URI:

    /ws/test/echo

The HTML page web/index.html is served to the client browser to issue a web socket
request.

The server is configured to keep the web socket open for 1 minute of inactivity and
then close the connection.

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

     http://localhost:8080/

This opens a web socket and sends the message "Hello Server" to the server. The server responds
by echoing back the message. After one minute of inactivity, the server will close the connection.

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [cache](cache) - Directory for compiled ESP modules
* [appweb.conf](appweb.conf) - Appweb server configuration file
* [echo.c](echo.c) - WebSockets echo server code
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
