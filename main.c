#include "stdio.h"
#include "Lexer.h"
#include "Ast.h"
#include "my_String.h"

int main() {
	// char* text  = " 12.23 \"flopper\" 45 69 999";
	// Lexer lexer = lexer_init(text);

	// while (!lexer_is_at_end(&lexer)) {
	// 	Token token = lexer_next_token(&lexer);
	// 	token_print(&token);
	// }

	// =====================================================
			// == Grammar
			//          Current grammar
			// expression :: equality ;
			// equality   :: comparison ( ( "!=" | "==" ) comparison )* ;
			// comparison :: term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
			// term       :: factor ( ( "-" | "+" ) factor )* ;
			// factor     :: unary ( ( "/" | "*" ) unary )* ;
			// unary      :: ( "!" | "-" ) unary
			//                | primary ;
			// primary    :: INTEGER | STRING | "(" expression ")" ;



	// TODO: better debugging for expression
	// TODO: maybe use token types for Primary expression types

	// char* text  = " \"flopper\" ";  // primary
	 char* text  = "- (- 1 + 2)";   // unary
	 Lexer lexer = lexer_init(text);
	 Expr* expr  = expression(&lexer);

	 String expr_as_str = expr_to_string(expr);
	 string_print(&expr_as_str);
	 string_delete(&expr_as_str);


	


	return 0;
}
