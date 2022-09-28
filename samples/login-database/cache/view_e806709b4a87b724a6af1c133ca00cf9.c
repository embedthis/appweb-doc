/*
   Generated from documents/public/login.esp
 */
#include "esp.h"

static void view_e806709b4a87b724a6af1c133ca00cf9(HttpStream *stream) {
  espRenderBlock(stream, "<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
    <link rel=\"shortcut icon\" href=\"data:image/x-icon;,\" type=\"image/x-icon\">\n\
    <link href=\"login.css\" rel=\"stylesheet\" type=\"text/css\" />\n\
    <title>login.esp</title>\n\
</head>\n\
<body>\n\
    <h1>Please log in</h1>\n\
    <form name=\"details\" method=\"post\" action=\"/user/login\">\n\
        <table border=\"0\">\n\
            <tr><td>Username</td><td><input type=\"text\" name=\"username\" value='", 419);
  espRenderSafeString(stream, param("username"));
  espRenderBlock(stream, "'></td></tr>\n\
            <tr><td>Password</td><td><input type=\"password\" name=\"password\" value='", 96);
  espRenderSafeString(stream, param("password"));
  espRenderBlock(stream, "'></td></tr>\n\
        </table>\n\
        <input type=\"submit\" name=\"submit\" value=\"OK\">\n\
        ", 93);
inputSecurityToken();   espRenderBlock(stream, "\n\
    </form>\n\
</body>\n\
</html>\n\
", 29);
}

ESP_EXPORT int esp_view_e806709b4a87b724a6af1c133ca00cf9(HttpRoute *route) {
   espDefineView(route, "public/login.esp", view_e806709b4a87b724a6af1c133ca00cf9);
   return 0;
}
