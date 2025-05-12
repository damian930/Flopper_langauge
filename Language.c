#include "Langauge.h"
#include "string.h"

// ================================================================================
// == Hash map for variables (name -> value)

Map_variables map_variables_init() {
    return (Map_variables) {
        .hash_variable_tuples = array_init(Array_type_tuple__hash_variable),
    };
}

void map_variables_delete(Map_variables* map) {
    for (int i=0; i<map->hash_variable_tuples.length; ++i) {
        Tuple__hash_variables* tuple = 
            ((Tuple__hash_variables*) map->hash_variable_tuples.arr);
        
            for (int j=0; j<tuple->variables.length; ++j) {
                Variable* var = 
                    ((Variable*) tuple->variables.arr);

                string_delete(&var->name);
            }
        array_delete(&tuple->variables);
    }

    array_delete(&map->hash_variable_tuples);
}


int map_variables_hash(String name) {
    return name.length;
}

// NOTE: If a variable is already in the map, it just adds a copy of it
int map_variables_add(Map_variables* map, String name, Evaluation value) {
    // Getting hash
    int hash = map_variables_hash(name);

    // Searching for the index of hash value
    int tuple_idx_for_hash = -1;
    for (int i=0; i<map->hash_variable_tuples.length; ++i) {
        Tuple__hash_variables* tuple_arr = 
            ((Tuple__hash_variables*) map->hash_variable_tuples.arr);
        if (tuple_arr[i].hash == hash) {
            tuple_idx_for_hash = i;
            break;
        }
    }

    // Not found
    if (tuple_idx_for_hash == -1) {
        Tuple__hash_variables new_tuple = (Tuple__hash_variables) {
            .hash      = hash,
            .variables = array_init(Array_type_variable),
        };
        Variable new_var = (Variable) { .name = name, .value = value };
        array_add(&new_tuple.variables, (void*) &new_var, Array_type_variable); 
        
        array_add(&map->hash_variable_tuples, (void*) &new_tuple, Array_type_tuple__hash_variable); 

        return 0;
    }
    else { // Found
        Tuple__hash_variables* tuple = 
            ((Tuple__hash_variables*) map->hash_variable_tuples.arr) + tuple_idx_for_hash;
        
        Variable new_var = (Variable) { .name = name, .value = value };

        // Check if the tuple arr doesn't alredy have the variable initialised
        for (int i=0; i<tuple->variables.length; ++i) {
            Variable temp = ((Variable*) tuple->variables.arr)[i];
            if (string_equal_to_string(&temp.name, &new_var.name)) {
                return -1;
            }
        }

        array_add(&tuple->variables, (void*) &new_var, Array_type_variable); 
        return 0;
    }

    //printf("Current map varaibles: \n");
    //for (int i=0; i<map->hash_variable_tuples.length; ++i) {
    //    Tuple__hash_variables tuple =
    //        ((Tuple__hash_variables*) map->hash_variable_tuples.arr)[i];

    //    for (int j=0; j<tuple.variables.length; ++j) {
    //        Variable var = ((Variable*)tuple.variables.arr)[j];
    //        //printf("\tVariable: ");
    //        //string_print(&var.name);
    //        //printf("\n");
    //    }
    //}

}

// NOTE: returning a pointer to be able to send out NULL if the varaible is not found
//       the behaviour is the handled by the caller
Variable* map_variables_get(Map_variables* map, String name) {
    int hash = map_variables_hash(name);

    int tuples_len = map->hash_variable_tuples.length;
    Tuple__hash_variables* tuples = 
            ((Tuple__hash_variables*) map->hash_variable_tuples.arr);
    
    for (int i=0; i<tuples_len; ++i) {
        int test_len = tuples->variables.length;
        // TODO: it might be, that this is not initialised yet (variables inside tuple)
        for (int i = 0; i < test_len; ++i) {
            Variable test_var = ((Variable*) tuples->variables.arr)[i];
            printf("test_var.name      : %s \n", test_var.name.str  );
            printf("test_var.value.type: %d \n", test_var.value.type);
        }


        if (tuples[i].hash == hash) {
            int variables_len   = tuples[i].variables.length; 
            Variable* variables = 
                ((Variable*) tuples[i].variables.arr);
            
            for (int j=0; j<variables_len; ++j) {
                if (string_equal_to_string(&variables[j].name, &name)) {
                    return variables + j;
                }
            }
            
        } 
    }
    return NULL;
    // printf("Was trying to acces a value for an unexsistant variable. \n");
    // printf("Identifier: %s. \n", name.str);
    // exit(1);
}


