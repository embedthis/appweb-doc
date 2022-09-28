/*
   Generated from dist/post/edit.esp
 */
#include "esp.h"

static void view_5cbd50b6ecf18a0e9b04544ad14d72c8(HttpStream *stream) {
EdiRec      *rec = getRec();
            EdiField    *fp;
          
  espRenderBlock(stream, "<!DOCTYPE html><html lang=\"en\"><title>Demo Application</title><meta charset=\"utf-8\"><meta name=\"description\" content=\"Demo Application\"><link rel=\"icon\" href=\"data:image/x-icon;base64,\" type=\"image/x-icon\"><link href=\"../css/all.css\" rel=\"stylesheet\" type=\"text/css\"><link rel=\"stylesheet\" href=\"../css/all.css\" type=\"text/css\"><div class=\"navbar\"><div class=\"navbar-header\"><a class=\"navbar-brand\" href=\"../\">", 410);
  espRenderSafeString(stream, getConfig("title"));
  espRenderBlock(stream, "</a></div></div><main class=\"container\"> ", 41);
  espRenderBlock(stream, " <h1>", 5);
  espRenderSafeString(stream, param("id") ? "Edit" : "Create");
  espRenderBlock(stream, " Post</h1><form name='PostForm' class='form-horizontal' action='../post/", 72);
  espRenderVar(stream, "id");
  espRenderBlock(stream, "' method=\"POST\"> ", 17);
for (fp = 0; (fp = ediGetNextField(rec, fp, 1)) != 0; ) {   espRenderBlock(stream, " <div class='form-group'><label class='control-label col-md-3'>", 63);
  espRenderSafeString(stream, stitle(fp->name));
  espRenderBlock(stream, "</label><div class='input-group col-md-8 ", 41);
  espRenderSafeString(stream, getFieldError(fp->name) ? "has-error" : "");
  espRenderBlock(stream, "'> ", 3);
input(fp->name, "{class: 'form-control'}");   espRenderBlock(stream, " </div></div> ", 14);
}   espRenderBlock(stream, " <div class='form-group'><div class='col-md-offset-2 col-md-10'><input type='submit' class='btn btn-primary' name=\"submit\" value=\"Save\"> <a href='list'><button class='btn' type=\"button\">Cancel</button></a> ", 206);
if (hasRec()) {   espRenderBlock(stream, " <input type='submit' class='btn' name=\"submit\" value=\"Delete\"> ", 64);
}   espRenderBlock(stream, " ", 1);
inputSecurityToken();   espRenderBlock(stream, " </div></div></form></main><div class=\"feedback\"> ", 50);
renderFeedback("*");   espRenderBlock(stream, " </div>\n\
", 8);
}

ESP_EXPORT int esp_view_5cbd50b6ecf18a0e9b04544ad14d72c8(HttpRoute *route) {
   espDefineView(route, "post/edit.esp", view_5cbd50b6ecf18a0e9b04544ad14d72c8);
   return 0;
}
