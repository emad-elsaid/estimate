#include "./string.h"
char *views_username_html() {
String *w = StringNew(NULL);
StringWrite(w, "<section class=\"section\">\n"
"  <div class=\"container\">\n"
"    <form action=\"/username\" method=\"POST\">\n"
"      <div class=\"field\">\n"
"        <label class=\"label\" for=\"username\">Your name</label>\n"
"        <div class=\"control is-fullwidth\">\n"
"          <input class=\"input\" type=\"text\" placeholder=\"Your name...\" name=\"username\" id=\"username\" autofocus>\n"
"        </div>\n"
"      </div>\n"
"\n"
"      <div class=\"field\">\n"
"        <div class=\"control\">\n"
"          <button class=\"button is-link\" type=\"submit\">Set name</button>\n"
"        </div>\n"
"      </div>\n"
"    </form>\n"
"  </div>\n"
"</section>\n"
"");
char *ret = w->value;
free(w);
return ret;
 }
char *views_header_html() {
String *w = StringNew(NULL);
StringWrite(w, "<!DOCTYPE html>\n"
"<html>\n"
"  <head>\n"
"    <meta charset=\"utf-8\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
"    <title>Estimate</title>\n"
"    <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bulma@0.9.2/css/bulma.min.css\">\n"
"  </head>\n"
"  <body>\n"
"");
char *ret = w->value;
free(w);
return ret;
 }
char *views_footer_html() {
String *w = StringNew(NULL);
StringWrite(w, "  </body>\n"
"</html>\n"
"");
char *ret = w->value;
free(w);
return ret;
 }
