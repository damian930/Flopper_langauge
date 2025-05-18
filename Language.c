#include "Langauge.h"
#include "string.h"
 
Array_init        (Tuple__token_evaluation, Token_evaluation_array, token_evaluation_array_init);
Array_delete      (Token_evaluation_array, token_evaluation_array_delete);
Array_add_by_value(Tuple__token_evaluation, Token_evaluation_array, token_evaluation_array_add_by_value);

Array_init        (Stmt_func_decl, Stmt_func_decl_array, stmt_func_decl_array_init);
Array_delete      (Stmt_func_decl_array, stmt_func_decl_array_delete);
Array_add_by_value(Stmt_func_decl, Stmt_func_decl_array, stmt_func_decl_array_add_by_value);

Array_init        (Language_scope, Language_scope_array, language_scope_array_init);
Array_delete      (Language_scope_array, language_scope_array_delete);
Array_add_by_value(Language_scope, Language_scope_array, language_scope_array_add_by_value);

// ============ Scope 
Evaluation* language_scope_get_value_for_varaible(Language_scope* scope, Token name) {
    for (int i=0; i<scope->token_eval_arr.length; ++i) {
        Token other_name = scope->token_eval_arr.data[i].token;
        bool are_equal = c_string_equal(name.lexeme, name.length, other_name.lexeme, other_name.length);
        if (are_equal) {
            return &scope->token_eval_arr.data[i].eval;
        }
    }
    return NULL;
}

Stmt_func_decl* language_scope_get_func_decl_for_name(Language_scope* scope, Token name) {
    Stmt_func_decl* funcs = scope->functions.data;   

    for (int i=0; i<scope->functions.length; ++i) {
        Token other_name = funcs->name;
        bool are_equal = c_string_equal(name.lexeme, name.length, funcs->name.lexeme, funcs->name.length);
        if (are_equal) {
            return (funcs + i);
        }
    }

    return NULL;
}

int language_scope_add_varaible(Language_scope* scope, Tuple__token_evaluation new_var) {
    // Checking if the array already has a variable with this name
    if (language_scope_get_value_for_varaible(scope, new_var.token) != NULL) {
        return -1;
    }

    // No redeclaration, success
    token_evaluation_array_add_by_value(&scope->token_eval_arr, new_var);

    return 0;
}

int language_scope_add_func_decl(Language_scope* scope, Stmt_func_decl func_decl) {
    // Checking if the array already has a decl with this name
    if (language_scope_get_func_decl_for_name(scope, func_decl.name) != NULL) {
        return -1;
    }

    // No redeclaration, success
    stmt_func_decl_array_add_by_value(&scope->functions, func_decl);

    return 0;
}

void language_scope_delete(Language_scope* scope) {
    token_evaluation_array_delete(&scope->token_eval_arr);
}


// ================================================================================
// == Language

Language language_init(const char* text) {
    Language language = {
        .parser     = parser_init(text),
        .scopes_arr = language_scope_array_init(),
    };

    // NOTE: providing the language environment with a default scope 
    Language_scope default_scope = {
        .token_eval_arr = token_evaluation_array_init(),
        .functions      = stmt_func_decl_array_init(),
    };
    language_scope_array_add_by_value(&language.scopes_arr, default_scope);

    return language;
}

void language_delete(Language* language) {
    for (int i=0; i<language->scopes_arr.length; ++i) {
        Language_scope* scope = language->scopes_arr.data + i;
        language_scope_delete(scope);
    }
    language_scope_array_delete(&language->scopes_arr);
    parser_delete(&language->parser);
}

void language_execute(Language* language) {
    parser_parse(&language->parser); // Created statements

    printf("\n\n");

    for (int i=0; i<language->parser.stmt_arr.length; ++i) {
        Stmt* stmt = language->parser.stmt_arr.data + i;
        language_execute_statement(language, stmt);
    }

}





