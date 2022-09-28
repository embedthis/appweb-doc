chroot-server Sample
===

This sample shows how to run Appweb in a Chroot-jail. This is where Appweb runs with enhanced
security by changing its root directory so that the rest of the operating system is inaccessible.

Requirements
---
* [Appweb](https://www.embedthis.com/appweb/download.html)

To build:
---
    make

To run:
---
    sudo make run

The server listens on port 8080. Browse to:

     http://localhost:8080/
     http://localhost:8080/test.esp

Alternatively, run appweb to use the appweb.conf file.

    sudo appweb

Notes:
---
So that the Compiler is not required inside the jail, the ESP pages are pre-compiled before running Appweb.
Appweb is configured to load modules before changing the root directory via the "Chroot" directive in appweb.conf.

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [server.c](server.c) - Main program
* [appweb.conf](appweb.conf) - Appweb server configuration file

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Chroot Directive](https://www.embedthis.com/appweb/doc/users/dir/server.html#chroot)
* [Security Considerations](https://www.embedthis.com/appweb/doc/users/security.html)

See Also:
---
* [typical-server - Fully featured server and embedding API](../typical-server/README.md)
