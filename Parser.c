#include <stdlib.h>
#include "string.h"
#include "stdio.h"
#include "Parser.h"
#include "Lexer.h"
#include "my_String.h"
//#include "Array.h"

Array_init(Tuple__expr_scope, Expr_scope_array, expr_scope_array_init);
Array_delete(Expr_scope_array, expr_scope_array_delete);
Array_add_by_value(Tuple__expr_scope, Expr_scope_array, expr_scope_array_add_by_value);

Array_init(Stmt, Stmt_array, stmt_array_init);
Array_delete(Stmt_array, stmt_array_delete);
Array_add_by_value(Stmt, Stmt_array, stmt_array_add_by_value);

// ========================================================================================
// == Expression grammar below

Expr* primary(Lexer* lexer) {
    // TODO: do some else, hardcoding this is not great
    if (lexer_peek_nth_token(lexer, 1).type == (int)'*'
        &&
        lexer_peek_nth_token(lexer, 2).type == (int)'/') 
    {
        printf("Found a closing '*/' block comment, but the starting '/*' was never found. \n");
        exit(1);
    }

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
            primary = &(Primary) {
                .type              = Token_Type_Identifier,
                .union_.identifier = token,
            };

            // // Funcion call 
            // if (lexer_match_token(lexer, (int) '(')) {
            //     Array args = array_init(Array_type_expr_p); 
            //     if (lexer_match_token(lexer, (int) ')') == false) {
            //         do {
            //             Token token = lexer_next_token(lexer);
            //             array_add(&args, &token, Array_type_token);
            //         } while(lexer_match_token(lexer, (int)','));
            //         lexer_match_token(lexer, (int) ')');
            //     }

            //     primary = &(Primary) {
            //         .type = Token_Type_Function_For_Parser,
            //         .union_.func_call = (Func_call) {
            //             .name           = token,
            //             .args_as_expr_p = args,
            //         }
            //     };
            // }
            // else {
            //     // TODO: check if i even need malloc here. i kanda do, but if the lifetypes of the source code and this are the same and i am fine not allocating this.
            //     char* name = malloc(sizeof(char) * token.length);
            //     if (name == NULL) {
            //         printf("Wasnt able to initialise a string due to lack of memory. \n");
            //         exit(1);
            //     }
            //     for (int i=0; i<token.length; ++i) {
            //         name[i] = token.lexeme[i];
            //     }
            //     String identifier = string_init("");
            //     string_add_c_string(&identifier, name, token.length);
            //     free(name);

            //     primary = &(Primary) {
            //         .type              = Token_Type_Identifier,
            //         .union_.identifier = identifier,
            //     };
            // }

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
    switch (expr->type) {
        case Expr_type_primary: { return; }

        case Expr_type_unary: {
            expr_delete(expr->union_.unary.right); // Delete the child expr
        
            free(expr->union_.unary.right);
            return;    
        }

        case Expr_type_binary: {
            expr_delete(expr->union_.binary.left ); // Delete the child expr
            expr_delete(expr->union_.binary.right); // Delete the child expr
            
            free(expr->union_.binary.left );
            free(expr->union_.binary.right);
            return;
        }

        default: {
            printf("BACK_END_ERROR: Was not able to delete expr inside 'expr_delete(Expr* expr).' \n");
            exit(1);
        }

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
                    Token identifier = expr->union_.primary.union_.identifier;
                    string_add_whole_c_string(&str, "Identifier: ");
                    string_add_c_string(&str, identifier.lexeme, identifier.length);

                    return str;
                }
                
                case Token_Type_Function_For_Parser: {
                    string_add_whole_c_string(&str, "Function call \n");

                    return str;
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

Stmt block(Lexer* lexer) {
    // '{' has alredy been consumed by the caller
    Stmt_array stmt_arr = stmt_array_init();   
    while(!lexer_is_at_end(lexer)) {
        if (lexer_peek_next_token(lexer).type == (int) '}')
            break;
        Stmt stmt = declaration(lexer);
        stmt_array_add_by_value(&stmt_arr, stmt);
    }

    if (lexer_is_at_end(lexer)) {
        printf("Was expecting '}', but never found. Unfinished block of code. \n");
        exit(1);
    }
    else
        lexer_match_token(lexer, (int) '}');

    return (Stmt) {
        .type         = Stmt_type_scope,
        .union_.scope = (Stmt_scope) {
            .statements = stmt_arr,
        },
    };
}

Stmt print_stmt(Lexer* lexer) {
    // 'print' has alredy been consumed by the caller
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
    if (lexer_match_token(lexer, Token_Type_Print)) {
        return print_stmt(lexer);
    }

    if (lexer_match_token(lexer, (int) '{')) {
        return block(lexer);
    }

    return expression_stmt(lexer);
}

Stmt var_assignment(Lexer* lexer) {
    Token var_name  = lexer_consume_token__exits(lexer, Token_Type_Identifier, "BACK_END_ERR: Was expecting an identifier for variable name. \n");
    Token ________  = lexer_consume_token__exits(lexer, (int) '=', "BACK_END_ERR: Was expecting a type specifier ':=' for var_declaration_auto.  \n"); // This is the lang back end error, and not Flopper source code error
    Expr* expr      = expression(lexer);
    lexer_consume_token__exits(lexer, (int) ';', "Was specting a ';' at the end of the assigment r_value expression. \n");
    
    return (Stmt) {
        .type                  = Stmt_type_var_assignment,
        .union_.var_assignment = (Stmt_var_assignment) {
            .var_name       = var_name,
            .assigment_expr = expr,
        }
    };
}

Stmt var_declaration_auto(Lexer* lexer) {
    Token var_name   = lexer_consume_token__exits(lexer, Token_Type_Identifier,       "BACK_END_ERR: Was expecting an identifier for variable name. \n");
    Token ________   = lexer_consume_token__exits(lexer, Token_Type_Declaration_Auto, "BACK_END_ERR: Was expecting a type specifier ':=' for var_declaration_auto.  \n"); // This is the lang back end error, and not Flopper source code error
    Expr* r_val_expr = expression(lexer);

    lexer_consume_token__exits(lexer, (int)';', "Was expecting a ';' for the end of a statement, but wasnt found. \n");

    // NOTE: type of the r_value is figured out by expression(Lexer* lexer) when creating a primary expression
    return (Stmt) {
        .type = Stmt_type_declaration_auto,
        .union_.var_decl_auto = (Stmt_var_decl_auto) {
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
            .var_name           = var_name,
            .var_specified_type = var_type,
            .init_expr          = r_value_epxr, 
        },
    };

}

Stmt if_condition(Lexer* lexer) {
    // 'if' has alredy been consumed by the caller
    Expr* main_if_expr  = expression(lexer);
    lexer_consume_token__exits(lexer, (int)'{', "Was expecting a '{' after if condition. \n");
    
    Stmt  temp = block(lexer);
    if (temp.type != Stmt_type_scope) {
        printf("BACK_END_ERROR: Somehow 'block(Lexer* lexer) didnt return Stmt with type Stmt_type_scope. \n'");
        exit(1);
    }
    Stmt_scope main_if_scope = temp.union_.scope;

    
    // Getting else_if statements 
    Expr_scope_array expr_scope_arr = expr_scope_array_init();

    while(lexer_peek_nth_token(lexer, 1).type == Token_Type_Else &&
          lexer_peek_nth_token(lexer, 2).type == Token_Type_If
    ) {
        lexer_consume_token__exits(lexer, Token_Type_Else,   "BACK_END_ERROR: was expecting to cosume 'if' but if wasnt found.   \n");
        lexer_consume_token__exits(lexer, Token_Type_If, "BACK_END_ERROR: was expecting to cosume 'else' but if wasnt found. \n");
        
        Expr* else_if_expr  = expression(lexer);

        lexer_consume_token__exits(lexer, (int)'{', "Was expecting a '{' after if else condition. \n");

        Stmt* else_if_scope = malloc(sizeof(Stmt));
        if (else_if_scope == NULL) {
            printf("Error: Was not able to initialise memory. \n");
            exit(1);
        }
        *else_if_scope = block(lexer);

        Tuple__expr_scope* new_tuple = malloc(sizeof(Tuple__expr_scope));
        if (new_tuple == NULL) {
            printf("Error: Was not able to initialise memory. \n");
            exit(1);
        }
        *new_tuple = (Tuple__expr_scope) {
            .expr  = else_if_expr,
            .scope = else_if_scope,
        };

        expr_scope_array_add_by_value(&expr_scope_arr, *new_tuple);
    }

    Stmt_scope else_scope = {
        .statements = stmt_array_init(),
    };
    if (lexer_match_token(lexer, Token_Type_Else)) {
        lexer_consume_token__exits(lexer, (int)'{', "Was expecting a '{' after 'else'. \n");
        Stmt temp = block(lexer);

        if (temp.type != Stmt_type_scope) {
            printf("BACK_END_ERROR: Somehow 'block(Lexer* lexer) didnt return Stmt with type Stmt_type_scope. \n'");
            exit(1);
        }

        else_scope = temp.union_.scope;
    }

    // Creating the main Stmt_if struct
    Stmt_if whole_if_stmt = {
        .main_if_expr   = main_if_expr,
        .main_if_scope  = main_if_scope,
        .expr_scope_arr = expr_scope_arr,
        .else_scope     = else_scope,
    };

    return (Stmt) {
        .type           = Stmt_type_if,
        .union_.if_else = whole_if_stmt 
    };

}

// INTEGER ".." INTEGER (".." INTEGER)?
For_loop_range for_loop_range(Lexer* lexer) {
    Expr* start = expression(lexer);
    
    Token _____ = lexer_consume_token__exits(lexer, Token_Type_Dot_Dot, "Error: was expecting '..' after an integer that starts a range. \n");
    
    bool  include_end_value = false;
    if (lexer_match_token(lexer, (int) '='))
        include_end_value = true;
        
    Expr* end = expression(lexer);
    
    Expr* increment = NULL;
    if (lexer_match_token(lexer, Token_Type_Dot_Dot)) 
        increment = expression(lexer);
    
    return (For_loop_range) {
        .start             = start,
        .end               = end,
        .increment         = increment,
        .include_end_value = include_end_value,
    };
}

// "for" IDENTIFIER "in" for_loop_range block
Stmt for_loop(Lexer* lexer) {
    // "for" has alredy been consumed by the caller
    Token identifier     = lexer_consume_token__exits(lexer, Token_Type_Identifier, "Error: Was expecting an identifier after 'for', but wasn't found. \n");
    Token __________     = lexer_consume_token__exits(lexer, Token_Type_In,         "Error: Was expecting 'in' after 'for var', but wasn't found.      \n");
    For_loop_range range = for_loop_range(lexer); 

    lexer_consume_token__exits(lexer, (int) '{', "Error: Was expecting '{' to start a scope after range in a for loop, but wasn't found. \n");
    Stmt stmt_block = block(lexer);
    if (stmt_block.type != Stmt_type_scope) {
        printf("BACK_END_ERROR: Somehow block(Lexer* lexer) return Stmt with type other than Stmt_type_scope. \n");
        exit(1);
    }
    Stmt_scope scope = stmt_block.union_.scope;

    return (Stmt) {
        .type = Stmt_type_for_loop,
        .union_.for_loop = (Stmt_for_loop) {
            .range      = range,
            .scope      = scope,
            .identifier = identifier 
        }
    };
}

// "while" expression block
Stmt while_loop(Lexer* lexer) {
    // "while" has alredy been consumed by the caller
    Expr* condition = expression(lexer);
    if (lexer_match_token(lexer, (int)'{')) {
        // TODO: this can be used to only execute 1 statement, if the '{' was not found
    }
    else {
        printf("Error: Was expecting a '{' to start a while loop scope after 'while' and condition. \n");
        exit(1);
    }
    Stmt  scope     = block(lexer);

    Stmt* scope_dyn = malloc(sizeof(Stmt));
    if (scope_dyn == NULL) {
        printf("Was not able to intialise memory. \n");
        exit(1);
    }
    *scope_dyn = scope;

    // THINK ABOUT IT: i either store STMT and then have to malloc it, or store Stmt_scope but dont have to malloc it,
    //                 but then i need to wrap it into Stmt every time. Maybe there is some other way thats better.

    return (Stmt) {
        .type              = Stmt_type_while_loop,
        .union_.while_loop = (Stmt_while_loop) {
            .condition = condition,
            .scope     = scope_dyn,
        }
    };
}


// Stmt function_declaration(Lexer* lexer) {
//     Token name = lexer_consume_token__exits(lexer, Token_Type_Identifier,  "Was expecting an identifier to start a function declaration. \n");
//     Token _    = lexer_consume_token__exits(lexer, Token_Type_Colon_Colon, "Was expecting a '::' after function name in function declaration. \n");
//     Token __   = lexer_consume_token__exits(lexer, (int) '(',              "Was expecting a '(' after function name and '::' to start a list of function arguments. \n");

//     Array arg_names = array_init(Array_type_token);
//     Array arg_types = array_init(Array_type_token);
//     if (lexer_peek_next_token(lexer).type != (int) ')') {
//         do {
//             Token var_name = lexer_consume_token__exits(lexer, Token_Type_Identifier, "Was expecting a variable name inside a function arguments declaration. \n");
//             Token ___      = lexer_consume_token__exits(lexer, (int) ':',             "Was expecting ':' after varaible name to declare the type in function arguments declaration. \n");
            
//             Token var_type = lexer_consume_token__exits(lexer, Token_Type_Int_Type, "Was expecting a variable type int, other are not supported. \n");

//             array_add(&arg_names, &var_name, Array_type_token);
//             array_add(&arg_types, &var_type, Array_type_token);

//         } while(lexer_match_token(lexer, (int) ','));
//     }
//     lexer_consume_token__exits(lexer, (int) ')', "Was expecting a closing ')' to after the list of function arguments. \n");

//     if (arg_names.length != arg_types.length) {
//         printf("BACK_END_ERROR: Somehow the length of arg names and types is not the same. \n");
//         exit(1);
//     }

//     // Need to cosume for the block() to work
//     lexer_consume_token__exits(lexer, (int)'{', "Was expecting a starting '{' for function body. \n");
//     Stmt_scope scope = block(lexer).union_.scope; // Might break, assert

//     return (Stmt) {
//         .type             = Stmt_type_func_decl,
//         .union_.func_decl = (Stmt_func_decl) {
//             .name  = name,
//             .scope = scope, 
//             .arg_names_as_tokens = arg_names,
//             .arg_types_as_tokens = arg_types,
//         }
//     };
// }





Stmt declaration(Lexer* lexer) {
    if (   
           lexer_peek_nth_token(lexer, 1).type == Token_Type_Identifier 
        && lexer_peek_nth_token(lexer, 2).type == (int) ':'
    ) {
        return var_declaration(lexer);
    }
    else if (
           lexer_peek_nth_token(lexer, 1).type == Token_Type_Identifier
        && lexer_peek_nth_token(lexer, 2).type == Token_Type_Declaration_Auto
    ) {
        return var_declaration_auto(lexer);
    }
    else if (
           lexer_peek_nth_token(lexer, 1).type == Token_Type_Identifier
        && lexer_peek_nth_token(lexer, 2).type == (int) '='   
    ) {
        return var_assignment(lexer);
    }
    else if (lexer_match_token(lexer, Token_Type_If)) {
        return if_condition(lexer);
    }
    else if (lexer_match_token(lexer, Token_Type_For)) {
        return for_loop(lexer);
    }
    else if (lexer_match_token(lexer, Token_Type_While)) {
        return while_loop(lexer);
    }
    // else if (
    //         lexer_peek_nth_token(lexer, 1).type == Token_Type_Identifier
    //     &&  lexer_peek_nth_token(lexer, 2).type == Token_Type_Colon_Colon
    //     &&  lexer_peek_nth_token(lexer, 3).type == (int) '('
    // ) {
    //     return function_declaration(lexer);
    // }

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
            expr_delete(stmt->union_.stmt_expr.expr);
            return;
        }

        case Stmt_type_print: {
            expr_delete(stmt->union_.print.expr);
            return;    
        }
    
        case Stmt_type_declaration: {
            expr_delete(stmt->union_.var_decl.init_expr);
            return;
        }

        case Stmt_type_declaration_auto: {
            expr_delete(stmt->union_.var_decl_auto.init_expr);
            return;
        }

        case Stmt_type_var_assignment: {
            expr_delete(stmt->union_.var_assignment.assigment_expr);

            break;
        }

        case Stmt_type_scope: {
            Stmt_array stmt_arr = stmt->union_.scope.statements;
            
            // Deleting statements inside the scope
            for (int i=0; i<stmt_arr.length; ++i) {
                Stmt* scope_nested_stmt = stmt_arr.data + i;
                stmt_delete(scope_nested_stmt);
            }

            // Deleting the heap array that stored the statemnets inside the scope
            stmt_array_delete(&stmt_arr);

            return;
        }

        case Stmt_type_if: {
            Stmt_if if_stmt = stmt->union_.if_else;
            
            expr_delete(if_stmt.main_if_expr);


            Stmt scope_as_stmt = stmt_scope_to_stmt(&if_stmt.main_if_scope);
            stmt_delete(&scope_as_stmt);
            
            // Since the else_if tuples store pointers to dyn memory, have to iterate and free them manually
            for (int i=0; i<if_stmt.expr_scope_arr.length; ++i) {
                Tuple__expr_scope* expr_scope = if_stmt.expr_scope_arr.data + i;

                expr_delete(expr_scope->expr);

                scope_as_stmt = stmt_scope_to_stmt(&expr_scope->scope);
                stmt_delete(&scope_as_stmt);
            }
            expr_scope_array_delete(&if_stmt.expr_scope_arr);

            scope_as_stmt = stmt_scope_to_stmt(&if_stmt.else_scope);
            stmt_delete(&stmt_delete);

            break;
        }

        case Stmt_type_for_loop: {
            // TODO: this has to change, just store STMT inside for_loop_stmt
            Stmt temp = {
                .type         = Stmt_type_scope,
                .union_.scope = stmt->union_.for_loop.scope,
            };
            stmt_delete(&temp);
            
            expr_delete(stmt->union_.for_loop.range.start);
            expr_delete(stmt->union_.for_loop.range.end);

            if (stmt->union_.for_loop.range.increment != NULL)
                expr_delete(stmt->union_.for_loop.range.increment);

            break;
        }

        case Stmt_type_while_loop: {
            expr_delete(stmt->union_.while_loop.condition);
            
            Stmt scope_as_stmt = stmt_scope_to_stmt(&stmt->union_.while_loop.scope);
            stmt_delete(&scope_as_stmt);

            break;
        }

        // case Stmt_type_func_decl: {
        //     printf("Need to be aable to delete a func delc bro.");
        //         //array_delete(&stmt->union_.func_decl.argument_tokens);

        //         //// TODO: this has to change, just store STMT inside for_loop_stmt
        //         //Stmt temp = {
        //         //    .type         = Stmt_type_scope,
        //         //    .union_.scope = stmt->union_.func_decl.scope,
        //         //};
        //         //stmt_delete(&temp);
        //        break;
        // }

        default: {
            printf("Was not able to delete a statement inside \"stmt_delete(Stmt* stmt)\", statement's type is not supported for deletion. \n");
            exit(1);
        }
    }

}

