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
	 char* text  = " 1 + 2 * 3 == 7";   // unary
	 Lexer lexer = lexer_init(text);
	 Expr* expr  = expression(&lexer);

	 String expr_as_str = expr_to_string(expr);
	 string_print(&expr_as_str);
	 string_delete(&expr_as_str);


	/*{	
		String str = string_init("flopper");
		string_print(&str);

		string_add_c_string(&str, " money");
		string_print(&str);

		String other = string_init(" peter peter peter peter peter");
		string_add_string(&str, &other);
		string_print(&str);
		string_delete(&other);

		string_print(&other);

		other = string_init("other");
		string_print(&other);
		string_delete(&other);

		string_delete(&str);

		string_print(&str);
		string_print(&other);
	}*/


	return 0;
}
