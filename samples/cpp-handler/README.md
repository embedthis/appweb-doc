cpp-handler Sample
===

This sample shows how to create an Appweb handler module. Handlers receive to client requests and
generate responses.

Requirements
---
* [Appweb](https://www.embedthis.com/appweb/download.html)

To build:
---
    make

To run:
---
    make run

Appweb listens on port 8080. Browse to:

     http://localhost:8080/

To send data to the handler, use:
    http --form 'name=john' 8080

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [cppHandler.cpp](cppHandler.cpp) - Simple handler
* [appweb.conf](appweb.conf) - Appweb server configuration file

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Creating Handlers](https://www.embedthis.com/appweb/doc/developers/handlers.html)
* [Creating Modules](https://www.embedthis.com/appweb/doc/developers/modules.html)
* [API Library](https://www.embedthis.com/appweb/doc/ref/native.html)

See Also:
---
* [simple-handler - C-Language handler](../simple-handler/README.md)