String stmt_to_string(Stmt* stmt) {
    switch (stmt->type) {
        case Stmt_type_expr            : { return expr_to_string(stmt->union_.stmt_expr.expr           ); }
        case Stmt_type_print           : { return expr_to_string(stmt->union_.print.expr               ); }
        case Stmt_type_declaration     : { return expr_to_string(stmt->union_.var_decl.init_expr       ); }
        case Stmt_type_declaration_auto: { return expr_to_string(stmt->union_.var_decl_auto.init_expr  ); }
        case Stmt_type_var_assignment  : { return string_init("ASSIGNMENT");                              }
        case Stmt_type_scope           : { return string_init("PRINT FOR SCOPES IS NOT YET IMPLEMENTED"); }
        case Stmt_type_if              : { return string_init("IF STMT");                                 }
        case Stmt_type_for_loop        : { return string_init("FOR LOOP");                                }
        case Stmt_type_while_loop      : { return string_init("WHILE LOOP");                              }
        //case Stmt_type_func_decl       : { return string_init("DECLARED A FUNCTION");                     }                            
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

        // case Evaluation_type_absent: {
        //     printf("Error, was trying to print an uninitialised varaible. \n");
        //     exit(1);
        // }

        default: {
            printf("Got an unexpected evaluation type inside \"language_execute_stmt\". \n");
            exit(1);
        }
    }
    
}


