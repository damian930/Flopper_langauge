#include "Langauge.h"
#include "string.h"


// ================================================================================
// Plan:
//  Here will be thing representing a single scope for a language
//  It will contain variables, and in the future enums and stuff.
//  This way i will be able to nest scopes, like in swift and acces stuff from them. 
//  Also using this struct i will be able to limit redefenitions in nested scopes.

// TODO: First implement just using regular Array, switch to hash tables later for better retrival

void tuple__string_evaluation_delete(Tuple__string_evaluation* tuple) {
    string_delete(&tuple->str);
}


// ============ Scope 
Evaluation* language_scope_get_value_for_varaible(Language_scope* scope, String name) {
    Tuple__string_evaluation* variables_arr = ((Tuple__string_evaluation*) scope->variables.arr);

    for (int i=0; i<scope->variables.length; ++i) {
        bool are_equal = string_equal_to_string(&(variables_arr + i)->str, &name);  
        if (are_equal) {
            return &(variables_arr + i)->eval;
        }
    }
    return NULL;
}

Stmt_func_decl* language_scope_get_func_decl_for_name(Language_scope* scope, String requested_name) {
    Stmt_func_decl* funcs = ((Stmt_func_decl*) scope->functions.arr);

    for (int i=0; i<scope->functions.length; ++i) {
        String name_as_str = string_init("");
        int test = (funcs + i)->name.length;
        string_add_c_string(&name_as_str, (funcs + i)->name.lexeme, (funcs + i)->name.length);
        bool are_equal = string_equal_to_string(&requested_name, &name_as_str);
        string_delete(&name_as_str);
        if (are_equal) {
            return (funcs + i);
        }
    }


    return NULL;
}

int language_scope_add_varaible(Language_scope* scope, Tuple__string_evaluation* new_var) {
    int variable_arr_len = scope->variables.length;

    // Checking if the array already has a variable with this name
    if (language_scope_get_value_for_varaible(scope, new_var->str) != NULL) {
        return -1;
    }

    // No redeclaration, success
    array_add(&scope->variables, new_var, Array_type_tuple_string_evaluation); 
    return 0;
}

void language_scope_delete(Language_scope* scope) {
    for (int i=0; i<scope->variables.length; ++i) {
        Tuple__string_evaluation* tuple = 
            ((Tuple__string_evaluation*) scope->variables.arr) + i;
        
        string_delete(&tuple->str);
    }
    array_delete(&scope->variables);
}


// ================================================================================
// == Language

Language language_init(const char* text) {
    Language language = {
        .parser = parser_init(text),
        .scopes = array_init(Array_type_language_scope),
    };

    // NOTE: providing the language environment with a default scope 
    Language_scope default_scope = {
        .variables = array_init(Array_type_tuple_string_evaluation),
        .functions = array_init(Array_type_stmt),
    };
    array_add(&language.scopes, (void*) &default_scope, Array_type_language_scope);

    return language;
}

void language_delete(Language* language) {
    for (int i=0; i<language->scopes.length; ++i) {
        Language_scope* scope = ((Language_scope*) language->scopes.arr) + i;
        language_scope_delete(scope);
    }
    array_delete(&language->scopes);
    parser_delete(&language->parser);
}

