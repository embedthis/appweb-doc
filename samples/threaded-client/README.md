Typical Client Sample
===

This sample shows how to efficiently use the Http library to issue Http client requests.
This is a fuller sample suitable for applications that need to issue multiple HTTP requests.
If you only need to issue one HTTP request, consult the simple-client sample.

Requirements
---
* [Appweb](https://www.embedthis.com/appweb/download.html)

To build:
---
    make

To run:
---
    make run

The client retrieves:

     https://www.example.com/index.html

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [client.c](client.c) - Main program

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Configuration Directives](https://www.embedthis.com/appweb/doc/users/configuration.html#directives)
* [Http Client](https://www.embedthis.com/appweb/doc/users/client.html)
* [Http API](https://www.embedthis.com/appweb/doc/api/http.html)
* [API Library](https://www.embedthis.com/appweb/doc/ref/native.html)

See Also:
---
* [simple-client - Simple client and embedding API](../simple-client/README.md)
