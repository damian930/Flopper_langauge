#pragma once
#include "stdio.h"
#include <stdlib.h>

struct Stmt;
typedef struct Stmt       Stmt;
typedef struct Array_stmt Array_stmt;

struct Array_stmt {
    Stmt* arr;
    int   length;
    int   capacity;
};

Array_stmt array_stmt_init();
void array_stmt_delete(Array_stmt* arr);

void array_stmt_add(Array_stmt* arr, Stmt stmt);