void language_execute(Language* language) {
    parser_parse(&language->parser); // Created statements

    printf("\n\n");

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
            if (eval.type == Evaluation_type_absent)
                printf("Was trying to print absent value. \n");
            else
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
        //       it adds new variables to the last scope.
        //       there is always at least a single scope there.
        case Stmt_type_declaration: {
            // TODO: why do i have a token here if i create a string inside the primary var decalration thingy
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
                int x = 1; // This is here to be able to see if the if was succesfull and it entered the scope when debugging
            }
            else if (
                eval.type == Evaluation_type_integer && 
                strcmp(type_as_str.str, "int") == 0
            ) {
                int x = 1; // Same here
            }
            else if (
                eval.type == Evaluation_type_boolean && 
                strcmp(type_as_str.str, "bool") == 0
            ) {
                int x = 1; // Same here
            }
            else {
                printf("The specified type for variable %s and the type of the initialisation expression were not the same. \n", name_as_str.str);
                exit(1);
            }
            string_delete(&type_as_str);
            
            // Adding to the last scope
            int n_scopes = language->scopes.length;
            Language_scope* last_scope = ((Language_scope*) language->scopes.arr) + (n_scopes - 1);

            Tuple__string_evaluation new_var = {
                .eval = eval,
                .str  = name_as_str,
            };
            int err_code = language_scope_add_varaible(last_scope, &new_var);
            if (err_code == -1) {
                printf("Error: Redefenition of a variable '%s'.\n", name_as_str.str);
                printf(
                    "       (Row_start, Col_start)__(Row_end, Col_end): (%d, %d)__(%d, %d)",
                    name_token.start_row_n, name_token.start_col_n,
                    name_token.end_row_n, name_token.end_col_n
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

            // Adding to the last scope
            int n_scopes = language->scopes.length;
            Language_scope* last_scope = ((Language_scope*) language->scopes.arr) + (n_scopes - 1);

            Tuple__string_evaluation new_var = {
                .eval = eval,
                .str  = name_as_str,
            };
            int err_code = language_scope_add_varaible(last_scope, &new_var);
            if (err_code == -1) {
                printf("Error: Redefenition of a variable '%s'.\n", name_as_str.str);
                printf(
                    "       (Row_start, Col_start)__(Row_end, Col_end): (%d, %d)__(%d, %d)",
                    name_token.start_row_n, name_token.start_col_n,
                    name_token.end_row_n, name_token.end_col_n
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

        case Stmt_type_var_assignment: {
            Token var_name      = stmt->union_.var_assignment.var_name;
            Expr* expr          = stmt->union_.var_assignment.assigment_expr;
            Evaluation expr_val = language_evaluate_expression(language, expr);

            String var_name_as_str = string_init("");
            string_add_c_string(&var_name_as_str, var_name.lexeme, var_name.length);

            Evaluation* var_value = NULL;
            for (int i=0; i<language->scopes.length; ++i) {
                Language_scope* scope = ((Language_scope*) language->scopes.arr) + i;
                var_value = language_scope_get_value_for_varaible(scope, var_name_as_str);
                
                if (var_value != NULL) break; 
            }

            if (var_value != NULL) {
                if (var_value->type == expr_val.type) {
                    *var_value = expr_val;
                }
                else {
                    printf("Error: can't assign a value to varaible '%s', since its type is not the same as the type of the r_value expression. \n", var_name_as_str.str);
                    exit(1);
                }
            }
            else {
                printf("Error: trying to assign a value to a non-existant variable. \n");
                exit(1);
            }

            string_delete(&var_name_as_str);

            break;
        }

        // NOTE: this create new scopes.
        //       if handles scope creation.
        //       scopes are created here and added to language scopes arr at the last position.
        //       then when the nested statements are executed, they will be handles by their own handlers.
        case Stmt_type_scope: {
            Language_scope new_scope = {
                .variables = array_init(Array_type_tuple_string_evaluation),
            };
            array_add(&language->scopes, &new_scope, Array_type_language_scope);

            // Executing statements
            Array stmt_arr = stmt->union_.scope.statements;
            for (int i=0; i<stmt_arr.length; ++i) {
                Stmt* scope_nested_stmt = ((Stmt*) stmt_arr.arr) + i;
                language_execute_statement(language, scope_nested_stmt);
            }     

            // Removing new scope
            //  The new scope in the last one, so just freeing the memory for new_scope and lanaguge.arr.length - 1
            language_scope_delete(&new_scope);
            language->scopes.length -= 1;

            // Asserting
            {
                // TODO: maybe dont do it by hand
                if (language->scopes.length < 1) {
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

        case Stmt_type_for_loop: {
            // NOTE: since execution for Stmt_scope deletes the scope after execution,
            //       we would not be able to execute for loop scope more than once.
            //       sicne the logic here is different, we execute scope ourselves.

            // TODO: maybe find a way to execute witout code duplication.
            //       maybe splitting execution for each type if stmt into its own function might help
            //       then it would have an if in the end like: bool delete = true.

            // Plan: 1. Create a new scope for the language
            //       2. Manually add a varaible to that scope
            //       3. Add that scope as the last one
            //       4. Execute the for_loop.scope stmt manually until the range runs out
            //       5. Delete the scope for the iter_var 

            Stmt_for_loop for_loop = stmt->union_.for_loop;

            Token identifier       = for_loop.identifier;
            Expr* start_expr       = for_loop.range.start; // TODO: maybe check if start and end are not NULL here
            Expr* end_expr         = for_loop.range.end;   // TODO: maybe check if start and end are not NULL here
            Expr* increment_expr   = for_loop.range.increment;
            bool include_end_value = for_loop.range.include_end_value; 

            // 1.
            Language_scope manual_scope = {
                .variables = array_init(Array_type_tuple_string_evaluation),
            };

            // 2.
            
            int start, end, increment;

            Evaluation start_eval = language_evaluate_expression(language, start_expr);
            Evaluation end_eval   = language_evaluate_expression(language, end_expr);
            
            if (start_eval.type != Evaluation_type_integer) {
                printf("Error: Value used to start a for loop range was not of int type, only int types are supported. \n");
                exit(1);
            }
            else 
                start = start_eval.union_.integer; 

            if (end_eval.type != Evaluation_type_integer) {
                printf("Error: Value used to end a for loop range was not of int type, only int types are supported. \n");
                exit(1);
            }
            else 
                end = end_eval.union_.integer;

            if (increment_expr != NULL) {
                Evaluation increment_eval = language_evaluate_expression(language, increment_expr);
                if (increment_eval.type != Evaluation_type_integer) {
                    printf("Error: Value used to increment a for loop range was not of int type, only int types are supported. \n");
                    exit(1);
                }
                else 
                    increment = increment_eval.union_.integer;
            }
            else 
                increment = 1;

            String iter_var_as_str = string_init("");
            string_add_c_string(&iter_var_as_str, identifier.lexeme, identifier.length);

            Tuple__string_evaluation iter_var = {
                .str  = iter_var_as_str,
                .eval = (Evaluation) {
                    .type           = Evaluation_type_integer,
                    .union_.integer = start,
                } 
            };
            array_add(&manual_scope.variables, (void*) &iter_var, Array_type_tuple_string_evaluation);

            // 3.
            array_add(&language->scopes, &manual_scope, Array_type_language_scope);
            
            // 4.
            // Have to make an Stmt from for_loop.scope since its Stmt_scope and not Stmt
            Stmt for_loop_stmt = {
                .type         = Stmt_type_scope,
                .union_.scope = for_loop.scope,
            };
            // Manual scope execution
            Evaluation* iter_var_eval = language_scope_get_value_for_varaible(&manual_scope, iter_var_as_str);
            if (iter_var_eval == NULL) {
                printf("BACK_END_ERROR: Somehow the iter variable for the for loop was not existant in its own scope. \n");
                exit(1);
            } 
            
            if (iter_var_eval->type != Evaluation_type_integer) {
                printf("BACK_END_ERROR: Somehow the type for iter variable in the for loop was not of int type, only int is supported. \n");
                exit(1);
            }

            while (true) {
                if (include_end_value == false && iter_var_eval->union_.integer == end)
                    break;
                
                if (include_end_value == true && increment < 0 && iter_var_eval->union_.integer < end)
                    break;  
                
                if (include_end_value == true && increment > 0 && iter_var_eval->union_.integer > end)
                    break;

                // Execute while range is valid
                {
                    Language_scope new_scope = {
                        .variables = array_init(Array_type_tuple_string_evaluation),
                    };
                    array_add(&language->scopes, &new_scope, Array_type_language_scope);

                    // Executing statements
                    Array stmt_arr = for_loop.scope.statements;
                    for (int i=0; i<stmt_arr.length; ++i) {
                        Stmt* scope_nested_stmt = ((Stmt*) stmt_arr.arr) + i;
                        language_execute_statement(language, scope_nested_stmt);
                    }     

                    // Removing new scope
                    // The new scope is the last one, so just freeing the memory for new_scope and lanaguge.arr.length - 1
                    language_scope_delete(&new_scope);
                    language->scopes.length -= 1;

                    // Asserting
                    {
                        // TODO: maybe dont do it by hand
                        if (language->scopes.length < 1) {
                            printf("BACK_END_ERROR: somehow the lang object is left with no scopes. \n");
                            printf("BACK_END_ERROR: Called by \"language_execute_statement(Language* language, Stmt* stmt)\" \n");
                            exit(1);
                        }
                    } 
                }
                iter_var_eval->union_.integer += increment;
            }

            // 5.
            language_scope_delete(&manual_scope); // String gets delete here
            language->scopes.length -= 1;

            // Delete all the heap
            // string_delete(&iter_var_as_str);

            break;

        }

        case Stmt_type_while_loop: {
            // NOTE: since execution for Stmt_scope deletes the scope after execution,
            //       we would not be able to execute for loop scope more than once.
            //       sicne the logic here is different, we execute scope ourselves.

            // TODO: maybe find a way to execute witout code duplication.
            //       maybe splitting execution for each type if stmt into its own function might help
            //       then it would have an if in the end like: bool delete = true.

            // Plan: 1. Execute the while_loop.scope stmt manually until while_loop.condition is false

            // 1.
            Stmt_while_loop while_loop = stmt->union_.while_loop;
            Evaluation condition_eval  = language_evaluate_expression(language, while_loop.condition); 
            
            if (condition_eval.type != Evaluation_type_boolean) {
                printf("Error: Condition for a while loop must have a boolean value, other type was found. \n");
                exit(1);
            }

            while (language_evaluate_expression(language_execute, while_loop.condition).union_.boolean == true)
                language_execute_statement(language, while_loop.scope);

            break;
        }

        case Stmt_type_func_decl: {
            // Adding the function to the array of funcs in the last scope of the lang
            Language_scope* last_scope = ((Language_scope*) language->scopes.arr) + (language->scopes.length - 1);

            array_add(&last_scope->functions, &stmt->union_.func_decl, Array_type_stmt);

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
                for (int i=language->scopes.length - 1; i>=0; --i) {
                    Language_scope* scope = ((Language_scope*) language->scopes.arr) + i;
                    
                    Evaluation* eval = language_scope_get_value_for_varaible(scope, var_name);
                    if (eval != NULL) {
                        return *eval;
                    }
                }
                
                printf("Was not able to evaluate a varaible with name %s. Variable is not existant. \n", var_name.str);
                exit(1);
            }

            if (primary_type == Token_Type_Function_For_Parser) {
                Func_call func_call = expr->union_.primary.union_.func_call;

                String func_call_name_as_str = string_init(""); // TODO: delete later
                string_add_c_string(&func_call_name_as_str, func_call.name.lexeme, func_call.name.length);

                // Get a function with name from lang
                Stmt_func_decl* func_decl = NULL;
                for (int i=language->scopes.length - 1; i>=0; --i) {
                    Language_scope* scope = ((Language_scope*) language->scopes.arr) + i;
                    
                    func_decl = language_scope_get_func_decl_for_name(scope, func_call_name_as_str);
                    
                    if (func_decl != NULL)  
                        break;
                    // Get a function with name from the scope
                }
                if (func_decl == NULL) {
                    printf("Was not able to evaluate a function call, casue the dunction called '%s' doesnt exist. \n", func_call_name_as_str.str);
                    exit(1);
                }

                // Working with the declaration
                // Plan: 1. Check if all the argumets comform to their specified types

                // TODO: make the scope have 2 list for arg names and second for arg types

                // 1.
                Array arg_names_as_tokens = func_decl->arg_names_as_tokens;
                Array arg_types_as_tokens = func_decl->arg_types_as_tokens;
                
                Evaluation* args_evals = malloc(sizeof(Evaluation) * func_call.args_as_expr_p.length);
                if (args_evals == NULL) {
                    printf("Not enought memory. \n");
                    exit(1);
                }

                if (arg_names_as_tokens.length != arg_types_as_tokens.length && arg_names_as_tokens.length != func_call.args_as_expr_p.length) 
                    exit(1);

                for (int i=0; i<arg_names_as_tokens.length; ++i) {
                    Expr* arg_expr = ((Expr*) func_call.args_as_expr_p.arr) + i;
                    Evaluation arg_eval = language_evaluate_expression(language, arg_expr);
                    args_evals[i] = arg_eval;

                    Token* arp_type_token = ((Token*) arg_types_as_tokens.arr) + i;

                    // Checking types here
                    if (arg_eval.type == Evaluation_type_integer && arp_type_token->type == Token_Type_Int_Type) {

                    }
                    else if (arg_eval.type == Evaluation_type_boolean && arp_type_token->type == Token_Type_Bool_type) {

                    }
                    else {
                        printf("Function passed in value dpnt confor to the function frclaration arg types. \n");
                        exit(1);
                    }

                }
                
                // Now that we cheked the types, 
                //  we create a new scope 
                //  and then will add varaibles and statements to it and them manuallly execute it

                // New scope 
                Language_scope new_scope = {
                    .functions = array_init(Array_type_stmt_func_decl),
                    .variables = array_init(Array_type_tuple_string_evaluation),
                };

                // Setting up the scope --> adding variables
                for (int i=0; i<arg_names_as_tokens.length; ++i) {
                    Token*     var_name = ((Token*) arg_names_as_tokens.arr) + i;
                    Evaluation var_eval = args_evals[i];

                    String var_name_as_str = string_init("");
                    string_add_c_string(&var_assignment, var_name->lexeme, var_name->length);
                    
                    Tuple__string_evaluation new_var = {.str = var_name_as_str, .eval = var_eval};
                    array_add(&new_scope.variables, &new_var, Array_type_tuple_string_evaluation);
                }

                array_add(&language->scopes, &new_scope, Array_type_language_scope); 
                
                // Executin the function block
                for (int i=0; i<func_decl->scope.statements.length; ++i) {
                    Stmt* stmt = ((Stmt*) func_decl->scope.statements.arr) + i;
                    language_execute_statement(language, stmt);
                }

                // Clearing out the new scope created for this function
                language_scope_delete(&new_scope);
                language->scopes.length -= 1;

                return (Evaluation) {.type = Evaluation_type_absent, .union_.integer = 0};

                break;



                // If all the types are fine, we create varaible and store them
                // Execute the scope manuallt again, add varaibles to it, and also execute it, then delete the scope




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
            Token_Type operator = expr->union_.binary.operator.type;
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
                case Token_Type_Equals_Equals: {
                    if (
                        left.type  == Evaluation_type_boolean &&
                        right.type == Evaluation_type_boolean
                    ) {
                        return (Evaluation) {
                            .type           = Evaluation_type_boolean,
                            .union_.boolean = (left.union_.boolean == right.union_.boolean),
                        };
                    }

                    if (
                        left.type  == Evaluation_type_integer &&
                        right.type == Evaluation_type_integer
                    ) {
                        return (Evaluation) {
                            .type           = Evaluation_type_boolean,
                            .union_.boolean = (left.union_.integer == right.union_.integer),
                        };
                    }

                    printf("Error, unsupported operator '==' for types. \n");
                    exit(1);

                    break;
                }
                case Token_Type_Not_Equals: {
                    if (
                        left.type  == Evaluation_type_boolean &&
                        right.type == Evaluation_type_boolean
                    ) {
                        return (Evaluation) {
                            .type           = Evaluation_type_boolean,
                            .union_.boolean = (left.union_.boolean != right.union_.boolean),
                        };
                    }

                    if (
                        left.type  == Evaluation_type_integer &&
                        right.type == Evaluation_type_integer
                    ) {
                        return (Evaluation) {
                            .type           = Evaluation_type_boolean,
                            .union_.boolean = (left.union_.integer != right.union_.integer),
                        };
                    }

                    printf("Error, unsupported operator '!=' for types. \n");
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
















