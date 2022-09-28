Secure Server Sample
===

This sample shows how to configure a secure Appweb server. This configuration uses:

* SSL for encryption of traffic
* Redirection of all traffic over SSL
* Chroot
* Login authentication
* Sandbox resource limits
* Blowfish encryption for the password

This sample uses a self-signed certificate. In your application, you will need a real certificate.

Notes:
The password database is kept in a flat file called auth.conf. The password was created via:

    authpass --cipher blowfish --password pass5 --file auth.conf example.com ralph

Requirements
---
* [Appweb](https://www.embedthis.com/appweb/download.html)

To build:
---

Run:

    make

This will precompile the ESP pages and a create device node inside the chroot jail for /dev/urandom.

To run:
---
    make run

The server listens on port 8080 for HTTP traffice and 4443 for SSL. Browse to:

     http://localhost:8080/

This will redirect to SSL (you will get a warning due to the self-signed certificate).
Continue and you will be prompted to login. The test username/password is ralph/pass5.

Code:
---
* [Makefile](Makefile) - Makefile build instructions
* [server.c](server.c) - Main program
* [appweb.conf](appweb.conf) - Appweb server configuration file
* [web](web) - Web content to serve
* [cache](cache) - Directory for cached ESP pages

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Chroot Directive](https://www.embedthis.com/appweb/doc/users/dir/server.html#chroot)
* [Configuration Directives](https://www.embedthis.com/appweb/doc/users/configuration.html#directives)
* [Sandbox Limits](https://www.embedthis.com/appweb/doc/users/dir/sandbox.html)
* [Security Considerations](https://www.embedthis.com/appweb/doc/users/security.html)
* [SSL in Appweb](https://www.embedthis.com/appweb/doc/users/ssl.html)
* [User Authentication](https://www.embedthis.com/appweb/doc/users/authentication.html)

See Also:
---
* [esp-login - ESP form-login](../esp-login/README.md)
* [min-server - Minimal server configuration](../min-server/README.md)
* [secure-server - Secure server configuration](../secure-server/README.md)
* [simple-server - Simple one-line embedding API](../simple-server/README.md)
* [ssl-server - SSL server](../ssl-server/README.md)
* [typical-server - Typical server configuration](../typical-server/README.md)
