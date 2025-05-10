#pragma once
#include "stdio.h"
#include "stdlib.h"

typedef enum   Array_type Array_type;
typedef struct Array      Array;

enum Array_type {
    Array_type_int,
    Array_type_stmt,
    Array_type_tuple__hash_variable,
    Array_type_variable,
};
int array_get_size_of_arr_type(Array_type type);

struct Array {  
    Array_type type;
    void* arr;
    int   length;
    int   capacity;
};

Array array_init(Array_type type);
void array_delete(Array* arr);

void array_add(Array* arr, void* value, Array_type value_type);



// TODO: 
// void array_print(Array* arr, )


// == Debugging ==============================================================

// Array arr = array_init(Array_type_int);
// 	for (int i=0; i<=20; i++) {
// 		array_add(&arr, (void*) &i, Array_type_int);
// 	}

// 	for (int i=0; i<=20; i++) {
// 		printf("-- %d --", ((int*)arr.arr)[i]);
// 	}


































