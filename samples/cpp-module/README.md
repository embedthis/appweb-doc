cpp-module Sample
===

This sample shows how to create an Appweb loadable module in C++. A module may provide an Appweb handler,
filter, custom configuration directives or any functionality you wish to integrate into Appweb. Appweb modules
are compiled into shared libraries and are dynamically loaded in response to appweb.conf LoadModule directives.
If your main program is statically linked, the same module, without change may be included in the main program
executable link, provided the module entry point is manually invoked from the main program.

Requirements
---
* [Appweb](https://www.embedthis.com/appweb/download.html)

To build:
---
    make

To run:
---
    make run

You will see trace in the console for the custom directive:

    Key CustomConfig = red

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [cppModule.cpp](cppModule.cpp) - Simple module
* [appweb.conf](appweb.conf) - Appweb server configuration file

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Creating Handlers](https://www.embedthis.com/appweb/doc/developers/handlers.html)
* [Creating Modules](https://www.embedthis.com/appweb/doc/developers/modules.html)
* [API Library](https://www.embedthis.com/appweb/doc/ref/native.html)

See Also:
---
* [simple-module - Simple one-line embedding C API](../simple-module/README.md)
