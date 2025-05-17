#pragma once
#include <stdio.h>
#include <stdlib.h>

// Macro to create a dynamic array struct for a given data type
#define Array_create_type(data_type, struct_name)     \
typedef struct {                                       \
    data_type* data;                                   \
    int length;                                        \
    int capacity;                                      \
} struct_name;

// Macro to initialize the dynamic array
#define Array_init(data_type, struct_name, func_name)         \
struct_name func_name() {                                     \
    int length = 0;                                           \
    int capacity = 20;                                        \
    data_type* arr = malloc(capacity * sizeof(data_type));    \
    if (arr == NULL) {                                        \
        printf("Unable to allocate memory for array.\n");     \
        exit(1);                                              \
    }                                                         \
    return (struct_name) {                                    \
        .data = arr,                                          \
        .length = length,                                     \
        .capacity = capacity                                  \
    };                                                        \
}

// Macro to free memory used by the array
#define Array_delete(struct_name, func_name)          \
void func_name(struct_name* arr) {                    \
    free(arr->data);                                  \
    arr->data = NULL;                                 \
    arr->length = 0;                                  \
    arr->capacity = 0;                                \
}

// Macro to add a value to the array (by value, not pointer)
#define Array_add_by_value(data_type, struct_name, func_name)          \
void func_name(struct_name* arr, data_type new_value) {                \
    if (arr->length == arr->capacity) {                                \
        int new_capacity = arr->capacity * 2;                          \
        data_type* new_mem = realloc(arr->data, new_capacity * sizeof(data_type)); \
        if (new_mem == NULL) {                                         \
            printf("Unable to allocate memory for resizing array.\n"); \
            exit(1);                                                   \
        }                                                              \
        arr->data = new_mem;                                           \
        arr->capacity = new_capacity;                                  \
    }                                                                  \
    arr->data[arr->length] = new_value;                                \
    arr->length += 1;                                                  \
}
