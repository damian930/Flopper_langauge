#include "stdio.h"
#include "Lexer.h"
#include "Ast.h"
#include "my_String.h"

int main() {
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

	char* text    = " !false ";
	// Lexer lexer   = lexer_init(text);
	// Token token   = lexer_next_token(&lexer);
	// token_print(&token);
	
	Parser parser = parser_init(text);
	parser_parse(&parser);

	String expr_as_str = expr_to_string(parser.ast);
	string_print(&expr_as_str);

	Evaluation eval = evalueate_expression(parser.ast);
	printf("Eval.type: %d \n", eval.type);
	printf("Eval.int : %d \n", eval.union_.integer);


	// epxr_delete(parser.ast);
	// parser.ast = NULL;
	// string_print(&expr_as_str);


	return 0;
}
