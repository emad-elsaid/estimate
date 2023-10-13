#include "views_includes.h"
#include "./string.h"
char *views_username_html(void *input) {
String *w = StringNew(NULL);
StringWrite(w, "<section class=\"section\">"
"  <div class=\"container\">"
"    <form action=\"/username\" method=\"POST\">"
"      <div class=\"field\">"
"        <label class=\"label\" for=\"username\">Your name</label>"
"        <div class=\"control is-fullwidth\">"
"          <input class=\"input\" type=\"text\" placeholder=\"Your name...\" name=\"username\" id=\"username\" autofocus>"
"        </div>"
"      </div>"
""
"      <div class=\"field\">"
"        <div class=\"control\">"
"          <button class=\"button is-link\" type=\"submit\">Set name</button>"
"        </div>"
"      </div>"
"    </form>"
"  </div>"
"</section>"
"");
char *ret = w->value;
free(w);
return ret;
 }
char *views_header_html(void *input) {
String *w = StringNew(NULL);
StringWrite(w, "<!DOCTYPE html>"
"<html>"
"  <head>"
"    <meta charset=\"utf-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"    <title>Estimate</title>"
"    <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bulma@0.9.2/css/bulma.min.css\">"
"  </head>"
"  <body>"
"");
char *ret = w->value;
free(w);
return ret;
 }
char *views_index_html(void *input) {
String *w = StringNew(NULL);
StringWrite(w, "");
 Board *board = input; StringWrite(w, ""
"<section class=\"section\">"
"  <div class=\"container\">"
""
"    ");
 if( board->id ){ StringWrite(w, ""
"    <form action=\"/boards/");
StringWrite(w,  board->id );
StringWrite(w, "\" method=\"POST\">"
"    ");
 }else{ StringWrite(w, ""
"    <form action=\"/boards\" method=\"POST\">"
"    ");
 } StringWrite(w, ""
""
"      <div class=\"field\">"
"        <label class=\"label\" for=\"columns\">Options</label>"
"        <div class=\"control\">"
"          <textarea class=\"textarea\" placeholder=\"Each line an option...\" name=\"options\" id=\"columns\" autofocus>");
StringWrite(w,  board->options_str );
StringWrite(w, "</textarea>"
"        </div>"
"      </div>"
""
"      <div class=\"field\">"
"        <div class=\"control\">"
"          <button class=\"button is-link\" type=\"submit\">Create</button>"
"        </div>"
"      </div>"
""
"    </form>"
""
"  </div>"
"</section>"
"");
char *ret = w->value;
free(w);
return ret;
 }
char *views_footer_html(void *input) {
String *w = StringNew(NULL);
StringWrite(w, "  </body>"
"</html>"
"");
char *ret = w->value;
free(w);
return ret;
 }
char *views_board_html(void *input) {
String *w = StringNew(NULL);
StringWrite(w, "");
 Board *board = input; StringWrite(w, ""
"");
 UUID userid = ""; StringWrite(w, ""
"<div class=\"container\">"
"  <div class=\"section\">"
"    ");
 if( strcmp(board->userid, userid)==0 ){ StringWrite(w, ""
"      <a class=\"button\" href=\"/boards?board=");
StringWrite(w,  board->id );
StringWrite(w, "/edit\">✏️ Edit</a>"
""
"      ");
 if( board->votes != NULL ){ StringWrite(w, ""
"      <a class=\"button is-link is-light\" href=\"/boards/reset?board=");
StringWrite(w,  board->id );
StringWrite(w, "\" title=\"This will clear votes and ask users to vote again\">✋ Request New Votes</a>"
"      ");
 } StringWrite(w, ""
""
"      ");
 if( board->hidden ){ StringWrite(w, ""
"        <a class=\"button is-info is-light\" href=\"/boards/show?board=");
StringWrite(w,  board->id );
StringWrite(w, "\">👁️ Show Votes</a>"
"      ");
 }else{ StringWrite(w, ""
"        <a class=\"button is-info is-light\" href=\"/boards/hide?board=");
StringWrite(w,  board->id );
StringWrite(w, "\">👁️ Hide Votes</a>"
"      ");
 } StringWrite(w, ""
""
"    ");
 } StringWrite(w, ""
"    <a class=\"button is-success is-light\" href=\"/boards/vote?board=");
StringWrite(w,  board->id );
StringWrite(w, "\">🗳️ Vote</a>"
"  </div>"
""
"  <div class=\"section\">"
""
"    ");
 if(board->votes == NULL){  StringWrite(w, ""
"      <div class=\"notification\">"
"        No votes yet. Waiting for the team to vote..."
"      </div>"
"    ");
 }else{ StringWrite(w, ""
"      <h1 class=\"title\">"
"        🗳️ ");
StringWrite(w,  board->votes_count );
StringWrite(w, " Votes"
"        ");
 if(! board->hidden ){ StringWrite(w, ""
"          | ");
 board[:votes].values.group_by(&:itself).each { |k,v| StringWrite(w, " ");
StringWrite(w,  k );
StringWrite(w, " <sub>×");
StringWrite(w,  v.count );
StringWrite(w, "</sub> ");
 } StringWrite(w, ""
"        ");
 } StringWrite(w, ""
"      </h1>"
"    ");
 } StringWrite(w, ""
""
"    <div class=\"columns is-multiline is-centered\">"
"      ");
 for(Vote *vote = board->votes; vote != NULL; vote = vote->next){ StringWrite(w, ""
"        <div class=\"column is-one-quarter\">"
"          <div class=\"card\">"
"            <div class=\"card-content has-text-centered\">"
"              <div class=\"title\" style=\"font-size: 9em;");
StringWrite(w,  'filter: blur(1rem);' if board[:hidden] );
StringWrite(w, "\">"
"                ");
 if(board->hidden){ StringWrite(w, ""
"                  X"
"                ");
 }else{ StringWrite(w, ""
"                  ");
StringWrite(w, h(vote) );
StringWrite(w, ""
"                ");
 } StringWrite(w, ""
"              </div>"
"              <div class=\"subtitle\">"
"                ");
StringWrite(w,  h(userid) );
StringWrite(w, ""
"              </div>"
"            </div>"
"          </div>"
"        </div>"
"      ");
 } StringWrite(w, ""
"    </div>"
""
"  </div>"
"</div>"
""
"<iframe frameborder=\"0\" src=\"/boards/");
StringWrite(w,  board->id );
StringWrite(w, "/check/");
StringWrite(w,  board->updated_at );
StringWrite(w, "\" style=\"width:0;height:0;display:absolute;\"></iframe>"
"");
char *ret = w->value;
free(w);
return ret;
 }
