#include <stdlib.h>
#include "Ast.h"
#include "Lexer.h"
#include "my_String.h"

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

Expr* primary(Lexer* lexer) {
    Token token = lexer_next_token(lexer);
    
    if (token.type == Token_Type_Integer) {
        int int_value = atoi(token.lexeme);
        Primary primary = {
            .type = Token_Type_Integer,
            .union_.integer = int_value,
        };

        Expr* expr = malloc(sizeof(Expr));
        if (expr == NULL) {
            printf("Was not able to initialise expr, ran out of memory. \n");
            exit(1);
        }

        *expr = (Expr) {
            .type = Expr_type_primary,
            .union_.primary = primary,
        };
        return expr;
    }

    if (token.type == Token_Type_String) {
        Primary primary = {
            .type = Token_Type_String,
            .union_.string = token.lexeme,
 
        };

        // TODO: test for no memory
        Expr* expr = malloc(sizeof(Expr));
        if (expr == NULL) {
            printf("Was not able to allocate memory for an Expession. \n");
            exit(1);
        }
        *expr = (Expr) {
            .type = Expr_type_primary,
            .union_.primary = primary,
        };
        return expr;
    }

    printf("Was not able to parse the primary expr. Not yet supported. \n");
    exit(1);


    // error here

    // grouping here
}

Expr* unary(Lexer* lexer) {
    if (lexer_peek_next_token(lexer).type == (int) '-') {
        Token operator = lexer_next_token(lexer);
        Expr* right    = unary(lexer);
        Unary unary    = {operator, right};

        Expr* expr = malloc(sizeof(Expr));
        if (expr == NULL) {
            printf("Was not able to initialise expr, ran out of memory. \n");
            exit(1);
        }

        *expr = (Expr) {
            .type         = Expr_type_unary, 
            .union_.unary = unary,
        };
        return expr;
    }

    return primary(lexer);
}

Expr* factor(Lexer* lexer) {
    Expr* left = unary(lexer);
    Expr* expr = NULL;

    while (true) {
        Token next_token = lexer_peek_next_token(lexer);
        if (next_token.type == (int) '*' || next_token.type == (int) '/') {
            Token operator = lexer_next_token(lexer);
            Expr* right    = unary(lexer);

            Expr* new_expr = malloc(sizeof(Expr));
            if (new_expr == NULL) {
                printf("Was not able to allocated expr on heap. \n");
                exit(1);
            }

            *new_expr = (Expr) {
                .type = Expr_type_binary,
                .union_.binary = {
                    .operator = operator,
                    .left     = left,
                    .right    = right,
                },
            };
            expr = new_expr;
        }
        else {
            break;
        }

    }   

    return (expr == NULL ? left : expr);
}

Expr* term(Lexer* lexer) {
    Expr* left = factor(lexer);
    Expr* expr = NULL;

    while (true) {
        Token next_token = lexer_peek_next_token(lexer);
        if (next_token.type == (int) '+' || next_token.type == (int) '-') {
            Token operator = lexer_next_token(lexer);
            Expr* right    = factor(lexer);

            Expr* new_expr = malloc(sizeof(Expr));
            if (new_expr == NULL) {
                printf("Was not able to allocated expr on heap. \n");
                exit(1);
            }

            *new_expr = (Expr) {
                .type = Expr_type_binary,
                .union_.binary = {
                    .operator = operator,
                    .left     = left,
                    .right    = right,
                },
            };
            expr = new_expr;
        }
        else {
            break;
        }
    }

    return (expr == NULL ? left : expr);
}

Expr* comparison(Lexer* lexer) {
    Expr* left = term(lexer);
    Expr* expr = NULL;

    while (true) {
        Token next_token = lexer_peek_next_token(lexer);
        if (   next_token.type == (int) '<' 
            || next_token.type == (int) '>'
            || next_token.type == Token_Type_Greater_Or_Equals 
            || next_token.type == Token_Type_Less_Or_Equals
        ) {
            Token operator = lexer_next_token(lexer);
            Expr* right    = term(lexer);

            Expr* new_expr = malloc(sizeof(Expr));
            if (new_expr == NULL) {
                printf("Was not able to allocated expr on heap. \n");
                exit(1);
            }

            *new_expr = (Expr) {
                .type = Expr_type_binary,
                .union_.binary = {
                    .operator = operator,
                    .left     = left,
                    .right    = right,
                },
            };
            expr = new_expr;
        }
        else {
            break;
        }
    }

    return (expr == NULL ? left : expr);
}

