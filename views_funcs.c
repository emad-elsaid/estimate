#include "./string.h"
char *views_footer_html() {
String *w = StringNew(NULL);
StringWrite(w, "  </body>\n"
"</html>\n"
"");
char *ret = w->value;free(w);return ret; }
