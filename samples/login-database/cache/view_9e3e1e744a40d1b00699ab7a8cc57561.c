/*
   Generated from documents/index.esp
 */
#include "esp.h"

static void view_9e3e1e744a40d1b00699ab7a8cc57561(HttpStream *stream) {
  espRenderBlock(stream, "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
    <link rel=\"shortcut icon\" href=\"data:image/x-icon;,\" type=\"image/x-icon\">  \n\
</head>\n\
<body>\n\
    <h1>Welcome</h1>\n\
\n\
    <p>You will remain logged in for one minute of inactivity. \n\
        Click <a href=\"/\">here</a> to restart the inactivity timer.</p>\n\
    <p>Click to <a href=\"/user/logout\">Logout</a> and you will be redirected back to the login form.</p>\n\
\n\
    <script>\n\
        setTimeout(function() { document.location = \"/user/logout\"; }, 60 * 1000);\n\
    </script>\n\
</body>\n\
</html>\n\
", 525);
}

ESP_EXPORT int esp_view_9e3e1e744a40d1b00699ab7a8cc57561(HttpRoute *route) {
   espDefineView(route, "index.esp", view_9e3e1e744a40d1b00699ab7a8cc57561);
   return 0;
}
