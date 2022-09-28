Tyical Server Sample
===

This sample shows how to embed Appweb using the full embedding API.

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
* [web](web) - Web content to serve

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Configuration Directives](https://www.embedthis.com/appweb/doc/users/configuration.html#directives)
* [Sandbox Limits](https://www.embedthis.com/appweb/doc/users/dir/sandbox.html)
* [Security Considerations](https://www.embedthis.com/appweb/doc/users/security.html)
* [SSL in Appweb](https://www.embedthis.com/appweb/doc/users/ssl.html)
* [User Authentication](https://www.embedthis.com/appweb/doc/users/authentication.html)

See Also:
---
* [min-server - Minimal server configuration](../min-server/README.md)
* [simple-server - Simple one-line embedding API](../simple-server/README.md)
