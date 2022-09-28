/*
   Generated from dist/post/list.esp
 */
#include "esp.h"

static void view_5448a3d74a90c834195932b29f2d4153(HttpStream *stream) {
EdiGrid     *grid = readTable("post");
            EdiRec      *rec;
            EdiField    *fp;
          
  espRenderBlock(stream, "<!DOCTYPE html><html lang=\"en\"><title>Demo Application</title><meta charset=\"utf-8\"><meta name=\"description\" content=\"Demo Application\"><link rel=\"icon\" href=\"data:image/x-icon;base64,\" type=\"image/x-icon\"><link href=\"../css/all.css\" rel=\"stylesheet\" type=\"text/css\"><link rel=\"stylesheet\" href=\"../css/all.css\" type=\"text/css\"><div class=\"navbar\"><div class=\"navbar-header\"><a class=\"navbar-brand\" href=\"../\">", 410);
  espRenderSafeString(stream, getConfig("title"));
  espRenderBlock(stream, "</a></div></div><main class=\"container\"> ", 41);
  espRenderBlock(stream, " <h1>Post List</h1><table class='list'><thead><tr>", 50);
if (grid && grid->nrecords) {
                        rec = grid->records[0];
                        for (fp = 0; (fp = ediGetNextField(rec, fp, 1)) != 0; ) { 
                            render("<th>%s</th>\n", stitle(fp->name));
                        }
                    }   espRenderBlock(stream, " <tbody> ", 9);
for (rec = 0; (rec = ediGetNextRec(grid, rec)) != 0; ) {   espRenderBlock(stream, " <tr onclick='document.location=\"", 33);
  espRenderSafeString(stream, uri("%s", rec->id));
  espRenderBlock(stream, "\"'> ", 4);
for (fp = 0; (fp = ediGetNextField(rec, fp, 1)) != 0; ) {   espRenderBlock(stream, " <td>", 5);
  espRenderSafeString(stream, ediFormatField(0, fp));
  espRenderBlock(stream, "</td> ", 6);
}   espRenderBlock(stream, " </tr> ", 7);
}   espRenderBlock(stream, " </table><a href=\"init\"><button class='btn btn-primary'>New Post</button></a></main><div class=\"feedback\"> ", 107);
renderFeedback("*");   espRenderBlock(stream, " </div>\n\
", 8);
}

ESP_EXPORT int esp_view_5448a3d74a90c834195932b29f2d4153(HttpRoute *route) {
   espDefineView(route, "post/list.esp", view_5448a3d74a90c834195932b29f2d4153);
   return 0;
}
