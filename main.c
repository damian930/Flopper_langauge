#include "stdio.h"
#include "Lexer.h"
#include "Parser.h"
#include "my_String.h"
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
		string_add_whole_c_string(&str, buffer);
	}
	fclose(file);

	return str;
}

int main() {
	String code      = read_file("code_input.txt");
	Language flopper = language_init(code.str);
	language_execute(&flopper);

	//printf("\n --> DONE <-- \n");
	/*Lexer lexer = lexer_init(code.str);
	while (!lexer_is_at_end(&lexer)) {
		Token token = lexer_next_token(&lexer);
		token_print(&token);
	}*/


	return 0;

}
