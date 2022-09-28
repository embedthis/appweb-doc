Maximum Config Server Sample
===

This sample shows many of the possible Appweb directives in the appweb.conf configuration file.
You are encouraged to see the other typical appweb.conf samples:

* [min-server - Minimal configuration server](../min-server/README.md)
* [simple-server - Simple one-line embedding API](../simple-server/README.md)
* [typical-server - Typical server](../typical-server/README.md)

Requirements
---
* [Appweb](https://www.embedthis.com/appweb/download.html)

To run:
---
    make run

The server listens on port 8080. Browse to:

     http://localhost:8080/

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [appweb.conf](appweb.conf) - Appweb server configuration file
* [auth.conf](auth.conf) - User/Password/Role authorization file
* [index.html](index.html) - web page to serve
* [web](web) - Web content to serve

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Configuration Directives](https://www.embedthis.com/appweb/doc/users/configuration.html#directives)
* [Sandbox Limits](https://www.embedthis.com/appweb/doc/users/dir/sandbox.html)

See Also:
---
* [min-server - Minimal configuration server](../min-server/README.md)
* [simple-server - Simple one-line embedding API](../simple-server/README.md)
* [typical-server - Typical server](../typical-server/README.md)
