#pragma once
#include "Parser.h"
#include "Array.h"
#include "my_String.h"

// ================================================================================
typedef struct Variable Variable;
typedef struct Tuple__hash_variables Tuple__hash_variables;
typedef struct Map_variables Map_variables;

struct Variable {
    String     name;
    Evaluation value;
};

struct Tuple__hash_variables {
    int hash;
    Array variables;
};

struct Map_variables {
    Array hash_variable_tuples;
};
Map_variables map_variables_init();
void map_variables_delete(Map_variables* map);

int       map_variables_hash(String name);
int       map_variables_add (Map_variables* map, String name, Evaluation value);
Variable* map_variables_get (Map_variables* map, String name);


// ================================================================================
typedef struct Language Language;

struct Language {
    Parser parser;
    Array  variable_scopes_arr; // NOTE: this is an arry of Map_variables
};
Language language_init   (const char* text);
void     language_delete (Language* language);
void     language_execute(Language* language);

Evaluation language_evaluate_expression(Language* language, Expr* expr);
void       language_execute_statement  (Language* language, Stmt* stmt);







