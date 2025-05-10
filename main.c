#include "stdio.h"
#include "Lexer.h"
#include "Ast.h"
#include "my_String.h"
#include "Array_Stmt.h"
#include "Array.h"
#include "Langauge.h"

String read_file(const char* file_name) {
	String str = string_init("");

	FILE* file         = NULL;
	errno_t error_code = fopen_s(&file, file_name, "r");
	if (error_code != 0 ) {
		printf("Was not able to open the file: %s. \n", file_name);
		exit(1);
	}
	char* buffer[100];
	while(fgets(buffer, 100, file) != NULL) {
		string_add_c_string(&str, buffer);
	}
	fclose(file);

	return str;
}

int main() {
	// String code = read_file("code_input.txt");

	// Parser parser = parser_init(code.str);
	// parser_parse(&parser);

	// Testing variables map
	Map_variables map_variables = map_variables_init();
	map_variables_add(
		&map_variables, 
		string_init("Flopper"), 
		(Evaluation) { .type = Evaluation_type_integer, .union_.boolean = true } 
	);
	map_variables_delete(&map_variables);


	printf(" --> DONE <-- \n");
	
	return 0;

}
