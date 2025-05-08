#pragma once

typedef struct {
    char* str;
    
    int length;
    int capacity;
} String;

String string_init(char* str);
void string_delete(String* str);

void string_add_c_string(String* str, char* other_str);
void string_add_string(String* str, String* other_str);

void string_print(String* str);
void string_debug_print(String* str);



// == TESTING 




