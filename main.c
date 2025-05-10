#include "stdio.h"
#include "Lexer.h"
#include "Ast.h"
#include "my_String.h"
#include "Array_Stmt.h"

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
	String code = read_file("code_input.txt");
	
	Parser parser = parser_init(code.str);
	parser_parse(&parser);



	return 0;
}
