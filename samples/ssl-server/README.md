SSL Server Sample
===

This sample shows how to configure Appweb to use SSL.

Requirements
---
* [Appweb](https://www.embedthis.com/appweb/download.html)

To build:
---
    make

To run:
---
    make run

The server listens on port 4443 for SSL requests. Browse to:

     https://localhost:4443/

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [appweb.conf](appweb.conf) - Appweb server configuration file
* [web](web) - Web content to serve

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Configuration Directives](https://www.embedthis.com/appweb/doc/users/configuration.html#directives)
* [Security Considerations](https://www.embedthis.com/appweb/doc/users/security.html)
* [SSL in Appweb](https://www.embedthis.com/appweb/doc/users/ssl.html)

See Also:
---
* [min-server - Minimal server configuration](../min-server/README.md)
* [secure-server - Secure server configuration](../secure-server/README.md)
* [simple-server - Simple one-line embedding API](../simple-server/README.md)
