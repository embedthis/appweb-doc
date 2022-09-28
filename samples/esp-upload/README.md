Appweb ESP File Upload Sample
===

This sample shows how to configure Appweb+ESP for file upload.

The sample includes an upload web form: web/upload-form.html. This form will
post the uploaded file to the web/upload/upload.esp page.

Requirements
---
* [APPWEB](https://www.embedthis.com/appweb/download.html)

To run:
---
    appweb

The server listens on port 8080. Browse to: 

     http://localhost:8080/upload-form.html

Code:
---
* [upload-form.html](upload-form.html) - File upload form
* [upload.esp](upload.esp) - ESP page to receive the uploaded file
* [cache](cache) - Compiled ESP modules

Documentation:
---
* [Appweb Documentation](https://www.embedthis.com/appweb/doc/index.html)
* [ESP Documentation](https://www.embedthis.com/esp/doc/index.html)
* [ESP Configuration in Appweb](https://www.embedthis.com/appweb/doc/users/dir/esp.html)
* [ESP Configuration](https://www.embedthis.com/esp/doc/users/config.html)
* [File Upload)(https://www.embedthis.com/esp/doc/users/uploading.html)
* [ESP APIs](https://www.embedthis.com/esp/doc/ref/api/esp.html)
* [ESP Guide](https://www.embedthis.com/esp/doc/users/index.html)
* [ESP Overview](https://www.embedthis.com/esp/doc/users/using.html)

See Also:
---
* [html-mvc - ESP HTML MVC Application](../html-mvc/README.md)
* [controller - Creating ESP controllers](../controller/README.md)
* [page - Serving ESP pages](../page/README.md)
