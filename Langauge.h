#pragma once
#include "Parser.h"
#include "Array.h"
#include "my_String.h"

typedef struct Tuple__string_evaluation Tuple__string_evaluation;
typedef struct Language_scope Language_scope;

struct Tuple__string_evaluation {
    String     str;
    Evaluation eval;
};
void tuple__string_evaluation_delete(Tuple__string_evaluation* tuple);


struct Language_scope {
    Array variables; // Array of Tuple__string_evaluations
    Array functions; // Array of Stmt_func_decl represented as Stmt_func_decl
};
// Also with this i will e able to then using enum specify what purpose i am addit it for
Evaluation*     language_scope_get_value_for_varaible(Language_scope* scope, String name);
Stmt_func_decl* language_scope_get_func_decl_for_name(Language_scope* scope, String name);
int             language_scope_add_varaible(Language_scope* scope, Tuple__string_evaluation* new_var);
void            language_scope_delete(Language_scope* scope);


// ================================================================================
typedef struct Language Language;

struct Language {
    Parser parser;
    Array  scopes; // Array of Language_scopes
    // Array  variable_scopes_arr; // NOTE: this is an arry of Map_variables
};
Language language_init   (const char* text);
void     language_delete (Language* language);
void     language_execute(Language* language);

Evaluation language_evaluate_expression(Language* language, Expr* expr);
void       language_execute_statement  (Language* language, Stmt* stmt);







