#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef int32_t   s32;
typedef uint32_t  u32;

typedef enum   Token_Type  Token_Type;
typedef struct Token       Token;
typedef struct Lexer       Lexer;

enum Token_Type {
    // Mentally isnert other ASCII in here

    Token_Type_Equals_Equals,
    Token_Type_Not_Equals,
    Token_Type_Greater_Or_Equals,
    Token_Type_Less_Or_Equals,
    
    // Value types for lexeme values
    Token_Type_String = 256, // THIS IS NOT A REAL TYPE YET
    Token_Type_Integer,
    Token_Type_True,
    Token_Type_False,

    Token_Type_And,
    Token_Type_Or,

    Token_Type_Print,

    Token_Type_Identifier,

    Token_Type_If,
    Token_Type_Else,

    Token_Type_Declaration_Auto,

    Token_Type_For,
    Token_Type_In,
    Token_Type_Dot_Dot,

    Token_Type_While,

    Token_Type_EOF
};

struct Token {
    Token_Type type;
    char*      lexeme;
    u32        length;

    u32 start_row_n;
    u32 start_col_n;
    
    u32 end_row_n;
    u32 end_col_n;
};

struct Lexer {
    char* text;
    u32   text_len;

    u32 token_start_idx;
    u32 current_idx;

    u32 token_start_row_n;
    u32 token_start_col_n;
    
    u32 token_end_row_n;
    u32 token_end_col_n;
};

Lexer lexer_init(const char *text);

bool lexer_is_at_end     (Lexer *lexer);
char lexer_peek_next_char(Lexer *lexer);
char lexer_peek_nth_char (Lexer* lexer, u32 n);
char lexer_consume_char  (Lexer *lexer);
// bool lexer_match_char(Lexer* lexer);

Token lexer_next_token     (Lexer *lexer);
Token lexer_peek_next_token(Lexer* lexer);
Token lexer_peek_nth_token (Lexer* lexer, u32 n);
bool  lexer_match_token    (Lexer* lexer, Token_Type expected_type);
Token lexer_consume_token__exits(Lexer* lexer, Token_Type expected_type, const char* error_message);

Token lexer_init_token             (Lexer *lexer, Token_Type type);
Token lexer_create_string_token    (Lexer *lexer);
Token lexer_create_digit_token     (Lexer *lexer);
Token lexer_create_identifier_token(Lexer* lexer);
Token lexer_match_keyword          (Lexer* lexer, u32 current_token_offset, const char* rest, u32 rest_len, Token_Type type_to_match);
bool  lexer_skip_whitespaces       (Lexer *lexer);
bool  lexer_skip_comments          (Lexer* lexer);

Token token_init(Token_Type type, char *lexeme, u32 length, u32 col, u32 row);
void token_print(Token *token);

