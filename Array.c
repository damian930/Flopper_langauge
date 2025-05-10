#include "Array.h"
#include "Ast.h"

int array_get_size_of_arr_type(Array_type type) {
    switch (type) {
        case Array_type_int : { return sizeof(int);  }
        case Array_type_stmt: { return sizeof(Stmt); }

        default: {
            printf("Was not able to calculate the size of Array_type. Unsupported type. \n");
            exit(1);
        }
    }
}

Array array_init(Array_type type) {
    int length            = 0;
    int capacity          = 20;
    int size_of_arr_value = array_get_size_of_arr_type(type); 

    void* arr = malloc(capacity * size_of_arr_value);
    if (arr == NULL) {
        printf("Was not able to intialise memeory for Array. \n");
        exit(1);
    } 
    
    return (Array) {
        .type     = type,
        .arr      = arr,
        .length   = length,
        .capacity = capacity,
    };
}

void array_delete(Array* arr) {
    free(arr->arr);
    arr->arr      = NULL;
    arr->length   = 0;
    arr->capacity = 0;
}


void array_add(Array* arr, void* new_value, Array_type value_type) {
    if (arr->type != value_type) {
        printf("Was not able to add a value to array. Mismatch in types. \n");
        exit(1);
    }
    
    if (arr->length == arr->capacity) {
        int   size_of_arr_value = array_get_size_of_arr_type(arr->type);
        int   new_cap           = arr->capacity * 2;
        void* new_mem           = realloc(arr->arr, new_cap * size_of_arr_value);
        if (new_mem == NULL) {
            printf("Was not able to intialise memeory for Array. \n");
            exit(1);
        }
        arr->arr      = new_mem;
        arr->capacity = new_cap;
    }

    switch (arr->type) {
        case Array_type_int: {
            int* int_arr         = (int*) arr->arr;
            int_arr[arr->length] = *((int*) new_value);
            arr->length          += 1;
            
            break;
        }
        case Array_type_stmt: {
            Stmt* stmt_arr        = (Stmt*) arr->arr;
            stmt_arr[arr->length] = *((Stmt*) new_value);
            arr->length           += 1;

            break;
        }

        default: {
            printf("Was not able to add a value to array. new_value is of an unsupported type. \n");
            exit(1);
        }
    }
}








