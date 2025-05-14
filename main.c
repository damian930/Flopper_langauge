#include "stdio.h"
#include "Lexer.h"
#include "Parser.h"
#include "my_String.h"
#include "Array.h"
#include "Langauge.h"
#include "string.h"

String read_file(const char* file_name);

int main(int args, char *argv[]) {
	String code      = read_file("code_input.txt");
	Language flopper = language_init(code.str);
	language_execute(&flopper);
	language_delete(&flopper);

	//for (int i = 0; i < 4; i++) {
	//	i++;
	//	int i = 2;
	//	i++;
	//	printf("%d --> ", i);
	//}

	
	
	//String code = string_init(" { } ");
	//Lexer lexer = lexer_init(code.str);
	//while (lexer_peek_next_token(&lexer).type != Token_Type_EOF) {
	//	Token token = lexer_next_token(&lexer);
	//	token_print(&token);
	//}
	
	// if (args == 1) {
	// 	String code      = read_file("code_input.txt");
	// 	Language flopper = language_init(code.str);
	// 	language_execute(&flopper);
	// }
	// else if (args == 2) {
	// 	if (strcmp("test", argv[1])) {
	// 		printf("Enter code line by line. \n");
	// 		printf("If an error occurs, you may still continue. \n");
	// 		printf("To exit, enter \"exit\". \n");

	// 		String code = string_init("");
	// 		while(strcmp(code.str, "exit") == false) {
	// 			char user_entered_code[200];
	// 			printf("> ");
	// 			scanf_s("%s", user_entered_code, 200);

	// 			printf("\n\n %s", user_entered_code);

	// 			break;
	// 		}
	// 	}
	// }

	
	//printf("\n --> DONE <-- \n");
	


	return 0;

}

// ====================================================

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
