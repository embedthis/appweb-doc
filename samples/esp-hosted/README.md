esp-hosted Sample
===

This sample shows how to host an ESP application in Appweb.

This sample uses:

* esp-html-skeleton server-side MVC application
* Pak to install required extensions
* Expansive to render layouts+pages into the documents directory

Notes:
* This application was generated via:

    mkdir esp-hosted
    cd esp-hosted
    pak init demo
    pak install esp-html-skeleton
    esp generate scaffold post title:string body:text
    expansive render

Requirements
---
* [APPWEB](https://www.embedthis.com/appweb/download.html)

To build:
---
    make build

To run:
---
    make run

or
    appweb -v

The server listens on port 8080 for HTTP traffic.

Browse to this home page for a welcome page:

    http://localhost:8080/


Browse to this page for a simple blog post app

    http://localhost:8080/post


Code:
---
* [appweb.conf](appweb.conf) - Appweb configuration file
* [cache](cache) - Directory for cached ESP pages
* [client/index.esp](client/index.esp) - Home page
* [controllers/post.c](controllers/post.c) - Controller code
* [db](db) - Database and migrations
* [documents](documents) - Public client-side documents.
* [esp.json](esp.json) - ESP configuration file
* [expansive.json](eexpansive.json) - Expansive configuration file
* [layouts](layouts) - ESP layout pages
* [package.json](package.json) - Package configuration file
* [paks](paks) - Extension packages
* [source](source) - Input client-side documents source.

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [ESP Documentation](https://www.embedthis.com/esp/doc/index.html)
* [ESP Configuration in Appweb](https://www.embedthis.com/appweb/doc/users/dir/esp.html)
* [ESP Configuration](https://www.embedthis.com/esp/doc/users/config.html)
* [Sandbox Limits](https://www.embedthis.com/appweb/doc/users/dir/sandbox.html)
* [Security Considerations](https://www.embedthis.com/appweb/doc/users/security.html)
* [User Authentication](https://www.embedthis.com/appweb/doc/users/authentication.html)