void language_execute_statement(Language* language, Stmt* stmt) {
    switch (stmt->type) {
        case Stmt_type_expr: {
            language_execute_stmt_expr(language, &stmt->union_.stmt_expr);
            break;
        }

        case Stmt_type_print: {
            language_execute_stmt_print(language, &stmt->union_.print);
            break;
        }

        // NOTE: this handles var creating. 
        //       it adds new variables to the last scope.
        //       there is always at least a single scope there.
        case Stmt_type_declaration: {
            language_execute_stmt_decl(language, &stmt->union_.var_decl);
            break;
        }

        case Stmt_type_declaration_auto: {
            language_execute_stmt_decl_auto(language, &stmt->union_.var_decl_auto);
            break;
        }

        case Stmt_type_var_assignment: {
            language_execute_stmt_assignment(language, &stmt->union_.var_assignment);
            break;
        }

        // NOTE: this create new scopes.
        //       if handles scope creation.
        //       scopes are created here and added to language scopes arr at the last position.
        //       then when the nested statements are executed, they will be handles by their own handlers.
        case Stmt_type_scope: {
            language_execute_stmt_scope(language, &stmt->union_.scope);
            break;

        }

        case Stmt_type_if: {
            language_execute_stmt_if(language, &stmt->union_.if_else);
            break;
        }

        case Stmt_type_for_loop: {
            language_execute_stmt_for_loop(language, &stmt->union_.for_loop);
            break;
        }

        case Stmt_type_while_loop: {
            language_execute_stmt_while_loop(language, &stmt->union_.while_loop);
            break;
        }

        case Stmt_type_func_decl: {
            // TODO: move this isnot its own function
            
            Language_scope* last_scope = language->scopes_arr.data + (language->scopes_arr.length - 1);
            int err_code = language_scope_add_func_decl(last_scope, stmt->union_.func_decl);

            if (err_code == -1) {
                printf("Error: function redefenition. \n");
                exit(1);
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

void language_execute_stmt_expr(Language* language, Stmt_expr* stmt) {
    Evaluation eval = language_evaluate_expression(language, stmt->expr);
    evaluation_print(&eval);
}

void language_execute_stmt_print(Language* language, Stmt_print* stmt) {
    Evaluation eval = language_evaluate_expression(language, stmt->expr);
    evaluation_print(&eval);
    printf("\n");
}

void language_execute_stmt_decl(Language* language, Stmt_var_decl* stmt) {
    Token name_token           = stmt->var_name;
    Token specified_type_token = stmt->var_specified_type;
    
    Evaluation eval;
    if (stmt->init_expr == NULL) { // No assigment
        eval = (Evaluation) {.type = Evaluation_type_absent, .union_.integer = 0}; /* the = 0 is a palceholder*/
    }   
    else {
        eval = language_evaluate_expression(language, stmt->init_expr);
    }

    // Checking if the specified type is the same type as the type of the init value expression
    if (eval.type == Evaluation_type_absent) {
        // When its absent, then its just not initialised, but the specified type stays
        int x = 1; // This is here to be able to see if the if was succesfull and it entered the scope when debugging
    }
    else if (
        eval.type == Evaluation_type_integer && 
        c_string_equal(specified_type_token.lexeme, specified_type_token.length, "int", 3)
    ) {
        int x = 1; // Same here
    }
    else if (
        eval.type == Evaluation_type_boolean && 
        c_string_equal(specified_type_token.lexeme, specified_type_token.length, "bool", 4)
    ) {
        int x = 1; // Same here
    }
    else {
        // Need better printing of variable name here
        printf("The specified type for variable and the type of the initialisation expression were not the same. \n");
        exit(1);
    }
    
    // Adding to the last scope
    int n_scopes = language->scopes_arr.length;
    Language_scope* last_scope = language->scopes_arr.data + (language->scopes_arr.length - 1);

    Tuple__token_evaluation new_var = {
        .token = name_token,
        .eval  = eval,
    };
    int err_code = language_scope_add_varaible(last_scope, new_var);
    if (err_code == -1) {

        // Need better error printing here
        printf("Error: Redefenition of a variable. \n");
        printf(
            "       (Row_start, Col_start)__(Row_end, Col_end): (%d, %d)__(%d, %d)",
            name_token.start_row_n, name_token.start_col_n,
            name_token.end_row_n, name_token.end_col_n
        );
        exit(1);
    }

    printf("Declared ("); 
    // printf("%s", name_as_str.str); // Need better debug here 
    // printf(": "); 
    if(eval.type == Evaluation_type_integer) printf("int");
    else if(eval.type == Evaluation_type_boolean) printf("bool");
    else if(eval.type == Evaluation_type_absent)  printf("Un Initialised");
    else printf("Unsupported eval type for printing");
    printf(")\n");
}

void language_execute_stmt_decl_auto(Language* language, Stmt_var_decl_auto* stmt) {
    Token name_token  = stmt->var_name;

    Evaluation eval;
    if (stmt->init_expr == NULL) { // No assigment
        printf("FEEL LIKE THIS SHOULD NEVER BE CALLED");
        printf("Error: varaible creationg using ':=' has to have a right value for initialisation. \n");
        printf("       to declare a varaible with no value, use a type specifier like: \n");
        //printf("       %s: <type> \n", name_as_str.str);
        exit(1);
    }   
    else {
        eval = language_evaluate_expression(language, stmt->init_expr);
    }

    // Adding to the last scope
    Language_scope* last_scope = language->scopes_arr.data + (language->scopes_arr.length - 1);

    Tuple__token_evaluation new_var = {
        .token = name_token,
        .eval  = eval,
    };
    int err_code = language_scope_add_varaible(last_scope, new_var);
    if (err_code == -1) {
        printf("Error: Redefenition of a variable. \n"); // Need better error here
        printf(
            "       (Row_start, Col_start)__(Row_end, Col_end): (%d, %d)__(%d, %d)",
            name_token.start_row_n, name_token.start_col_n,
            name_token.end_row_n, name_token.end_col_n
        );
        exit(1);
    }

    printf("Declared ("); 
    // printf("%s", name_as_str.str);
    printf(": "); 
    if(eval.type == Evaluation_type_integer) printf("int");
    else if(eval.type == Evaluation_type_boolean) printf("bool");
    else if(eval.type == Evaluation_type_absent)  printf("Un Initialised");
    else printf("Unsupported eval type for printing");
    printf(")\n");
}

void language_execute_stmt_assignment(Language* language, Stmt_var_assignment* stmt) {
    Token var_name      = stmt->var_name;
    Expr* expr          = stmt->assigment_expr;
    Evaluation expr_val = language_evaluate_expression(language, expr);

    Evaluation* var_value = NULL;
    for (int i=0; i<language->scopes_arr.length; ++i) {
        Language_scope* scope = language->scopes_arr.data + i;
        var_value = language_scope_get_value_for_varaible(scope, var_name);
        
        if (var_value != NULL) break; 
    }

    if (var_value != NULL) {
        if (var_value->type == expr_val.type) {
            *var_value = expr_val;
        }
        else {
            printf("Error: cant asssign a value");
            // printf("Error: can't assign a value to varaible '%s', since its type is not the same as the type of the r_value expression. \n", var_name_as_str.str);
            exit(1);
        }
    }
    else {
        printf("Error: trying to assign a value to a non-existant variable. \n");
        exit(1);
    }
}

void language_execute_stmt_scope(Language* language, Stmt_scope* stmt) {
    Language_scope new_scope = {
        .token_eval_arr = token_evaluation_array_init(),
    };
    language_scope_array_add_by_value(&language->scopes_arr, new_scope);

    // Executing statements
    Stmt_array stmt_arr = stmt->statements;
    for (int i=0; i<stmt_arr.length; ++i) {
        Stmt* scope_nested_stmt = stmt_arr.data + i;
        language_execute_statement(language, scope_nested_stmt);
    }     

    // Removing new scope
    //  The new scope in the last one, so just freeing the memory for new_scope and lanaguge.arr.length - 1
    language_scope_delete(&new_scope);
    language->scopes_arr.length -= 1;

    // Asserting
    {
        // TODO: maybe dont do it by hand
        if (language->scopes_arr.length < 1) {
            printf("BACK_END_ERROR: somehow the lang object is left with no scopes. \n");
            printf("BACK_END_ERROR: Called by \"language_execute_statement(Language* language, Stmt* stmt)\" \n");
            exit(1);
        }
    }

}

void language_execute_stmt_if(Language* language, Stmt_if* stmt) {
    bool executed_if_stmt = false; 
    Evaluation main_expr_eval = language_evaluate_expression(language, stmt->main_if_expr);
    if (main_expr_eval.type == Evaluation_type_boolean) {
        if (main_expr_eval.union_.boolean == true) {
            Stmt scope_as_stmt = stmt_scope_to_stmt(&stmt->main_if_scope);
            language_execute_statement(language, &scope_as_stmt);
            executed_if_stmt = true;
        }
    }
    else {
        printf("Was not able to evaluate a condition for and if statement, the value of the condition is not of a boolean type. \n");
        exit(1);
    }

    if (!executed_if_stmt) {
        for (int i=0; i<stmt->expr_scope_arr.length; ++i) {
            Tuple__expr_scope* tuple = stmt->expr_scope_arr.data + i;
            
            Evaluation else_if_expr_eval = language_evaluate_expression(language, tuple->expr);
            if (else_if_expr_eval.type == Evaluation_type_boolean) {
                if (else_if_expr_eval.union_.boolean == true) {
                    Stmt scope_as_stmt = stmt_scope_to_stmt(&tuple->scope);
                    language_execute_statement(language, &scope_as_stmt);
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
        Stmt scope_as_stmt = stmt_scope_to_stmt(&stmt->else_scope);
        language_execute_statement(language, &scope_as_stmt);
    }
}

void language_execute_stmt_for_loop(Language* language, Stmt_for_loop* stmt) {
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

    Stmt_for_loop for_loop = *stmt;

    Token identifier       = for_loop.identifier;
    Expr* start_expr       = for_loop.range.start; // TODO: maybe check if start and end are not NULL here
    Expr* end_expr         = for_loop.range.end;   // TODO: maybe check if start and end are not NULL here
    Expr* increment_expr   = for_loop.range.increment;
    bool include_end_value = for_loop.range.include_end_value; 

    // 1.
    Language_scope manual_scope = {
        .token_eval_arr = token_evaluation_array_init(),
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

    Tuple__token_evaluation iter_var = {
        .token = identifier,
        .eval  = (Evaluation) {
            .type           = Evaluation_type_integer,
            .union_.integer = start,
        } 
    };
    token_evaluation_array_add_by_value(&manual_scope.token_eval_arr, iter_var);

    // 3.
    language_scope_array_add_by_value(&language->scopes_arr, manual_scope);
    
    // 4.
    // Have to make an Stmt from for_loop.scope since its Stmt_scope and not Stmt
    Stmt for_loop_stmt = {
        .type         = Stmt_type_scope,
        .union_.scope = for_loop.scope,
    };
    // Manual scope execution
    Evaluation* iter_var_eval = language_scope_get_value_for_varaible(&manual_scope, identifier);
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
                .token_eval_arr = token_evaluation_array_init(),
            };
            language_scope_array_add_by_value(&language->scopes_arr, new_scope);
            
            // Executing statements
            for (int i=0; i<for_loop.scope.statements.length; ++i) {
                Stmt* scope_nested_stmt = for_loop.scope.statements.data + i;
                language_execute_statement(language, scope_nested_stmt);
            }     

            // Removing new scope
            // The new scope is the last one, so just freeing the memory for new_scope and lanaguge.arr.length - 1
            language_scope_delete(&new_scope);
            language->scopes_arr.length -= 1;

            // Asserting
            {
                // TODO: maybe dont do it by hand
                if (language->scopes_arr.length < 1) {
                    printf("BACK_END_ERROR: somehow the lang object is left with no scopes. \n");
                    printf("BACK_END_ERROR: Called by \"language_execute_statement(Language* language, Stmt* stmt)\" \n");
                    exit(1);
                }
            } 
        }
        iter_var_eval->union_.integer += increment;
    }

    // 5.
    language_scope_delete(&manual_scope); 
    language->scopes_arr.length -= 1;
}

void language_execute_stmt_while_loop(Language* language, Stmt_while_loop* stmt) {
    // NOTE: since execution for Stmt_scope deletes the scope after execution,
    //       we would not be able to execute for loop scope more than once.
    //       sicne the logic here is different, we execute scope ourselves.

    // TODO: maybe find a way to execute witout code duplication.
    //       maybe splitting execution for each type if stmt into its own function might help
    //       then it would have an if in the end like: bool delete = true.

    // Plan: 1. Execute the while_loop.scope stmt manually until while_loop.condition is false

    // 1.
    Evaluation condition_eval  = language_evaluate_expression(language, stmt->condition); 
    
    if (condition_eval.type != Evaluation_type_boolean) {
        printf("Error: Condition for a while loop must have a boolean value, other type was found. \n");
        exit(1);
    }

    while (language_evaluate_expression(language_execute, stmt->condition).union_.boolean == true) {
        Stmt scope_as_stmt = stmt_scope_to_stmt(&stmt->scope);
        language_execute_statement(language, &scope_as_stmt);
    }

}



// Given an expression, manually evaluates it
Evaluation language_evaluate_expression(Language* language, Expr* expr) {
    switch (expr->type) {
        case Expr_type_primary: {
            return language_evaluate_expr_primary(language, &expr->union_.primary);
        }

        case Expr_type_unary: {
            return language_evaluate_expr_unary(language, &expr->union_.unary);
        }

        case Expr_type_binary: {
            return language_evaluate_expr_binary(language, &expr->union_.binary);
        }

        default: {
            printf("Was not able to evauate expr, expr type unsupported. \n");
            exit(1);
        }
    }
}

Evaluation language_evaluate_expr_primary(Language* language, Primary* expr) {
    Token_Type primary_type = expr->type;
    
    if (primary_type == Token_Type_True || primary_type == Token_Type_False) {
        return (Evaluation) {
            .type           = Evaluation_type_boolean,
            .union_.boolean = expr->union_.boolean,
        };
    }

    else if (primary_type == Token_Type_Integer) {
        return (Evaluation) {
            .type           = Evaluation_type_integer,
            .union_.integer = expr->union_.integer,
        };
    }

    else if (primary_type == Token_Type_Identifier) {
        Token var_name = expr->union_.identifier;

        // 1. Check the newest scope. If not then check the one before that. And so on
        for (int i=language->scopes_arr.length - 1; i>=0; --i) {
            
            Language_scope* scope = language->scopes_arr.data + i;
            
            Evaluation* eval = language_scope_get_value_for_varaible(scope, var_name);
            if (eval != NULL) {
                return *eval;
            }
        }
        
        printf("Was not able to evaluate a varaible with name . Variable is not existant. \n");
        exit(1);
    }

    else if (primary_type == Token_Type_Function_Call_For_Parser) {
        Func_call func_call = expr->union_.func_call;
        
        // Getting the func declaration for passed in func name
        Stmt_func_decl* func_decl = NULL;
        for (int i= language->scopes_arr.length - 1; i>=0; ++i) {
            Language_scope* scope = language->scopes_arr.data + i;
            
            func_decl = language_scope_get_func_decl_for_name(scope, func_call.name);
            if (func_decl != NULL) break;
        }
        if (func_decl == NULL) {
            printf("Function with name was not found in any scope. \n");
            exit(1);
        }

        if (func_decl->arg_names_as_tokens.length != func_call.args_as_expr_p.length) {
            printf("Invalid number of passed arguments to a function. \n");
            exit(1);
        }

        // Evaluating expression passed as arguments
        Evaluation* args_evals = malloc(sizeof(Evaluation) * func_call.args_as_expr_p.length);
        if (args_evals == NULL) {
            printf("Was not able to initialise memory for malloc. \n");
            exit(1);
        } 

        for (int i=0; i<func_call.args_as_expr_p.length; ++i) {
            Expr* arg_expr = func_call.args_as_expr_p.data[i];
            args_evals[i] = language_evaluate_expression(language, arg_expr);
        }

        for (int i=0; i<func_call.args_as_expr_p.length; ++i) {
            Token type_identifier = func_decl->arg_types_as_tokens.data[i];
            Evaluation test = args_evals[i];

            if (
                args_evals[i].type == Evaluation_type_integer &&
                c_string_equal(type_identifier.lexeme, type_identifier.length, "int", 3)
            ) {
                int x = 1; // For debbuger
            }

            else if (
                args_evals[i].type == Evaluation_type_boolean &&
                c_string_equal(type_identifier.lexeme, type_identifier.length, "bool", 4)
            ) {
                int x = 1; // For debbuger
            }
            else {
                printf("Type mismatch for a function. \n");
                exit(1);
            }
        }

        // NOTE: since i cant have defaut values for arguments in c, i cant do overrides for types of block execution i need.
        //       for that i exeute block logic manually pretty much everywhere.
        //       here i need to create a scope for function, add arg variables to it, execute, delete the scope
        
        Language_scope func_exe_scope = {
            .token_eval_arr = token_evaluation_array_init(),
            .functions      = stmt_func_decl_array_init(),
        };

        for (int i=0; i<func_decl->arg_names_as_tokens.length; ++i) {
            Tuple__token_evaluation variable = {
                .token = func_decl->arg_names_as_tokens.data[i],
                .eval  = args_evals[i],
            };

            token_evaluation_array_add_by_value(&func_exe_scope.token_eval_arr, variable);
        }

        for (int i = 0; i < func_exe_scope.token_eval_arr.length; ++i) {
            token_print(&func_exe_scope.token_eval_arr.data[i].token);
        }

        language_scope_array_add_by_value(&language->scopes_arr, func_exe_scope);
        
        for (int i=0; i<func_decl->scope.statements.length; ++i) {
            Stmt* stmt_to_execute = func_decl->scope.statements.data + i;
            language_execute_statement(language, stmt_to_execute);
        }

        language_scope_delete(&func_exe_scope);
        language->scopes_arr.length -= 1; 

        return (Evaluation) { .type = Evaluation_type_absent, .union_.integer = 0 };
    }

    printf("Unsupported primary evaluation. \n");
    exit(1);
}

Evaluation language_evaluate_expr_unary(Language* language, Unary* expr) {
    Token_Type operator = expr->operator.type;
    Evaluation right    = language_evaluate_expression(language, expr->right);

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
                    token_print(&expr->operator);
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
                    token_print(&expr->operator);
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
            token_print(&expr->operator);
            exit(1);
        }
    }

}

Evaluation language_evaluate_expr_binary(Language* language, Binary* expr) {
    Token_Type operator = expr->operator.type;
    Evaluation left     = language_evaluate_expression(language, expr->left);
    Evaluation right    = language_evaluate_expression(language, expr->right);

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
            token_print(&expr->operator);
            exit(1);
        }

        // TODO: other bin operators
    }

}















