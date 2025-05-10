#include <stdlib.h>
#include "stdio.h"
#include "Ast.h"
#include "Lexer.h"
#include "my_String.h"
#include "Array_Stmt.h"

// Expr grammar below

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

    if (token.type == Token_Type_Integer) {
        int int_value = atoi(token.lexeme);
        primary = &(Primary) {
            .type = Token_Type_Integer,
            .union_.integer = int_value,
        };
    }

    if (token.type == Token_Type_True) {
        primary = &(Primary) {
            .type = Token_Type_True,
            .union_.boolean = true,
        };
    }

    if (token.type == Token_Type_False) {
        primary = &(Primary) {
            .type = Token_Type_True,
            .union_.boolean = false,
        };
    }

    // if (token.type == Token_Type_Double) {
    //     char *endptr;
    //     double double_value = strtod(token.lexeme, &endptr);
    //     primary = &(Primary) {
    //         .type = Token_Type_Double,
    //         .union_.double_ = double_value,
    //     };
    // }

    // if (token.type == Token_Type_String) {
    //     primary = &(Primary) {
    //         .type = Token_Type_String,
    //         .union_.string = token.lexeme,
    //     };
    // }

    if (primary == NULL) {
        printf("Was not able to parse the primary expr. Not yet supported. \n");
        exit(1);
    }

    Expr* expr = malloc(sizeof(Expr));
    if (expr == NULL) {
        printf("Was not able to initialise expr, ran out of memory. \n");
        exit(1);
    }

    *expr = (Expr) {
        .type = Expr_type_primary,
        .union_.primary = *primary,
    };
    return expr;
}

