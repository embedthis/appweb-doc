Tiny Server Sample
===

This sample shows how to run Appweb while limit resources to be as small as possible
using appweb.conf configuration directives.

To really shrink appweb, build Appweb with MakeMe. Configure appweb from source without
all features, and then re-add only those you need:

    ./configure --without all --with esp
    me

Requirements
---
* [Appweb](https://www.embedthis.com/appweb/download.html)
* [MakeMe Build Tool](https://www.embedthis.com/makeme/download.html)

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
* [min-server - Minimal server configuration](../min-server/README.md)
* [simple-server - Simple one-line embedding API](../simple-server/README.md)
* [typical-server - Typical server configuration](../typical-server/README.md)
