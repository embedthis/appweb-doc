login-basic Sample
===

This sample shows how to configure a simple basic (or digest) browser-based login. This sample uses the
internal browser dialog for entering username and password credentials.

WARNING: You should use Basic and Digest authentication only as a last resort. Basic and Digest authentication
standards employ weak ciphers, repeatedly send credentials over the wire and and are not sufficiently secure.
Basic authentication transmits passwords as clear-text in each and every request. Digest authentication uses the weak
MD5 cipher, and both require use of SSL on all requests to be minimally secure. Further, both Basic and Digest
do not provide a reliable log out mechanism. Logout works on some browsers but not on other browsers or even
different versions of the same browser. We therefore strongly recommend using "form" authentication, with
"blowfish" encryption for a more usable, secure and reliable solution. See the login-form example for the
recommended approach.

This sample uses:

* SSL for encryption of traffic
* Automatic redirection of HTTP traffic to SSL
* Self-signed certificate. You should obtain a real certificate.
* Login username and password entry via browser dialog
* Username / password validation using the "config" file-based authentication store.
* Blowfish or MD5 encryption for secure password hashing

Notes:
* This sample keeps the passwords in the auth.conf. So the sample works with Digest authentication which requires
  MD5 encryption, the test password was created via:

    authpass --cipher md5 --password pass1 --file auth.conf example.com joshua user

  However, if using basic authentication, you should use the more secure blowfish encryption via:

    authpass --cipher blowfish --password pass1 --file auth.conf example.com joshua user

* The sample is setup to use the "config" auth store which keeps the passwords in the auth.conf file.
    Set this to "system" if you wish to use passwords in the system password database (linux or macosx only).

* The sample uses the "basic" auth type by default.
    It can be configured to use the "digest" authentication protocol by setting the AuthType to "digest".

* The entire site is secured including all URLs. There is no portion of the site that is "public".

* Logout is problematic with basic/digest schemes. The standard does not define a mechanism for logout.
    It cannot be reliably implemented on all browsers over all versions.

Requirements
---
* [Download Appweb](https://www.embedthis.com/appweb/download.html)

To run:
---
    appweb -v

The server listens on port 8080 for HTTP traffic and 4443 for SSL. Browse to:

     http://localhost:8080/

This will redirect to SSL (you will get a warning due to the self-signed certificate).
Continue and you will be prompted to login. The test username/password is:

    joshua/pass1

Code:
---
* [index.html](index.html) - Home page
* [appweb.conf](appweb.conf) - Appweb configuration
* [auth.conf](auth.conf) - Password definitions

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [Appweb Configuration](https://www.embedthis.com/appweb/doc/users/configuration.html)
* [Appweb User Authentication](https://www.embedthis.com/appweb/doc/users/authentication.html)
