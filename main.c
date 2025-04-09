#include "stdio.h"
#include "Lexer.h"

int main() {
	char* text = " \"flopper\" 45";
	Lexer lexer = lexer_init(text);

	while (!lexer_is_at_end(&lexer)) {
		Token token = lexer_next_token(&lexer);
		token_print(&token);
	}


	return 0;
}