#pragma once
#include "Lexer.h"
#include "stdio.h"
#include "ctype.h"

Lexer lexer_init(const char *text)
{
    Lexer lexer = {
        .text = text,
        .token_start_idx = 0,
        .current_idx = 0,
    };

    return lexer;
}

inline bool lexer_is_at_end(const Lexer *lexer)
{
    return lexer->text[lexer->current_idx] == '\0';
}

char lexer_peek_next_char(const Lexer *lexer)
{
    if (lexer_is_at_end(lexer))
        return '\0';
    else
        return lexer->text[lexer->current_idx];
}

char lexer_consume_char(Lexer *lexer)
{
    if (lexer_is_at_end(lexer))
        return '\0';
    else
    {
        // Line and column code positions will be changed here
        // since this is the only function that consumes chars
        return lexer->text[lexer->current_idx++];
    }
}


// == Grammar
//          Current grammar
// expression :: equality ;
// equality   :: comparison ( ( "!=" | "==" ) comparison )* ;
// comparison :: term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
// term       :: factor ( ( "-" | "+" ) factor )* ;
// factor     :: unary ( ( "/" | "*" ) unary )* ;
// unary      :: ( "!" | "-" ) unary
//                | primary ;
// primary    :: INTEGER | STRING | "(" expression ")" ;

Token lexer_next_token(Lexer *lexer)
{
    lexer->token_start_idx = lexer->current_idx;
    char prev_ch = lexer_consume_char(lexer);

    if (isspace(prev_ch))
    {
        lexer_skip_whitespaces(lexer);

        lexer->token_start_idx = lexer->current_idx;
        prev_ch = lexer_consume_char(lexer);
    }

    switch (prev_ch)
    {
    case '(':
        return lexer_init_token(lexer, (int)'(');
    case ')':
        return lexer_init_token(lexer, (int)')');
    case '+':
        return lexer_init_token(lexer, (int)'+');
    case '-':
        return lexer_init_token(lexer, (int)'-');
    case '*':
        return lexer_init_token(lexer, (int)'*');
    case '/':
        return lexer_init_token(lexer, (int)'/');
    case '>':
    {
        if (lexer_peek_next_char(lexer) == '=')
        {
            lexer_consume_char(lexer);
            return lexer_init_token(lexer, Token_Type_Greater_Or_Equals);
        }
        else
        {
            return lexer_init_token(lexer, (int)'>');
        }
    }
    case '<':
    {
        if (lexer_peek_next_char(lexer) == '=')
        {
            lexer_consume_char(lexer);
            return lexer_init_token(lexer, Token_Type_Less_Or_Equals);
        }
        else
        {
            return lexer_init_token(lexer, (int)'<');
        }
    }

    case '!':
    {
        if (lexer_peek_next_char(lexer) == '=')
        {
            lexer_consume_char(lexer);
            return lexer_init_token(lexer, Token_Type_Not_Equals);
        }
        else
        {
            return lexer_init_token(lexer, Token_Type_Illegal);
        }
    }
    case '=':
    {
        if (lexer_peek_next_char(lexer) == '=')
        {
            lexer_consume_char(lexer);
            return lexer_init_token(lexer, Token_Type_Equals_Equals);
        }
        else
            return lexer_init_token(lexer, Token_Type_Illegal);
    }

    default:
        if (prev_ch == '\"')
            return lexer_create_string_token(lexer);

        if (isdigit(prev_ch))
            return lexer_create_digit_token(lexer);

        return lexer_init_token(lexer, Token_Type_Illegal);
    }
}

Token lexer_peek_next_token(Lexer* lexer) {
    int token_start_idx = lexer->current_idx;
    Token token = lexer_next_token(lexer);
    lexer->current_idx = token_start_idx;
    return token;
}

Token lexer_create_string_token(Lexer *lexer) {
    while (lexer_peek_next_char(lexer) != '\"' && !lexer_is_at_end(lexer))
    {
        lexer_consume_char(lexer);
    }

    if (lexer->text[lexer->current_idx] == '\"')
    {
        lexer_consume_char(lexer);
        return lexer_init_token(lexer, Token_Type_String);
    }
    else
        return lexer_init_token(lexer, Token_Type_Error_String);
}

Token lexer_create_digit_token(Lexer *lexer) {
    // right now just to integers

    while (isdigit(lexer_peek_next_char(lexer)))
    {
        lexer_consume_char(lexer);
    }

    bool is_float = false;
    if (lexer_peek_next_char(lexer) == '.')
    {
        lexer_consume_char(lexer);

        while (isdigit(lexer_peek_next_char(lexer)))
        {
            lexer_consume_char(lexer);
        }

        is_float = true;
    }

    if (is_float)
        return lexer_init_token(lexer, Token_Type_Float);
    else
        return lexer_init_token(lexer, Token_Type_Integer);
}

void lexer_skip_whitespaces(Lexer *lexer) {
    while (isspace(lexer_peek_next_char(lexer)))
    {
        lexer_consume_char(lexer);
    }
}

inline Token lexer_init_token(Lexer *lexer, Token_Type type) {
    return token_init(type, &lexer->text[lexer->token_start_idx], lexer->current_idx - lexer->token_start_idx);
}


Token token_init(Token_Type type, char *lexeme, int length) {
    Token token = {
        .type = type,
        .lexeme = lexeme,
        .length = length};

    return token;
}


void token_print(Token *token)
{
    printf("Token: \n");
    printf("\tlength: %d \n", token->length);
    printf("\ttype:   %d \n", token->type);

    printf("\tlexeme: \"");
    for (int i = 0; i < token->length; i++)
    {
        printf("%c", token->lexeme[i]);
    }
    printf("\"\n");
}
