#pragma once
#include "Lexer.h"
#include "stdio.h"
#include "ctype.h"

Lexer lexer_init(const char* text) {
    Lexer lexer = {
        .text = text,
        .token_start_idx = 0,
        .current_idx = 0
        
        // .column  = 1,
        // .line    = 1
    };

    return lexer;
}

inline
bool lexer_is_at_end(const Lexer* lexer) {
    return lexer->text[lexer->current_idx] == '\0';        
}

char lexer_peek_next(const Lexer* lexer) {
    if (lexer_is_at_end(lexer))
        return '\0';
    else
        return lexer->text[lexer->current_idx];
}

char lexer_consume_char(Lexer* lexer) {
    // Dont know if i like returning \0 if end
    // I kinda only want to return EOF if \0
    // So maybe making it an assert if at some time i am in the end witout EOF 

    if (lexer_is_at_end(lexer))
        return '\0';
    else 
        return lexer->text[lexer->current_idx++];
}

Token lexer_next_token(Lexer* lexer) {
    lexer->token_start_idx = lexer->current_idx;
    char prev_ch = lexer_consume_char(lexer);

    switch(prev_ch) {
        case '(': return lexer_init_token(lexer, (int) '(');
        case ')': return lexer_init_token(lexer, (int) ')');
        default:
            if (prev_ch == '\"')
                return lexer_create_string_token(lexer);
            if (isdigit(prev_ch))
                return lexer_create_digit_token(lexer);

            return lexer_init_token(lexer, Token_Type_Illegal);
    }


}

Token lexer_create_string_token(Lexer* lexer) {
    while(lexer_peek_next(lexer) != '\"' && !lexer_is_at_end(lexer)) {
        lexer_consume_char(lexer);
    }

    if (lexer->text[lexer->current_idx] == '\"') {
        lexer_consume_char(lexer);
        return lexer_init_token(lexer, Token_Type_String);
        
    }
    else 
        return lexer_init_token(lexer, Token_Type_Error_String);


}

Token lexer_create_digit_token(Lexer* lexer) {
    // right now just to integers

    while(isdigit(lexer_peek_next(lexer)) && !lexer_is_at_end(lexer)) {
        lexer_consume_char(lexer);
    }

    if (!lexer_is_at_end(lexer)) {
        lexer_consume_char(lexer);
        return lexer_init_token(lexer, Token_Type_Integer);
    }

    return lexer_init_token(lexer, Token_EOF);
    
}

inline
Token lexer_init_token(Lexer* lexer, Token_Type type) {
    return token_init(type, &lexer->text[lexer->token_start_idx], lexer->current_idx - lexer->token_start_idx);
}


Token token_init(Token_Type type, char* lexeme, int length) {
    Token token = {
        .type = type,
        .lexeme = lexeme,
        .length = length
    };

    return token;

}

void token_print(Token* token) {
    printf("Token: \n");
    printf("\tlength: %d \n", token->length);
    
    printf("\tlexeme: \"");
    for(int i=0; i<token->length; i++) {
        printf("%c", token->lexeme[i]);
    }
    printf("\"\n");
}


