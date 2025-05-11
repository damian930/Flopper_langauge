#pragma once
#include "Lexer.h"
#include "stdio.h"
#include "ctype.h"

Lexer lexer_init(const char *text) {
    Lexer lexer = {
        .text            = text,
        .token_start_idx = 0,
        .current_idx     = 0,
        .column          = 1,
        .line            = 1,
    };

    return lexer;
}

bool lexer_is_at_end(const Lexer *lexer) {
    return lexer->text[lexer->current_idx] == '\0';
}

char lexer_peek_next_char(const Lexer *lexer) {
    if (lexer_is_at_end(lexer))
        return '\0';
    else
        return lexer->text[lexer->current_idx];
}

char lexer_consume_char(Lexer *lexer) {
    if (lexer_is_at_end(lexer))
        return '\0';
    else {
        // Indentation
        char current_char = lexer->text[lexer->current_idx];
        if (current_char == '\n') {
            lexer->column += 1;
            lexer->line    = 1;
        }
        else {
            lexer->line += 1;
        }

        lexer->current_idx += 1;
        return current_char;
    }
}

Token lexer_next_token(Lexer *lexer) {
    lexer->token_start_idx = lexer->current_idx;
    char prev_ch = lexer_consume_char(lexer);

    if (isspace(prev_ch)) {
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
    case ':': {
        if (lexer_peek_next_char(lexer) == (int) '=') {
            lexer_consume_char(lexer);
            return lexer_init_token(lexer, Token_Type_Declaration_Auto);
        }
        else
            return lexer_init_token(lexer, (int) ':');
    }
    case '+':
        return lexer_init_token(lexer, (int)'+');
    case '-':
        return lexer_init_token(lexer, (int)'-');
    case '*':
        return lexer_init_token(lexer, (int)'*');
    case '/':
        return lexer_init_token(lexer, (int)'/');
    case ';':
        return lexer_init_token(lexer, (int) ';');
    case '>': {
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
    case '<': {
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
    case '!': {
        if (lexer_peek_next_char(lexer) == '=') {
            lexer_consume_char(lexer);
            return lexer_init_token(lexer, Token_Type_Not_Equals);
        }
        else {
            return lexer_init_token(lexer, (int) '!');
        }
    }
    case '=': {
        if (lexer_peek_next_char(lexer) == '=')
        {
            lexer_consume_char(lexer);
            return lexer_init_token(lexer, Token_Type_Equals_Equals);
        }
        else
            return lexer_init_token(lexer, (int) '=');
    }

    default:
        if (prev_ch == '\"')
            return lexer_create_string_token(lexer);

        if (isdigit(prev_ch))
            return lexer_create_digit_token(lexer);

        return lexer_create_identifier_token(lexer);
        //  return lexer_init_token(lexer, Token_Type_Illegal);
    }
}

Token lexer_peek_next_token(Lexer* lexer) {
    int token_start_idx = lexer->current_idx;
    Token token         = lexer_next_token(lexer);
    lexer->current_idx  = token_start_idx;
    return token;
}

Token lexer_peek_n_token(Lexer* lexer, int n) {
    int token_start_idx = lexer->current_idx;
    
    Token token;
    for (int i=0; i<n; ++i)
        token = lexer_next_token(lexer);
    
    lexer->current_idx = token_start_idx;
    return token;
}

bool lexer_match_token(Lexer* lexer, Token_Type expected_type) {
    Token next_token = lexer_peek_next_token(lexer);
    if (next_token.type == expected_type) {
        lexer_next_token(lexer);
        return true;
    }
    else 
        return false;
}

Token lexer_consume_token__exits(Lexer* lexer, Token_Type expected_type, const char* error_message) {
    Token token = lexer_next_token(lexer);
    if (token.type != expected_type) {
        printf("%s \n",   error_message   );
        printf("Row: %d \n", lexer->line  );
        printf("Col: %d \n", lexer->column);
        exit(1);
    }
    return token;
}

Token lexer_create_string_token(Lexer *lexer) {
    while (lexer_peek_next_char(lexer) != '\"' && !lexer_is_at_end(lexer)) {
        lexer_consume_char(lexer);
    }

    if (lexer->text[lexer->current_idx] == '\"') {
        lexer_consume_char(lexer);
        return lexer_init_token(lexer, Token_Type_String);
    }
    else {
        printf("Was not able to parse a string. \n");
        exit(1);
    }
}

Token lexer_create_digit_token(Lexer *lexer) {
    // right now just to integers

    while (isdigit(lexer_peek_next_char(lexer))) {
        lexer_consume_char(lexer);
    }

    return lexer_init_token(lexer, Token_Type_Integer);
    
    // bool is_double = false;
    // if (lexer_peek_next_char(lexer) == '.') {
    //     lexer_consume_char(lexer);

    //     while (isdigit(lexer_peek_next_char(lexer))) {
    //         lexer_consume_char(lexer);
    //     }

    //     is_double = true;
    // }

    // if (is_double)
    //     return lexer_init_token(lexer, Token_Type_Double);
    // else
    //     return lexer_init_token(lexer, Token_Type_Integer);

}

Token lexer_create_identifier_token(Lexer* lexer) {
    // Read the indentifier
    char next_char = lexer_peek_next_char(lexer);
    while (!lexer_is_at_end(lexer) && (isalnum(next_char) || next_char == '_')) {
        lexer_consume_char(lexer);
        next_char = lexer_peek_next_char(lexer);
    }
    
    // Get the type of the identifier
    switch (lexer->text[lexer->token_start_idx]) {
        case 't': return lexer_match_keyword(lexer, 1, "rue",  3, Token_Type_True );
        case 'f': return lexer_match_keyword(lexer, 1, "alse", 4, Token_Type_False);
        case 'a': return lexer_match_keyword(lexer, 1, "nd",   2, Token_Type_And  );
        case 'o': return lexer_match_keyword(lexer, 1, "r",    1, Token_Type_Or   );
        case 'p': return lexer_match_keyword(lexer, 1, "rint", 4, Token_Type_Print);
        default : return lexer_init_token(lexer, Token_Type_Identifier); 
    }
}

Token lexer_match_keyword(Lexer* lexer, int current_token_offset, const char* rest, int rest_len, Token_Type type_to_match) {
    // Might be our token, need to check
    if (lexer->token_start_idx + current_token_offset + rest_len == lexer->current_idx) {
        for (int i=0; i<rest_len; ++i) {
            if (lexer->text[lexer->token_start_idx + current_token_offset + i] != rest[i])
            break;
        }
        return lexer_init_token(lexer, type_to_match);
    }

    return lexer_init_token(lexer, Token_Type_Identifier); 
}

void lexer_skip_whitespaces(Lexer *lexer) {
    while (isspace(lexer_peek_next_char(lexer))) {
        lexer_consume_char(lexer);
    }
    lexer->token_start_idx = lexer->current_idx;
}

inline Token lexer_init_token(Lexer *lexer, Token_Type type) {
    Token token = token_init(
        type, 
        &lexer->text[lexer->token_start_idx], 
        lexer ->current_idx - lexer->token_start_idx,
        lexer->column,
        lexer->line
    );
    // Manually skipping whitespaces here, so the next token is a legit token
    lexer_skip_whitespaces(lexer); 
    return token;
}



Token token_init(Token_Type type, char *lexeme, int length, int col, int row) {
    Token token = {
        .type   = type,
        .lexeme = lexeme,
        .length = length,
        .column = col,
        .line   = row,
    };

    return token;
}

void token_print(Token *token) {
    printf("Token: \n");
    printf("\tlength: %d \n", token->length);
    printf("\ttype:   %d \n", token->type);

    printf("\tlexeme: \"");
    for (int i = 0; i < token->length; i++) {
        printf("%c", token->lexeme[i]);
    }
    printf("\"\n");
}



// Ctrl + K + Ctrl + 0: fold all levels (namespace, class, method, and block)
// Ctrl + K + Ctrl + 1: namespace / @Component(For Angular)
// Ctrl + K + Ctrl + 2: class / methods
// Ctrl + K + Ctrl + 3: methods / blocks
// Ctrl + K + Ctrl + 4: blocks / inner blocks
// Ctrl + K + Ctrl + [ or Ctrl + k + ]: current cursor block
// Ctrl + K + Ctrl + j: UnFold