/*
   Generated from dist/index.esp
 */
#include "esp.h"

static void view_6f066ed641998e6b0e8794e436f0a3e9(HttpStream *stream) {
  espRenderBlock(stream, "<!DOCTYPE html><html lang=\"en\"><title>Demo Application</title><meta charset=\"utf-8\"><meta name=\"description\" content=\"Demo Application\"><link rel=\"icon\" href=\"data:image/x-icon;base64,\" type=\"image/x-icon\"><link href=\"css/all.css\" rel=\"stylesheet\" type=\"text/css\"><link rel=\"stylesheet\" href=\"./css/all.css\" type=\"text/css\"><div class=\"navbar\"><div class=\"navbar-header\"><a class=\"navbar-brand\" href=\"./\">", 405);
  espRenderSafeString(stream, getConfig("title"));
  espRenderBlock(stream, "</a></div></div><main class=\"container\"><article><h1>Welcome to Embedded Server Pages</h1><section><p>ESP is a powerful \"C\"-based web framework for blazing fast dynamic web applications. It has an page templating engine, layout pages, a Model-View-Controller framework, embedded database, content caching and application generator.</section><section><h2>Quick Start</h2><ol><li><b>Create ESP Pages</b><p>Create web pages under the \"contents\" directory. Modify the layout in \"layouts/default.html.exp\" and customize the Less-based style sheet in the \"contents/css/app.less\" and \"contents/css/theme.less\".<li><b>Render</b><p>Install <a href=\"https://www.embedthis.com/expansive/\">Expansive</a> and run <b>\"expansive render\"</b> to render the site into the \"dist\" directory.<li><b>Serve</b><p>Run <b>\"esp -v\"</b> or <b>\"expansive\"</b> to serve pages.<li><b>Browse</b><p>Navigate to <b>http://localhost:4000/</b> in your browser.<li><b>Generate Controllers</b><p>Create controllers to manage your app. Run <a href=\"https://www.embedthis.com/esp/doc/users/esp.html\"><b>esp</b></a> with no options to see its usage.<pre>esp generate controller NAME [action, ...]</pre><li><b>Generate Scaffolds</b><p>Create entire scaffolds for large sections of your application.<pre>esp generate scaffold NAME [field:type, ...]</pre><li><b>Read the Documentation</b><p>Go to <a href=\"https://www.embedthis.com/esp/doc/\">https://www.embedthis.com/esp/doc/</a> for the latest ESP documentation. Here you can read quick tours, overviews and access all the ESP APIs.<li><b>Enjoy!</b></ol></section></article><aside><h2 class=\"section\">ESP Links</h2><ul><li><a href=\"https://www.embedthis.com\">Official Web Site</a><li><a href=\"https://www.embedthis.com/esp/doc/\">Documentation</a><li><a href=\"https://www.embedthis.com/esp/doc/users/\">ESP User's Guide</a><li><a href=\"https://github.com/embedthis/esp\">ESP Repository and Issue List</a><li><a href=\"https://www.embedthis.com/blog/\">Embedthis Blog</a></ul></aside></main><div class=\"feedback\"> ", 1985);
renderFeedback("*");   espRenderBlock(stream, " </div>\n\
", 8);
}

ESP_EXPORT int esp_view_6f066ed641998e6b0e8794e436f0a3e9(HttpRoute *route) {
   espDefineView(route, "index.esp", view_6f066ed641998e6b0e8794e436f0a3e9);
   return 0;
}
