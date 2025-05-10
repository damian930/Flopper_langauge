#include <stdlib.h>
#include "stdio.h"
#include "Ast.h"
#include "Array_Stmt.h"


Array_stmt array_stmt_init() {
    int length   = 0;
    int capacity = 2;
    
    Stmt* arr    = malloc(sizeof(Stmt) * capacity);
    if (arr == NULL) {
        printf("Was not able to initialise memory for a statement. \n");
        exit(1);
    }

    return (Array_stmt) {
        .arr      = arr,
        .length   = length,
        .capacity = capacity,
    };
}

void array_stmt_delete(Array_stmt* arr) {
    free(arr->arr); // TODO: check if this can fail
    arr->arr      = NULL;
    arr->capacity = 0;
    arr->length   = 0;
}

void array_stmt_add(Array_stmt* arr, Stmt stmt) {
    if (arr->length == arr->capacity) {
        int new_cap   = arr->capacity * 2;
        Stmt* new_mem = realloc(arr->arr, sizeof(Stmt) * new_cap);
        if (new_mem  == NULL) {
            printf("Was not able to initialise memory for a statement. \n");
            exit(1);
        }
        arr->arr      = new_mem;
        arr->capacity = new_cap;
    }

    arr->arr[arr->length] = stmt;
    arr->length += 1;
}





