Expr* unary(Lexer* lexer) {
    if (lexer_peek_next_token(lexer).type == (int) '-') {
        Token operator = lexer_next_token(lexer);
        Expr* right    = unary(lexer);

        Expr* expr = malloc(sizeof(Expr));
        if (expr == NULL) {
            printf("Was not able to initialise expr, ran out of memory. \n");
            exit(1);
        }

        *expr = (Expr) {
            .type         = Expr_type_unary, 
            .union_.unary = {
                .operator = operator,
                .right    = right,
            },
        };
        return expr;
    }

    if (lexer_peek_next_token(lexer).type == (int) '!') {
        Token operator = lexer_next_token(lexer);
        Expr* right    = unary(lexer);

        Expr* expr = malloc(sizeof(Expr));
        if (expr == NULL) {
            printf("Was not able to initialise expr, ran out of memory. \n");
            exit(1);
        }

        *expr = (Expr) {
            .type         = Expr_type_unary, 
            .union_.unary = {
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


// Stmt grammar below

Stmt print_stmt(Lexer* lexer) {
    // "print" was alredy consumed by the caller
    Expr* expr = expression(lexer);
    lexer_consume_token__exits(lexer, (int) ';', "Invalid syntax, was expecting ';' at the end of expression");
    return (Stmt) {
        .type = Stmt_type_print,
        .expr = expr,
    };
}

Stmt expression_stmt(Lexer* lexer) {
    Expr* expr = expression(lexer);
    lexer_consume_token__exits(lexer, (int) ';', "Invalid syntax, was expecting ';' at the end of expression");
    return (Stmt) {
        .type = Stmt_type_expr,
        .expr = expr,
    };
}

Stmt statement(Lexer* lexer) {
    if (lexer_peek_next_token(lexer).type == Token_Type_Print) {
        lexer_next_token(lexer);
        return print_stmt(lexer);
    }

    return expression_stmt(lexer);
}

Stmt program(Lexer* lexer) {
    return statement(lexer);
}


// TODO: delete the dynamic AST tree

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

                case Token_Type_True: 
                case Token_Type_False: {
                    bool bool_value   = expr->union_.primary.union_.boolean;
                    char* bool_as_str = (bool_value == true ? "true" : "false"); 
                    string_add_c_string(&str, "(bool -> ");
                    string_add_c_string(&str, bool_as_str);
                    string_add_c_string(&str, ")"); 
                    
                    break;
                }

                // case Token_Type_String: {
                //     string_add_c_string(&str, "(string -> ");
                //     string_add_c_string(&str, expr->union_.primary.union_.string);
                //     string_add_c_string(&str, ")");

                //     break;
                // }

                // case Token_Type_Double: {
                //     char double_as_str[30]; 
                //     sprintf_s(double_as_str, 30, "%.3lf", expr->union_.primary.union_.double_);
                //     string_add_c_string(&str, "(double -> ");
                //     string_add_c_string(&str, double_as_str);
                //     string_add_c_string(&str, ")");
                    
                //     break;
                // }

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

                    string_add_c_string(&str, "("); 
                    string_add_c_string(&str, operator_str); 
                    string_add_string(&str, &child_expr_str);
                    string_add_c_string(&str, ")");
                
                    string_delete(&child_expr_str);
                    free(operator_str);
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
                case Token_Type_And: { operator = "and "; break; }
                case Token_Type_Or: { operator = "or "; break; }

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

String stmt_to_string(Stmt* stmt) {
    return expr_to_string(stmt->expr);
}
 

// Parser below

Parser parser_init(const char* text) {
    Lexer lexer         = lexer_init(text);
    Array_stmt stmt_arr = array_stmt_init(); 
    bool had_error      = false; 
    return (Parser) {lexer, stmt_arr, had_error};
}

void parser_parse(Parser* parser) {
    while(!lexer_is_at_end(&parser->lexer)) {
        Stmt stmt = statement(&parser->lexer);
        array_stmt_add(&parser->stmt_arr, stmt);
        lexer_skip_whitespaces(&parser->lexer);

    }
        
    // TODO: Delete expr inside the Stmt here

    for (int i=0; i<parser->stmt_arr.length; ++i) {
        Expr* expr         = parser->stmt_arr.arr[i].expr;
        String expr_as_str = expr_to_string(expr);
        string_print(&expr_as_str);
    }

    printf("\n\n");

    for (int i=0; i<parser->stmt_arr.length; ++i) {
        Stmt stmt = parser->stmt_arr.arr[i];
        parser_execute_statement(&stmt);
    }
}


// void parser_evaluate_expr(Parser* parser) {
//     Expr* expr      = parser->ast;
//     Evaluation eval = evalueate_expression(expr);
// }

Evaluation evaluate_expression(Expr* expr) {
    switch (expr->type) {
        case Expr_type_primary: {
            Token_Type primary_type = expr->union_.primary.type;
            
            if (primary_type == Token_Type_True || primary_type == Token_Type_False) {
                return (Evaluation) {
                    .type           = Evaluation_type_boolean,
                    .union_.boolean = expr->union_.primary.union_.boolean,
                };
            }
            
            if (primary_type == Token_Type_Integer) {
                return (Evaluation) {
                    .type           = Evaluation_type_integer,
                    .union_.integer = expr->union_.primary.union_.integer,
                };
            }

        }

        case Expr_type_unary: {
            Token_Type operator = expr->union_.unary.operator.type;
            Evaluation right    = evaluate_expression(expr->union_.unary.right);

            switch (operator) {
                case (int) '!': {
                    switch (right.type) {
                        case Evaluation_type_boolean: {
                            return (Evaluation) {
                                .type           = Evaluation_type_boolean,
                                .union_.boolean = ! right.union_.boolean, 
                            };
                        }

                        default: {
                            printf("Wasn't able to evaluate unary expr, it is not supported. \n");
                            printf("Operator token: ");
                            token_print(&expr->union_.binary.operator);
                            exit(1);
                        }
                    }
                }

                case (int) '-': {
                    switch (right.type) {
                        case Evaluation_type_integer: {
                            return (Evaluation) {
                                .type           = Evaluation_type_integer,
                                .union_.integer = - right.union_.integer, 
                            };
                        }

                        default: {
                            printf("Wasn't able to evaluate unary expr, it is not supported. \n");
                            printf("Operator token: ");
                            token_print(&expr->union_.binary.operator);
                            exit(1);
                        }
                    }
                }
            }

            switch (right.type) {
                case Evaluation_type_integer: {
                    return (Evaluation) {
                        .type           = Evaluation_type_integer,
                        .union_.integer = - right.union_.integer, 
                    };
                }

                default: {
                    printf("Wasn't able to evaluate unary expr, it is not supported. \n");
                    printf("Operator token: ");
                    token_print(&expr->union_.binary.operator);
                    exit(1);
                }
            }
        }

        case Expr_type_binary: {
            Token_Type operator =  expr->union_.binary.operator.type;
            Evaluation left     = evaluate_expression(expr->union_.binary.left);
            Evaluation right    = evaluate_expression(expr->union_.binary.right);

            switch (operator) {
                case (int) '+': {
                    if (
                        left.type  == Evaluation_type_integer &&
                        right.type == Evaluation_type_integer
                    ) {
                        
                        return (Evaluation) {
                            .type           = Evaluation_type_integer,
                            .union_.integer = left.union_.integer + right.union_.integer, 
                        };
                    }

                    
                    printf("Error, unsupported addition for types. \n");
                    exit(1);

                    break;
                }
                case (int) '-': {
                    if (
                        left.type  == Evaluation_type_integer &&
                        right.type == Evaluation_type_integer
                    ) {
                        
                        return (Evaluation) {
                            .type           = Evaluation_type_integer,
                            .union_.integer = left.union_.integer - right.union_.integer, 
                        };
                    }

                    
                    printf("Error, unsupported substaction for types. \n");
                    exit(1);

                    break;
                }
                case (int) '*': {
                    if (
                        left.type  == Evaluation_type_integer &&
                        right.type == Evaluation_type_integer
                    ) {
                        
                        return (Evaluation) {
                            .type           = Evaluation_type_integer,
                            .union_.integer = left.union_.integer * right.union_.integer, 
                        };
                    }

                    printf("Error, unsupported multiplication for types. \n");
                    exit(1);

                    break;
                }
                case (int) '/': {
                    if (
                        left.type  == Evaluation_type_integer &&
                        right.type == Evaluation_type_integer
                    ) {
                        
                        return (Evaluation) {
                            .type           = Evaluation_type_integer,
                            .union_.integer = (int) (left.union_.integer / right.union_.integer), 
                        };
                    }

                    printf("Error, unsupported division for types. \n");
                    exit(1);

                    break;
                }
                case Token_Type_And: {
                    if (
                        left.type  == Evaluation_type_boolean &&
                        right.type == Evaluation_type_boolean
                    ) {
                        
                        return (Evaluation) {
                            .type           = Evaluation_type_boolean,
                            .union_.boolean = left.union_.boolean && right.union_.boolean,  
                        };
                    }

                    printf("Error, unsupported 'and' for types. \n");
                    exit(1);

                    break;
                }
                case Token_Type_Or: {
                    if (
                        left.type  == Evaluation_type_boolean &&
                        right.type == Evaluation_type_boolean
                    ) {
                        
                        return (Evaluation) {
                            .type           = Evaluation_type_boolean,
                            .union_.boolean = left.union_.boolean || right.union_.boolean,  
                        };
                    }

                    printf("Error, unsupported 'or' for types. \n");
                    exit(1);

                    break;
                }

                default: {
                    printf("Wasn't able to evaluate binary expr, it is not supported. \n");
                    printf("Operator token: ");
                    token_print(&expr->union_.binary.operator);
                    exit(1);
                }

                // TODO: other bin operators
            }


        }

        default: {
            printf("Was not able to evauate expr, expr type unsupported. \n");
            exit(1);
        }
    }
}


void parser_execute_statement(Stmt* stmt) {
    switch (stmt->type) {
        case Stmt_type_expr: {
            Expr* expr      = stmt->expr;
            Evaluation eval = evaluate_expression(expr);
            printf("Evaluated \n");

            break;
        }

        case Stmt_type_print: {
            Expr* expr      = stmt->expr;
            Evaluation eval = evaluate_expression(expr);
            switch (eval.type) {
                case Evaluation_type_integer: {
                    printf("%d", eval.union_.integer);
                    printf("\n");
                    break;
                }

                case Evaluation_type_boolean: {
                    if (eval.union_.boolean == true) 
                        printf("true");
                    else 
                        printf("false");

                    printf("\n");
                    
                    break;
                }

                default: {
                    printf("Got an unexpected evaluation type inside \"parser_execute_stmt\". \n");
                    exit(1);
                }

            }
            break;
        }

        default: {
            printf("Was not able to execute a statement. Statement type is unsupported. \n");
            printf("\tStatement: ");
            String expr_as_str = expr_to_string(stmt->expr);
            string_print(&expr_as_str);
            printf("\n");
            exit(1);
        }

    }
} 


// TODO-LIST: 
//      -- Delete dynamic Expr from heap.


