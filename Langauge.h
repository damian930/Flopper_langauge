#pragma once
#include "Ast.h"
#include "Array.h"
#include "my_String.h"

// ========================================================================================
// List of pointers, each pointer point to a Tuple (int hash, Variable*)
// arr inside the tuple is a pointer to an array of variables, 
// each variable is a String name and as Evaluation value

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

int map_variables_hash(String name);
void map_variables_add(Map_variables* map, String name, Evaluation value);
Variable map_variables_get(Map_variables* map, String name);


// ========================================================================================


typedef struct Language Language;

struct Language {
    Parser parser;
    // Variables
    
};









