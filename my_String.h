#pragma once
#include <stdbool.h>

typedef struct {
    char* str;
    
    int length;
    int capacity;
} String;

String string_init(char* str);
void string_delete(String* str);

void string_add_c_string(String* str, char* other_str);
void string_add_string(String* str, String* other_str);
bool string_equal(String* str1, String* str2);

void string_print(String* str);
void string_debug_print(String* str);



// == TESTING 

/*{	
	String str = string_init("flopper");
	string_print(&str);

	string_add_c_string(&str, " money");
	string_print(&str);

	String other = string_init(" peter peter peter peter peter");
	string_add_string(&str, &other);
	string_print(&str);
	string_delete(&other);

	string_print(&other);

	other = string_init("other");
	string_print(&other);
	string_delete(&other);

	string_delete(&str);

	string_print(&str);
	string_print(&other);
}*/