// ========================================================================================
// == Parser

Parser parser_init(const char* text) {
    Lexer lexer         = lexer_init(text);
    Stmt_array stmt_arr = stmt_array_init();

    return (Parser) { lexer, stmt_arr };
}

void parser_parse(Parser* parser) {
    while (lexer_peek_next_token(&parser->lexer).type != Token_Type_EOF) {
        Stmt stmt  = program(&parser->lexer);
        String str = stmt_to_string(&stmt);
        printf("Stmt: %s \n", stmt_to_string(&stmt).str);

        stmt_array_add_by_value(&parser->stmt_arr, stmt);
        string_delete(&str);

        // NOTE: stmt holds a dynamic Expr. Deleting it will remove the nested expr.
        //       BUT, when we add a stmt to an arr, it add a copy, but it does the default c copying (BY VALUE).
        //       SO,  so the pointer is different, but the heap memory is the same, calling this will remove it,
        //              and the pointer inside the array will start working with random values in memory. 
        //stmt_delete(&stmt);
    }
}

void parser_delete(Parser* parser) {
    // Deleting original data from parser stmt_arr
    for (int i=0; i<parser->stmt_arr.length; ++i) {
        Stmt* stmt = parser->stmt_arr.data + i;
        stmt_delete(stmt);
    }

    // Deleting the array itself
    stmt_array_delete(&parser->stmt_arr);
}





// Dont know where to put this, need to create a separte file for utils

Stmt stmt_scope_to_stmt(Stmt_scope* scope) {
    return (Stmt) {
        .type = Stmt_type_scope,
        .union_.scope = *scope,
    };
}


