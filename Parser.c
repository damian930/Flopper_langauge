#include <stdlib.h>
#include "stdio.h"
#include "Parser.h"
#include "Lexer.h"
#include "my_String.h"
#include "Array.h"

// ========================================================================================
// == Grammar:
//      program         :: declaration*                                     
//      declaration     :: var_declaration | statement
//
//      var_declaration :: IDENTIFIER ":" IDENTIFIER (= expression)? ";"   <---                            
//      statement       :: expression_stmt | print_stmt             
//      expression_stmt :: expression ";"                          
//      print_stmt      :: "print" expression ";"                   
//
//      expression :: equality 
//      equality   :: and_or ( ( "!=" | "==" ) and_or )*                
//      and_or     :: comparison ( ( "and" | "or" )  comparison )*  
//      comparison :: term ( ( ">" | ">=" | "<" | "<=" ) term )* 
//      term       :: factor ( ( "-" | "+" ) factor )* 
//      factor     :: unary ( ( "/" | "*" ) unary )* 
//      unary      :: ( "!" | "-" ) unary
//                    | primary 
//      primary    :: INTEGER | BOOLEAN | "(" expression ")" 
// ========================================================================================

// ========================================================================================
// == Expression grammar below

Expr* primary(Lexer* lexer) {
    Token token = lexer_next_token(lexer);
    
    if (token.type == (int) '(') {
        Expr* grouped_expr = expression(lexer);
        lexer_consume_token__exits(
            lexer, 
            (int) ')', 
            "Was not able to parse a groupping, second ')' was missing."
        );
        return grouped_expr;
    }
    
    Primary* primary = NULL;
    switch (token.type) {
        case Token_Type_Integer: {
            int int_value = atoi(token.lexeme);
            primary = &(Primary) {
                .type           = Token_Type_Integer,
                .union_.integer = int_value,
            };
            break;
        }

        case Token_Type_True: {
            primary = &(Primary) {
                .type           = Token_Type_True,
                .union_.boolean = true,
            };
            break;
        }

        case Token_Type_False: {
            primary = &(Primary) {
                .type = Token_Type_False,
                .union_.boolean = false,
            };
            break;
        }

        case Token_Type_Identifier: {
            // TODO: check if i even need malloc here. i kanda do, but if the lifetypes of the source code and this are the same and i am fine not allocating this.
            char* name = malloc(sizeof(char) * token.length);
            if (name == NULL) {
                printf("Wasnt able to initialise a string due to lack of memory. \n");
                exit(1);
            }
            for (int i=0; i<token.length; ++i) {
                name[i] = token.lexeme[i];
            }
            String identifier = string_init("");
            string_add_c_string(&identifier, name, token.length);
            free(name);

            primary = &(Primary) {
                .type              = Token_Type_Identifier,
                .union_.identifier = identifier,
            };
            break;
        }

        default: {
            printf("Either was expecting an expression or was not able to parse the primary expr. Not yet supported. \n");
            exit(1);
        }
    }

    Expr* expr = malloc(sizeof(Expr));
    if (expr == NULL) {
        printf("Was not able to initialise expr, ran out of memory. \n");
        exit(1);
    }

    *expr = (Expr) {
        .type           = Expr_type_primary,
        .union_.primary = *primary,
    };
    return expr;
}

Expr* unary(Lexer* lexer) {
    Token next_token = lexer_peek_next_token(lexer);
    if (
        next_token.type == (int) '-' ||
        next_token.type == (int) '!'
    ) {
        Token operator = lexer_next_token(lexer);
        Expr* right    = unary(lexer);

        Expr* expr = malloc(sizeof(Expr));
        if (expr == NULL) {
            printf("Was not able to initialise expr, ran out of memory. \n");
            exit(1);
        }

        *expr = (Expr) {
            .type         = Expr_type_unary, 
            .union_.unary = (Unary) {
                .operator = operator,
                .right    = right,
            },
        };
        return expr;
    }

    return primary(lexer);
}

Expr* factor(Lexer* lexer) {
    Expr* left = unary(lexer);

    while (true) {
        Token next_token    = lexer_peek_next_token(lexer);
        if (next_token.type == (int) '*' || next_token.type == (int) '/') {
            Token operator  = lexer_next_token(lexer);
            Expr* right     = unary(lexer);

            Expr* new_expr = malloc(sizeof(Expr));
            if (new_expr == NULL) {
                printf("Was not able to allocated expr on heap. \n");
                exit(1);
            }

            *new_expr = (Expr) {
                .type = Expr_type_binary,
                .union_.binary = (Binary) {
                    .operator  = operator,
                    .left      = left,
                    .right     = right,
                },
            };
            left = new_expr;
        }
        else {
            break;
        }
    }   

    return left;
}

