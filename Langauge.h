#pragma once
#include "Parser.h"
#include "Array.h"
#include "my_String.h"



typedef struct Tuple__token_evaluation  Tuple__token_evaluation;
typedef struct Language_scope           Language_scope;

Array_create_type (Tuple__token_evaluation, Token_evaluation_array);
Array_create_type (Stmt_func_decl, Stmt_func_decl_array);
Array_create_type (Language_scope, Language_scope_array);


struct Tuple__token_evaluation {
    Token      token;
    Evaluation eval;
};


struct Language_scope {
    Token_evaluation_array token_eval_arr;
    Stmt_func_decl_array   functions;                        // Array of Stmt_func_decl represented as Stmt_func_decl
};
Evaluation*     language_scope_get_value_for_varaible(Language_scope* scope, Token name);
//Stmt_func_decl* language_scope_get_func_decl_for_name(Language_scope* scope, Token name);
int             language_scope_add_varaible(Language_scope* scope, Tuple__token_evaluation new_var);
void            language_scope_delete(Language_scope* scope);


// ================================================================================
typedef struct Language Language;

struct Language {
    Parser parser;
    Language_scope_array scopes_arr; // Array of Language_scopes
    // Array  variable_scopes_arr; // NOTE: this is an arry of Map_variables
};
Language language_init   (const char* text);
void     language_delete (Language* language);
void     language_execute(Language* language);

Evaluation language_evaluate_expression(Language* language, Expr* expr);
void       language_execute_statement  (Language* language, Stmt* stmt);

void language_execute_stmt_expr      (Language* language, Stmt_expr*           stmt);
void language_execute_stmt_print     (Language* language, Stmt_print*          stmt);
void language_execute_stmt_decl      (Language* language, Stmt_var_decl*       stmt);
void language_execute_stmt_decl_auto (Language* language, Stmt_var_decl_auto*  stmt);
void language_execute_stmt_assignment(Language* language, Stmt_var_assignment* stmt);
void language_execute_stmt_scope     (Language* language, Stmt_scope*          stmt);
void language_execute_stmt_if        (Language* language, Stmt_if*             stmt);
void language_execute_stmt_for_loop  (Language* language, Stmt_for_loop*       stmt);
void language_execute_stmt_while_loop(Language* language, Stmt_while_loop* stmt);

Evaluation language_evaluate_expr_primary(Language* language, Primary* epxr);
Evaluation language_evaluate_expr_unary  (Language* language, Unary*   epxr);
Evaluation language_evaluate_expr_binary (Language* language, Binary*  epxr);












