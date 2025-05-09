#pragma once
#include <stdbool.h>

typedef struct {
    char* text;
    int token_start_idx;
    int current_idx;

    // int column;
    // int line;
} Lexer;

typedef enum {
    // Mentally isnert some other ASCII in here

    Token_Type_Equals_Equals,
    Token_Type_Not_Equals,
    Token_Type_Greater_Or_Equals,
    Token_Type_Less_Or_Equals,
    
    Token_Type_String = 256,
    Token_Type_Integer,
    Token_Type_True,
    Token_Type_False,

    Token_Type_Illegal,
    Token_Type_EOF
} Token_Type;

typedef struct {
    Token_Type type;
    char *lexeme;
    int length;

    // int column;
    // int line

} Token;

Lexer lexer_init(const char *text);

static bool lexer_is_at_end(const Lexer *lexer);
static char lexer_peek_next_char(const Lexer *lexer);
static char lexer_consume_char(Lexer *lexer);

Token lexer_next_token(Lexer *lexer);
Token lexer_peek_next_token(Lexer* lexer);
bool  lexer_match_token(Lexer* lexer, Token_Type expected_type);
void  lexer_consume_token__exits(Lexer* lexer, Token_Type expected_type, const char* error_message);

static Token lexer_init_token(Lexer *lexer, Token_Type type);
static Token lexer_create_string_token(Lexer *lexer);
static Token lexer_create_digit_token(Lexer *lexer);
static void  lexer_skip_whitespaces(Lexer *lexer);

Token token_init(Token_Type type, char *lexeme, int length);
void token_print(Token *token);