// ================================================================================
// == Language

Language language_init(const char* text) {
    Language language = {
        .parser              = parser_init(text),
        .variable_scopes_arr = array_init(Array_type_map_variables),
    };
    // NOTE: providing the language environment with a default scope 
    Map_variables scope = map_variables_init();
    array_add(&language.variable_scopes_arr, (void*) &scope, Array_type_map_variables);
    return language;
}

void language_delete(Language* language) {
    for (int i=0; i<language->variable_scopes_arr.length; ++i) {
        Map_variables* scope_vars = ((Map_variables*) language->variable_scopes_arr.arr) + i; 
        map_variables_delete(scope_vars);
    }

    array_delete(&language->variable_scopes_arr);
}

void language_execute(Language* language) {
    parser_parse(&language->parser); // Created statements

    for (int i=0; i<language->parser.stmt_arr.length; ++i) {
        Stmt* stmt = ((Stmt*) language->parser.stmt_arr.arr) + i;
        language_execute_statement(language, stmt);
    }

}

void language_execute_statement(Language* language, Stmt* stmt) {
    switch (stmt->type) {
        case Stmt_type_expr: {
            Expr* expr      = stmt->union_.stmt_expr.expr;
            Evaluation eval = language_evaluate_expression(language, expr);
            evaluation_print(&eval);

            break;
        }

        case Stmt_type_print: {
            Expr* expr      = stmt->union_.print.expr;
            Evaluation eval = language_evaluate_expression(language, expr);
            printf("Print: \"");
            evaluation_print(&eval);
            printf("\"");
            printf("\n");

            break;
        }

        // NOTE: this handles var creating. 
        //       it adds new variables to the last created scope.
        //       there is always at least a single scope there.
        case Stmt_type_declaration: {
            // TODO: why do i have a token here ef i create a string inside the primary var decalration thingy
            Token name_token           = stmt->union_.var_decl.var_name;
            Token specified_type_token = stmt->union_.var_decl.var_specified_type;
            
            char* name_buffer = malloc(sizeof(char) * name_token.length);
            if (name_buffer == NULL) {
                printf("Wasnt able to create a dynamic char array, no memory. \n");
                exit(1);
            } 
            
            for (int i=0; i<name_token.length; ++i) {
                name_buffer[i] = name_token.lexeme[i];
            }
            String name_as_str = string_init("");
            string_add_c_string(&name_as_str, name_buffer, name_token.length);
            free(name_buffer);
            
            Evaluation eval;
            if (stmt->union_.var_decl.init_expr == NULL) { // No assigment
                eval = (Evaluation) {.type = Evaluation_type_absent, .union_.integer = 0}; /* the = 0 is a palceholder*/
            }   
            else {
                eval = language_evaluate_expression(language, stmt->union_.var_decl.init_expr);
            }

            // Checking if the specified type is the same type as the type of the init value expression
            String type_as_str = string_init("");
            string_add_c_string(&type_as_str, specified_type_token.lexeme, specified_type_token.length);
            if (eval.type == Evaluation_type_absent) {
                // When its absent, then its just not initialised, but the specified type stays
                int x = 1;
            }
            else if (
                eval.type == Evaluation_type_integer && 
                strcmp(type_as_str.str, "int") == 0
            ) {
                int x = 1;
            }
            else if (
                eval.type == Evaluation_type_boolean && 
                strcmp(type_as_str.str, "bool") == 0
            ) {
                int x = 1;
            }
            else {
                printf("The specified type for variable %s and the type of the initialisation expression were not the same. \n", name_as_str.str);
                exit(1);
            }
            string_delete(&type_as_str);
            
            // Adding to the last variable scope
            int scope_length          = language->variable_scopes_arr.length;
            int last_scope_idx        = (scope_length == 0 ? 0 : scope_length - 1);
            Map_variables* last_scope = ((Map_variables*) language->variable_scopes_arr.arr) + last_scope_idx;
            
            // NOTE: language by default has at least 1 scope
            int err = map_variables_add(last_scope, name_as_str, eval);
            if (err == -1 ) {
                printf(
                    "Error: Redefenition of a variable '%s', col: %d, row: %d.\n", 
                    name_as_str.str,
                    name_token.column,
                    name_token.line
                );
                exit(1);
            }

            printf("Declared ("); 
            printf("%s", name_as_str.str);
            printf(": "); 
            if(eval.type == Evaluation_type_integer) printf("int");
            else if(eval.type == Evaluation_type_boolean) printf("bool");
            else if(eval.type == Evaluation_type_absent)  printf("Un Initialised");
            else printf("Unsupported eval type for printing");
            printf(")\n");

            break;
        }

        case Stmt_type_declaration_auto: {
            // TODO: why do i have a token here ef i create a string inside the primary var decalration thingy
            Token name_token  = stmt->union_.var_decl_auto.var_name;
            
            char* name_buffer = malloc(sizeof(char) * name_token.length);
            if (name_buffer == NULL) {
                printf("Wasnt able to create a dynamic char array, no memory. \n");
                exit(1);
            } 
            
            for (int i=0; i<name_token.length; ++i) {
                name_buffer[i] = name_token.lexeme[i];
            }
            String name_as_str = string_init("");
            string_add_c_string(&name_as_str, name_buffer, name_token.length);
            free(name_buffer);
            
            Evaluation eval;
            if (stmt->union_.var_decl_auto.init_expr == NULL) { // No assigment
                printf("FEEL LIKE THIS SHOULD NEVER BE CALLED");
                printf("Error: varaible creationg using ':=' has to have a right value for initialisation. \n");
                printf("       to declare a varaible with no value, use a type specifier like: \n");
                printf("       %s: <type> \n", name_as_str.str);
                exit(1);
            }   
            else {
                eval = language_evaluate_expression(language, stmt->union_.var_decl_auto.init_expr);
            }


            
            // Adding to the last variable scope
            int scope_length          = language->variable_scopes_arr.length;
            int last_scope_idx        = (scope_length == 0 ? 0 : scope_length - 1);
            Map_variables* last_scope = ((Map_variables*) language->variable_scopes_arr.arr) + last_scope_idx;
            
            // NOTE: language by default has at least 1 scope
            int err = map_variables_add(last_scope, name_as_str, eval);
            if (err == -1 ) {
                printf(
                    "Error: Redefenition of a variable '%s', col: %d, row: %d.\n", 
                    name_as_str.str,
                    name_token.column,
                    name_token.line
                );
                exit(1);
            }

            printf("Declared ("); 
            printf("%s", name_as_str.str);
            printf(": "); 
            if(eval.type == Evaluation_type_integer) printf("int");
            else if(eval.type == Evaluation_type_boolean) printf("bool");
            else if(eval.type == Evaluation_type_absent)  printf("Un Initialised");
            else printf("Unsupported eval type for printing");
            printf(")\n");

            break;
        }

        // NOTE: this create new scopes.
        //       if handles scope creation.
        //       scopes are created here and added to language scopes arr at the last position.
        //       then when the nested statements are executed, they will be handles by their own handlers.
        case Stmt_type_scope: {
            // Creating new scope and placing it at the last position of lang scopes
            Map_variables new_scope = map_variables_init();
            array_add(&language->variable_scopes_arr, &new_scope, Array_type_map_variables); 

            // Executing statements
            //Array* stmt_arr = &stmt->union_.scope.statements;
            Array stmt_arr = stmt->union_.scope.statements;
            for (int i=0; i<stmt_arr.length; ++i) {
                Stmt* scope_nested_stmt = ((Stmt*) stmt_arr.arr) + i;
                language_execute_statement(language, scope_nested_stmt);
            }     

            // Removing new scope
            map_variables_delete(&new_scope);           // This one here is a sticky one, TODO: write a better explanation for this here
            language->variable_scopes_arr.length -= 1;
            {
                // TODO: maybe dont do it by hand
                if (language->variable_scopes_arr.length < 1) {
                    printf("BACK_END_ERROR: somehow the lang object is left with no scopes. \n");
                    printf("BACK_END_ERROR: Called by \"language_execute_statement(Language* language, Stmt* stmt)\" \n");
                    exit(1);
                }
            }

            break;
        }

        case Stmt_type_if: {
            Stmt_if if_stmt       = stmt->union_.if_else;
            bool executed_if_stmt = false; 
            Evaluation main_expr_eval = language_evaluate_expression(language, if_stmt.main_if_expr);
            if (main_expr_eval.type == Evaluation_type_boolean) {
                if (main_expr_eval.union_.boolean == true) {
                    language_execute_statement(language, if_stmt.main_if_scope);
                    executed_if_stmt = true;
                }
            }
            else {
                printf("Was not able to evaluate a condition for and if statement, the value of the condition is not of a boolean type. \n");
                exit(1);
            }

            if (!executed_if_stmt) {
                for (int i=0; i<if_stmt.expr_scope_tuples.length; ++i) {
                    Tuple__expr_scope* tuple = ((Tuple__expr_scope*) if_stmt.expr_scope_tuples.arr) + i;
                    
                    Evaluation else_if_expr_eval = language_evaluate_expression(language, tuple->expr);
                    if (else_if_expr_eval.type == Evaluation_type_boolean) {
                        if (else_if_expr_eval.union_.boolean == true) {
                            language_execute_statement(language, tuple->scope);
                            executed_if_stmt = true;
                        }
                    }
                    else {
                        printf("Was not able to evaluate a condition for and else if statement, the value of the condition is not of a boolean type. \n");
                        exit(1);
                    }
                }
            }

            if (!executed_if_stmt) {
                language_execute_statement(language, if_stmt.else_scope);
            }

            break;
        }

        default: {
            printf("Was not able to execute a statement. Statement type is unsupported. \n");
            printf("\tStatement: ");
            //String expr_as_str = expr_to_string(stmt->union_.);
            // string_print(&expr_as_str);
            printf("\n");
            exit(1);
        }

    }
} 

// Given an expression, manually evaluates it
Evaluation language_evaluate_expression(Language* language, Expr* expr) {
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

            if (primary_type == Token_Type_Identifier) {
                String var_name = expr->union_.primary.union_.identifier;

                // 1. Check the newest scope. If not then check the one before that. And so on
                Array* scopes = &language->variable_scopes_arr;
                for (int i=0; i<scopes->length; ++i) {
                    Map_variables* scope = ((Map_variables*) scopes->arr) + i;
                    Variable* var        = map_variables_get(scope, var_name);
                    
                    if (var == NULL) { // Variable not found in the current scope
                        // searching inside the next scope
                    }
                    else 
                        return var->value;
                }
                
                printf("Was not able to evaluate a varaible with name %s. Variable is not existant. \n", var_name.str);
                exit(1);
            }

            break;

        }

        case Expr_type_unary: {
            Token_Type operator = expr->union_.unary.operator.type;
            Evaluation right    = language_evaluate_expression(language, expr->union_.unary.right);

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
            Evaluation left     = language_evaluate_expression(language, expr->union_.binary.left);
            Evaluation right    = language_evaluate_expression(language, expr->union_.binary.right);

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
