Expr* term(Lexer* lexer) {
    Expr* left = factor(lexer);

    while (true) {
        Token next_token     = lexer_peek_next_token(lexer);
        if (next_token.type == (int) '+' || next_token.type == (int) '-') {
            Token operator   = lexer_next_token(lexer);
            Expr* right      = factor(lexer);

            Expr* new_expr = malloc(sizeof(Expr));
            if (new_expr == NULL) {
                printf("Was not able to allocated expr on heap. \n");
                exit(1);
            }

            *new_expr = (Expr) {
                .type = Expr_type_binary,
                .union_.binary = (Binary) {
                    .operator  = operator,
                    .left      = left,
                    .right     = right,
                },
            };
            left = new_expr;
        }
        else {
            break;
        }
    }

    return left;
}

Expr* comparison(Lexer* lexer) {
    Expr* left = term(lexer);

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
            if (new_expr  == NULL) {
                printf("Was not able to allocated expr on heap. \n");
                exit(1);
            }

            *new_expr = (Expr) {
                .type = Expr_type_binary,
                .union_.binary = (Binary) {
                    .operator  = operator,
                    .left      = left,
                    .right     = right,
                },
            };
            left = new_expr;
        }
        else {
            break;
        }
    }

    return left;
}

Expr* and_or(Lexer* lexer) {
    Expr* left = comparison(lexer);

    while (true) {
        Token next_token = lexer_peek_next_token(lexer);
        if (   
            next_token.type == Token_Type_And ||
            next_token.type == Token_Type_Or
        ) {
            Token operator = lexer_next_token(lexer);
            Expr* right    = comparison(lexer);

            Expr* new_expr = malloc(sizeof(Expr));
            if (new_expr  == NULL) {
                printf("Was not able to allocated expr on heap. \n");
                exit(1);
            }

            *new_expr = (Expr) {
                .type = Expr_type_binary, 
                .union_.binary = (Binary) {
                    .operator  = operator,
                    .left      = left,
                    .right     = right,
                },
            };
            left = new_expr;
        }
        else {
            break;
        }
    }

    return left;
}

Expr* equality(Lexer* lexer) {
    Expr* left = and_or(lexer);

    while (true) {
        Token next_token = lexer_peek_next_token(lexer);
        if (   next_token.type == Token_Type_Equals_Equals 
            || next_token.type == Token_Type_Not_Equals 
        ) {
            Token operator = lexer_next_token(lexer);
            Expr* right    = and_or(lexer);

            Expr* new_expr = malloc(sizeof(Expr));
            if (new_expr  == NULL) {
                printf("Was not able to allocated expr on heap. \n");
                exit(1);
            }
            
            *new_expr = (Expr) {
                .type = Expr_type_binary,
                .union_.binary = (Binary) {
                    .operator  = operator,
                    .left      = left,
                    .right     = right,
                },
            };
            left = new_expr;
        }
        else {
            break;
        }
    }

    return left;
}

Expr* expression(Lexer* lexer) {
    return equality(lexer);
}

void expr_delete(Expr* expr) {
    if (expr->type == Expr_type_primary) return;

    if (expr->type == Expr_type_unary) {
        expr_delete(expr->union_.unary.right); // Delete the child expr
        
        free(expr->union_.unary.right);
        return;
    }

    if (expr->type == Expr_type_binary) {
        expr_delete(expr->union_.binary.left ); // Delete the child expr
        expr_delete(expr->union_.binary.right); // Delete the child expr
        
        free(expr->union_.binary.left );
        free(expr->union_.binary.right);
        return;
    }
    
}

