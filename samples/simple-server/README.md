Simple Server Sample
===

This sample shows how to embed Appweb into a main program using a one-line embedding API.

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

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [server.c](server.c) - Main program
* [appweb.conf](appweb.conf) - Appweb server configuration file
* [index.html](index.html) - Web page to serve

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Configuration Directives](https://www.embedthis.com/appweb/doc/users/configuration.html#directives)
* [Sandbox Limits](https://www.embedthis.com/appweb/doc/users/dir/sandbox.html)

See Also:
---
* [typical-server - Fully featured server and embedding API](../typical-server/README.md)
