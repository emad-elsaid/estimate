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
	 Board *board = input; 	StringWrite(w, ""
	"<section class=\"section\">"
	"  <div class=\"container\">"
	""
	"    ");
	 if( board->id ){ 	StringWrite(w, ""
	"    <form action=\"/boards/edit?board=");
	StringWrite(w,  board->id );
	StringWrite(w, "\" method=\"POST\">"
	"    ");
	 }else{ 	StringWrite(w, ""
	"    <form action=\"/boards\" method=\"POST\">"
	"    ");
	 } 	StringWrite(w, ""
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
char *views_vote_html(void *input) {
	String *w = StringNew(NULL);
	 Board *board = input; 	StringWrite(w, ""
	"<section class=\"section\">"
	"  <div class=\"container\">"
	"    <h1 class=\"title\">"
	"      Vote"
	"    </h1>"
	""
	"    <div class=\"columns is-multiline\">"
	""
	"      ");
	 for(Option *option=board->options; option != NULL; option = option->next){ 	StringWrite(w, ""
	"        <div class=\"column is-one-third\">"
	"          <div class=\"card\">"
	"            <div class=\"card-content has-text-centered\">"
	"              <form action=\"/boards/vote?board=");
	StringWrite(w,  board->id );
	StringWrite(w, "\" method=\"POST\">"
	"                <input name=\"vote\" type=\"hidden\" value=\"");
	StringWrite(w,  option->value );
	StringWrite(w, "\"/>"
	"                <button class=\"button is-ghost\" style=\"font-size: 5em;\" type=\"submit\">");
	StringWrite(w,  option->value );
	StringWrite(w, "</button>"
	"              </form>"
	"            </div>"
	"          </div>"
	"        </div>"
	"      ");
	 } 	StringWrite(w, ""
	""
	"    </div>"
	"  </div>"
	"</section>"
	"");
	char *ret = w->value;
	free(w);
	return ret;
}
char *views_board_html(void *input) {
	String *w = StringNew(NULL);
	
BoardPage page = *(BoardPage *)input;
Board *board = page.board;
UUID userid = page.user;
	StringWrite(w, ""
	"<div class=\"container\">"
	"  <div class=\"section\">"
	"    ");
	 if( strcmp(board->userid, userid)==0 ){ 	StringWrite(w, ""
	"    <a class=\"button\" href=\"/boards/edit?board=");
	StringWrite(w,  board->id );
	StringWrite(w, "\">✏️ Edit</a>"
	""
	"      ");
	 if( board->votes != NULL ){ 	StringWrite(w, ""
	"      <a class=\"button is-link is-light\" href=\"/boards/reset?board=");
	StringWrite(w,  board->id );
	StringWrite(w, "\" title=\"This will clear votes and ask users to vote again\">✋ Request New Votes</a>"
	"      ");
	 } 	StringWrite(w, ""
	""
	"      ");
	 if( board->hidden ){ 	StringWrite(w, ""
	"        <a class=\"button is-info is-light\" href=\"/boards/show?board=");
	StringWrite(w,  board->id );
	StringWrite(w, "\">👁️ Show Votes</a>"
	"      ");
	 }else{ 	StringWrite(w, ""
	"        <a class=\"button is-info is-light\" href=\"/boards/hide?board=");
	StringWrite(w,  board->id );
	StringWrite(w, "\">👁️ Hide Votes</a>"
	"      ");
	 } 	StringWrite(w, ""
	""
	"    ");
	 } 	StringWrite(w, ""
	"    <a class=\"button is-success is-light\" href=\"/boards/vote?board=");
	StringWrite(w,  board->id );
	StringWrite(w, "\">🗳️ Vote</a>"
	"  </div>"
	""
	"  <div class=\"section\">"
	""
	"    ");
	 if(board->votes == NULL){  	StringWrite(w, ""
	"      <div class=\"notification\">"
	"        No votes yet. Waiting for the team to vote..."
	"      </div>"
	"    ");
	 }else{ 	StringWrite(w, ""
	"      <h1 class=\"title\">"
	"        🗳️ ");
	StringWrite(w,  intToChar(board->votes_count) );
	StringWrite(w, " Votes"
	"        ");
	 if(! board->hidden ){ 	StringWrite(w, ""
	"          | ");
	 for(Hash *stat=board->votes_stats; stat != NULL; stat = stat->next){ 	StringWrite(w, ""
	"              ");
	StringWrite(w,  stat->key );
	StringWrite(w, " <sub>×");
	StringWrite(w,  intToChar(*(int*) stat->value) );
	StringWrite(w, "</sub>"
	"            ");
	 } 	StringWrite(w, ""
	"        ");
	 } 	StringWrite(w, ""
	"      </h1>"
	"    ");
	 } 	StringWrite(w, ""
	""
	"    <div class=\"columns is-multiline is-centered\">"
	"      ");
	 for(Vote *vote = board->votes; vote != NULL; vote = vote->next){ 	StringWrite(w, ""
	"        <div class=\"column is-one-quarter\">"
	"          <div class=\"card\">"
	"            <div class=\"card-content has-text-centered\">"
	"              <div class=\"title\" style=\"font-size: 9em;");
	StringWrite(w,  (board->hidden)? "filter: blur(1rem);" : "" );
	StringWrite(w, "\">"
	"                ");
	 if(board->hidden){ 	StringWrite(w, ""
	"                  X"
	"                ");
	 }else{ 	StringWrite(w, ""
	"                  ");
	StringWrite(w,  h(vote->vote) );
	StringWrite(w, ""
	"                ");
	 } 	StringWrite(w, ""
	"              </div>"
	"              <div class=\"subtitle\"> ");
	StringWrite(w,  h(userid) );
	StringWrite(w, " </div>"
	"            </div>"
	"          </div>"
	"        </div>"
	"        ");
	 } 	StringWrite(w, ""
	"    </div>"
	""
	"  </div>"
	"</div>"
	""
	"<iframe frameborder=\"0\" src=\"/boards/check?board=");
	StringWrite(w,  board->id );
	StringWrite(w, "&time=");
	StringWrite(w,  timeToChar(board->updated_at) );
	StringWrite(w, "\" style=\"width:0;height:0;display:absolute;\"></iframe>"
	"");
	char *ret = w->value;
	free(w);
	return ret;
}