String expr_to_string(Expr* expr) {
    String str = string_init("");

    switch (expr->type) {

        case Expr_type_primary: {
            switch (expr->union_.primary.type) {
                case Token_Type_Integer: {
                    char int_as_str[30];
                    _itoa_s(expr->union_.primary.union_.integer, int_as_str, 30, 10);
                    string_add_whole_c_string(&str, "(integer -> ");
                    string_add_whole_c_string(&str, int_as_str);
                    string_add_whole_c_string(&str, ")");
                                        
                    return str;
                }

                case Token_Type_True: 
                case Token_Type_False: {
                    bool bool_value   = expr->union_.primary.union_.boolean;
                    char* bool_as_str = (bool_value == true ? "true" : "false"); 
                    string_add_whole_c_string(&str, "(bool -> ");
                    string_add_whole_c_string(&str, bool_as_str);
                    string_add_whole_c_string(&str, ")");
                    
                    return str;
                }

                case Token_Type_Identifier: {
                    printf("", expr->union_.primary.union_.identifier.str);
                    string_add_whole_c_string(&str, "Identifier: ");
                    string_add_whole_c_string(&str, expr->union_.primary.union_.identifier.str);

                    break;

                }

                default: {
                    printf("Was not able to create a string for a primary expr,");
                    printf("cause the primary expr type is not supported for string creation. \n");
                    exit(1);

                }
            }

            break;
        }

        case Expr_type_unary: {
            switch (expr->union_.unary.operator.type) {
                case (int) '-':
                case (int) '!': {
                    Expr* child_expr = expr->union_.unary.right;
                    String child_expr_str = expr_to_string(child_expr);
                    
                    // Copying the operator lexeme 
                    char* operator_lexeme    = expr->union_.unary.operator.lexeme;
                    int operator_lexeme_size = expr->union_.unary.operator.length;
                    char* operator_str       = malloc(sizeof(char) * operator_lexeme_size + 1);
                    for (int i=0; i<operator_lexeme_size; ++i) {
                        operator_str[i] = operator_lexeme[i];
                    }
                    operator_str[operator_lexeme_size] = '\0';

                    string_add_whole_c_string(&str, "(");
                    string_add_whole_c_string(&str, operator_str);
                    string_add_whole_c_string(&str, &child_expr_str);
                    string_add_whole_c_string(&str, ")");
                
                    string_delete(&child_expr_str);
                    free(operator_str);
                    return str;

                }

                default: {
                    printf("Was not able to create a string for a unary expr,");
                    printf("cause the unary operator type is not supported for string creation. ");
                    printf("Operator: %c. \n", expr->union_.unary.operator.type);
                    exit(1);

                }
                
            }

        }

        case Expr_type_binary: {
            Expr* child_left_expr  = expr->union_.binary.left;
            Expr* child_right_expr = expr->union_.binary.right;

            String child_left_expr_str  = expr_to_string(child_left_expr);
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
                case Token_Type_And: { operator = "and "; break; }
                case Token_Type_Or: { operator = "or "; break; }

                default: {
                    printf("Was not able to create a string for a binary expr,");
                    printf("cause the binary operator type is not supported for string creation. ");
                    printf("Operator: %c. \n", expr->union_.binary.operator.type);
                    exit(1);

                    break;
                }
            }

            string_add_whole_c_string(&str, "[");
            string_add_whole_c_string(&str, operator);
            string_add_whole_c_string(&str, child_left_expr_str.str);
            string_add_whole_c_string(&str, child_right_expr_str.str);
            string_add_whole_c_string(&str, "]");

            string_delete(&child_right_expr_str);
            string_delete(&child_left_expr_str);

            return str;

        }

        default: {
            printf("Was not able to create a string, cause the expr type is not supported for string creation. \n");
            exit(1);

        }
    }

    return str;
}


// ========================================================================================
// == Expression grammar below

Stmt print_stmt(Lexer* lexer) {
    // "print" was alredy consumed by the caller
    Expr* expr = expression(lexer);
    lexer_consume_token__exits(lexer, (int) ';', "Invalid syntax, was expecting ';' at the end of expression");
    return (Stmt) {
        .type              = Stmt_type_print,
        .union_.print.expr = expr,
    };
}

Stmt expression_stmt(Lexer* lexer) {
    Expr* expr = expression(lexer);
    lexer_consume_token__exits(lexer, (int) ';', "Invalid syntax, was expecting ';' at the end of expression");
    return (Stmt) {
        .type                  = Stmt_type_expr,
        .union_.stmt_expr.expr = expr, 
    };
}

Stmt statement(Lexer* lexer) {
    if (lexer_peek_next_token(lexer).type == Token_Type_Print) {
        lexer_next_token(lexer); // NOTE: doing this here to now then check if it the correct token there
        return print_stmt(lexer);
    }

    return expression_stmt(lexer);
}

Stmt var_declaration_auto(Lexer* lexer) {
    Token var_name   = lexer_consume_token__exits(lexer, Token_Type_Identifier,       "Was expecting an identifier for variable name. \n");
    Token ________   = lexer_consume_token__exits(lexer, Token_Type_Declaration_Auto, "BACK_END_ERR: Was expecting a type specifier ':=' for var_declaration_auto.  \n"); // This is the lang back end error, and not Flopper source code error
    Expr* r_val_expr = expression(lexer);
    lexer_consume_token__exits(lexer, (int)';', "Was expecting a ';' for the end of a statement, but wasnt found. \n");

    // NOTE: type of the r_value is figured out by expression(Lexer* lexer) when creating a primary expression
    return (Stmt) {
        .type = Stmt_type_declaration,
        .union_.var_decl = (Stmt_var_decl) {
            .var_name    = var_name,
            .init_expr   = r_val_expr, 
        },
    };
}

