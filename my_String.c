#include "stdio.h"
#include <stdlib.h>
#include "my_String.h"
#include "string.h"
#include "stdbool.h"

static 
int length_of_str(const char* str) {
    int length = -1;
    while(str[++length] != '\0');
    return length;
}

String string_init(const char* static_str) {
    int static_str_length = length_of_str(static_str);
    int capacity   = static_str_length + 21;
    
    char* str = malloc(sizeof(char) * capacity);
    if (str == NULL) {
        printf("Was not able to allocate a string on heap. ");
        printf("The size of memory needed was: %d bytes. \n", capacity * sizeof(char));
        exit(1);
    }

    for (int i=0; i<static_str_length; ++i) {
        str[i] = static_str[i];
    }
    str[static_str_length] = '\0';

    return (String) {
        .str      = str,
        .length   = static_str_length,
        .capacity = capacity,
    };
}

void string_delete(String* str) {
    free(str->str);
    str->str      = NULL;
    str->capacity = 0;
    str->length   = 0;
}

void string_add_c_string(String* str, char* other_str, int other_str_len) {
    int capacity = str->capacity;
    if (capacity < str->length + other_str_len + 1) {
        while (capacity < str->length + other_str_len + 1) {
            capacity *= 2;
        }

        str->str = realloc(str->str, sizeof(char) * capacity);
        if (str->str == NULL) {
            printf("Was not able to allocate a string on heap. ");
            printf("The size of memory needed was: %d bytes. \n", capacity * sizeof(char));
            exit(1);
        }
        str->capacity = capacity;
    }
    
    for (int i = str->length; i < str->length + other_str_len; ++i) {
        str->str[i] = other_str[i - str->length];
    }
    str->length += other_str_len;
    str->str[str->length] = '\0';
}

void string_add_whole_c_string(String* str, char* other_str) {
    int other_str_len = length_of_str(other_str);
    int capacity = str->capacity;
    if (capacity < str->length + other_str_len + 1) {
        while (capacity < str->length + other_str_len + 1) {
            capacity *= 2;
        }

        str->str = realloc(str->str, sizeof(char) * capacity);
        if (str->str == NULL) {
            printf("Was not able to allocate a string on heap. ");
            printf("The size of memory needed was: %d bytes. \n", capacity * sizeof(char));
            exit(1);
        }
        str->capacity = capacity;
    }

    for (int i = str->length; i < str->length + other_str_len; ++i) {
        str->str[i] = other_str[i - str->length];
    }
    str->length += other_str_len;
    str->str[str->length] = '\0';
}

void string_add_string(String* str, String* other_str) {
    int capacity = str->capacity;
    if (capacity < str->length + other_str->length + 1) {
        while (capacity < str->length + other_str->length + 1) {
            capacity *= 2;
        }

        str->str = realloc(str->str, capacity);
        if (str->str == NULL) {
            printf("Was not able to allocate a string on heap. ");
            printf("The size of memory needed was: %d bytes. \n", capacity * sizeof(char));
            exit(1);
        }
        str->capacity = capacity;
    }
    
    for (int i = str->length; i < str->length + other_str->length; ++i) {
        str->str[i] = other_str->str[i - str->length];
    }
    str->length += other_str->length;
    str->str[str->length] = '\0';
}

bool string_equal(String* str1, String* str2) {
    if (str1->length != str2->length) return false;

    for(int i=0; i<str1->length; ++i) {
        if (str1->str[i] != str2->str[i])
            return false;
    }
    return true;
}



void string_print(String* str) {
    printf("%s \n", str->str);
}

void string_debug_print(String* str) {
    printf("String --> str: '%s', ", str->str);
    printf("len: %d, cap: %d \n", str->length, str->capacity);
}