Expr* equality(Lexer* lexer) {
    Expr* left = comparison(lexer);
    Expr* expr = NULL;

    while (true) {
        Token next_token = lexer_peek_next_token(lexer);
        if (   next_token.type == Token_Type_Equals_Equals 
            || next_token.type == Token_Type_Not_Equals 
        ) {
            Token operator = lexer_next_token(lexer);
            Expr* right    = comparison(lexer);

            Expr* new_expr = malloc(sizeof(Expr));
            if (new_expr == NULL) {
                printf("Was not able to allocated expr on heap. \n");
                exit(1);
            }
            
            *new_expr = (Expr) {
                .type = Expr_type_binary,
                .union_.binary = {
                    .operator = operator,
                    .left     = left,
                    .right    = right,
                },
            };
            expr = new_expr;
        }
        else {
            break;
        }
    }

    return (expr == NULL ? left : expr);
}

Expr* expression(Lexer* lexer) {
    return equality(lexer);
}


String expr_to_string(Expr* expr) {
    String str = string_init("");

    switch (expr->type) {

        case Expr_type_primary: {
            switch (expr->union_.primary.type) {
                case Token_Type_Integer: {
                    char int_as_str[30];
                    _itoa_s(expr->union_.primary.union_.integer, int_as_str, 30, 10);
                    string_add_c_string(&str, "(integer -> ");
                    string_add_c_string(&str, int_as_str);
                    string_add_c_string(&str, ")");
                    
                    break;
                }

                case Token_Type_String: {
                    string_add_c_string(&str, "(string -> ");
                    string_add_c_string(&str, expr->union_.primary.union_.string);
                    string_add_c_string(&str, ")");

                    break;
                }

                default: {
                    printf("Was not able to create a string for a primary expr,");
                    printf("cause the primary expr type is not supported. \n");
                    exit(1);

                    break;
                }
            }

            break;
        }

        case Expr_type_unary: {
            switch (expr->union_.unary.operator.type) {
                case (int) '-': {
                    Expr* child_expr = expr->union_.unary.right;
                    String child_expr_str = expr_to_string(child_expr);
                    
                    string_add_c_string(&str, "(- ");
                    string_add_string(&str, &child_expr);
                    string_add_c_string(&str, ")");
                
                    string_delete(&child_expr_str);
                    break;
                }

                default: {
                    printf("Was not able to create a string for a unary expr,");
                    printf("cause the unary operator type is not supported. ");
                    printf("Operator: %c. \n", expr->union_.unary.operator.type);
                    exit(1);

                    break;
                }
            }

            break;
        }

        case Expr_type_binary: {
            Expr* child_left_expr = expr->union_.binary.left;
            Expr* child_right_expr = expr->union_.binary.right;

            String child_left_expr_str = expr_to_string(child_left_expr);
            String child_right_expr_str = expr_to_string(child_right_expr);

            char* operator;
            switch (expr->union_.binary.operator.type) {
                case (int)'+': { operator = "+ "; break; }
                case (int)'-': { operator = "- "; break; }
                case (int)'*': { operator = "* "; break; }
                case (int)'/': { operator = "/ "; break; }
                case (int)'>': { operator = "> "; break; }
                case (int)'<': { operator = "< "; break; }

                case Token_Type_Greater_Or_Equals: { operator = ">= "; break; }
                case Token_Type_Less_Or_Equals: { operator = "<= "; break; }
                case Token_Type_Equals_Equals: { operator = "== "; break; }
                case Token_Type_Not_Equals: { operator = "!= "; break; }

                default: {
                    printf("Was not able to create a string for a binary expr,");
                    printf("cause the binary operator type is not supported. ");
                    printf("Operator: %c. \n", expr->union_.binary.operator.type);
                    exit(1);

                    break;
                }
            }

            string_add_c_string(&str, "[");
            string_add_c_string(&str, operator);
            string_add_string(&str, &child_left_expr_str);
            string_add_string(&str, &child_right_expr_str);
            string_add_c_string(&str, "]");

            string_delete(&child_right_expr_str);
            string_delete(&child_left_expr_str);
            
            break;
        }

        default: {
            printf("Was not able to create a string, cause the expr type is not supported. \n");
            exit(1);

            break;
        }
    }

    return str;
}









