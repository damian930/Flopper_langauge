#pragma once
#include <stdbool.h>


typedef struct {
    char* text;
    int token_start_idx;
    int current_idx;

    // int column;
    // int line;

} Lexer;

typedef enum  {
    // Mentally isnert some other ASCII in here
    
    Token_Type_String,
    Token_Type_Error_String,
    Token_Type_Integer,
    Token_Type_Illegal,
    Token_EOF
} Token_Type;

typedef struct {
    Token_Type type;
    char* lexeme;
    int length;

    // int column;
    // int line

} Token;


Lexer lexer_init(const char* text);
bool lexer_is_at_end(const Lexer* lexer);
char lexer_peek_next(const Lexer* lexer);
char lexer_consume_char(Lexer* lexer);
Token lexer_next_token(Lexer* lexer);
Token lexer_init_token(Lexer* lexer, Token_Type type);
Token lexer_create_string_token(Lexer* lexer);
Token lexer_create_digit_token(Lexer* lexer);

Token token_init(Token_Type type, char* lexeme, int length);
void token_print(Token* token);