Stmt var_declaration(Lexer* lexer) {
    Token var_name = lexer_consume_token__exits(lexer, Token_Type_Identifier, "Was expecting an identifier for variable name. \n");
    Token ________ = lexer_consume_token__exits(lexer, (int) ':'            , "Was expecting a type specifier ':' after a variable declaraion. \n");
    Token var_type = lexer_consume_token__exits(lexer, Token_Type_Identifier, "Was expecting an identifier of the type specification for variable creation. \n");

    Expr* r_value_epxr = NULL;

    if (lexer_match_token(lexer, (int) '=')) {
        r_value_epxr = expression(lexer);
    }
    lexer_consume_token__exits(lexer, (int)';', "Was expecting a ';' for the end of a statement, but wasnt found. \n");

    return (Stmt) {
        .type = Stmt_type_declaration,
        .union_.var_decl = (Stmt_var_decl) {
            .var_name    = var_name,
            .init_expr   = r_value_epxr, 
        },
    };

}

Stmt declaration(Lexer* lexer) {
    if (   lexer_peek_n_token(lexer, 1).type == Token_Type_Identifier 
        && lexer_peek_n_token(lexer, 2).type == (int) ':'
    ) {
        return var_declaration(lexer);
    }
    else if (
           lexer_peek_n_token(lexer, 1).type == Token_Type_Identifier
        && lexer_peek_n_token(lexer, 2).type == Token_Type_Declaration_Auto
    ) {
        return var_declaration_auto(lexer);
    }
    else 
        return statement(lexer);

}

Stmt program(Lexer* lexer) {
    return declaration(lexer);
}

// TODO: check if i have to switch over what type to delete.
//       might be able to just delte regardless since Expr is a dynamic union.
void stmt_delete(Stmt* stmt) {
    switch (stmt->type) {
        case Stmt_type_expr: {
            expr_delete(&stmt->union_.stmt_expr);
            return;
        }

        case Stmt_type_print: {
            expr_delete(&stmt->union_.print);
            return;    
        }
    
        case Stmt_type_declaration: {
            expr_delete(&stmt->union_.var_decl);
            return;
        }

        default: {
            printf("Was not able to delete a statement, statement's type is not supported for deletion. \n");
            exit(1);
        }
    }

}

String stmt_to_string(Stmt* stmt) {
    switch (stmt->type) {
        case Stmt_type_expr       : { return expr_to_string(stmt->union_.stmt_expr.expr     ); }
        case Stmt_type_print      : { return expr_to_string(stmt->union_.print.expr         ); }
        case Stmt_type_declaration: { return expr_to_string(stmt->union_.var_decl.init_expr ); }
        default: {
            printf("Was not able to create a string from a statement.");
            printf("Statement's type is not supported for string creation. \n");
            exit(1);
        }
    }
}


// ========================================================================================
// == Evaluation
 
void evaluation_print(Evaluation* eval) {
    switch (eval->type) {
        case Evaluation_type_integer: {
            printf("%d", eval->union_.integer);

            break;
        }

        case Evaluation_type_boolean: {
            if (eval->union_.boolean == true) 
                printf("true");
            else 
                printf("false");

            break;
        }

        case Evaluation_type_absent: {
            printf("Error, was trying to print an uninitialised varaible. \n");
            exit(1);
        }

        default: {
            printf("Got an unexpected evaluation type inside \"language_execute_stmt\". \n");
            exit(1);
        }
    }
    
}


// ========================================================================================
// == Parser

Parser parser_init(const char* text) {
    Lexer lexer    = lexer_init(text);
    Array stmt_arr = array_init(Array_type_stmt); 
    bool had_error = false; 
    return (Parser) {lexer, stmt_arr, had_error};
}

void parser_parse(Parser* parser) {
    while(!lexer_is_at_end(&parser->lexer)) {
        Stmt stmt  = program(&parser->lexer);
        String str = stmt_to_string(&stmt);
        printf("Stmt: %s \n", stmt_to_string(&stmt).str);

        array_add(&parser->stmt_arr, (void*) &stmt, Array_type_stmt);
        stmt_delete(&stmt);
    }
}

void parser_delete(Parser* parser) {
    // Deleting original data from parser stmt_arr
    for (int i=0; i<parser->stmt_arr.length; ++i) {
        Stmt* stmt = ((Stmt*) parser->stmt_arr.arr) + i;
        stmt_delete(stmt);
    }

    // Deleting the array itself
    array_delete(&parser->stmt_arr);
}


