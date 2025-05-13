#pragma once
#include <stdint.h>
#include "Lexer.h"
#include "stdio.h"
#include "ctype.h"

static u32 c_string_len(const char* str) {
    u32 len = 0;
    while(str[len] != '\0')
        len += 1;
    return len;
}

Lexer lexer_init(const char *text) {
    Lexer lexer = {
        .text              = text,
        .text_len          = c_string_len(text),

        .token_start_idx   = 0,
        .current_idx       = 0,
        
        .token_start_row_n = 1,
        .token_start_col_n = 0,
        
        .token_end_row_n   = 1,
        .token_end_col_n   = 0,
    };

    return lexer;
}

bool lexer_is_at_end(Lexer *lexer) {
    if (lexer->current_idx < lexer->text_len)
        return false;
    else if (lexer->current_idx == lexer->text_len)
        return true;
    else {
        printf("BACK_END_ERROR: Somehow lexer index went out of the text char array. \n");
        exit(1);
    }
}

char lexer_peek_next_char(Lexer *lexer) {
    if (lexer_is_at_end(lexer))
        return '\0';
    else
        return lexer->text[lexer->current_idx];
}

char lexer_peek_nth_char(Lexer* lexer, u32 n) {
    if (lexer->current_idx + n < lexer->text_len) 
        return lexer->text[lexer->current_idx + n];
    else 
        return '\0';
}

char lexer_consume_char(Lexer *lexer) {
    if (lexer_is_at_end(lexer))
        return '\0';
    else {
        // Deal with row, col information for tokens
        char consumed_char  = lexer->text[lexer->current_idx];
        lexer->current_idx += 1;

        if (consumed_char == '\n') {
            lexer->token_end_row_n += 1;
            lexer->token_end_col_n  = 1;
        }
        else 
            lexer->token_end_col_n += 1;


        return consumed_char;
    }
}

Token lexer_next_token(Lexer *lexer) {
    while (   lexer_skip_whitespaces(lexer) == true
           || lexer_skip_comments(lexer)    == true
    ); 

    // Setting up data for the upcomming token
    lexer->token_start_row_n = lexer->token_end_row_n;
    lexer->token_start_col_n = lexer->token_end_col_n;
    char prev_char = lexer_consume_char(lexer);

    switch (prev_char) 
    {
    case '\0':
        return lexer_init_token(lexer, Token_Type_EOF);
    case '(':
        return lexer_init_token(lexer, (int)'(');
    case ')':
        return lexer_init_token(lexer, (int)')');
    case '{':
        return lexer_init_token(lexer, (int) '{');
    case '}':
        return lexer_init_token(lexer, (int) '}');
    case '+':
        return lexer_init_token(lexer, (int)'+');
    case '-':
        return lexer_init_token(lexer, (int)'-');
    case '*':
        return lexer_init_token(lexer, (int)'*');
    case ';':
        return lexer_init_token(lexer, (int) ';');
    case '/':
        return lexer_init_token(lexer, (int)'/');
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
    case ':': {
        if (lexer_peek_next_char(lexer) == (int) '=') {
            lexer_consume_char(lexer);
            return lexer_init_token(lexer, Token_Type_Declaration_Auto);
        }
        else
            return lexer_init_token(lexer, (int) ':');
    }

    default:
        if (prev_char == '\"')
            return lexer_create_string_token(lexer);

        if (isdigit(prev_char))
            return lexer_create_digit_token(lexer);

        return lexer_create_identifier_token(lexer);
        //  return lexer_init_token(lexer, Token_Type_Illegal);
    }
}

Token lexer_peek_next_token(Lexer* lexer) {
    // Only storing end values, since start are changed to end when token is created
    u32 token_start_idx = lexer->current_idx;
    u32 token_end_col_n = lexer->token_end_col_n;
    u32 token_end_row_n = lexer->token_end_row_n;

    Token token         = lexer_next_token(lexer);

    lexer->current_idx     = token_start_idx;
    lexer->token_end_col_n = token_end_col_n;
    lexer->token_end_row_n = token_end_row_n;

    return token;
}

Token lexer_peek_nth_token(Lexer* lexer, u32 n) {
    // Only storing end values, since start are changed to end when token is created
    u32 token_start_idx = lexer->current_idx;
    u32 token_end_col_n = lexer->token_end_col_n;
    u32 token_end_row_n = lexer->token_end_row_n;
    
    Token token;
    for (int i=0; i<n; ++i)
        token = lexer_next_token(lexer);

    lexer->current_idx     = token_start_idx;
    lexer->token_end_col_n = token_end_col_n;
    lexer->token_end_row_n = token_end_row_n;

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
        printf("%s \n",      error_message);
        printf("Start position: (row, col) -> (%d, %d). \n", lexer->token_start_row_n, lexer->token_start_col_n);
        printf("End   position: (row, col) -> (%d, %d). \n", lexer->token_end_row_n,   lexer->token_end_col_n  );
        exit(1);
    }
    return token;
}

Token lexer_create_string_token(Lexer *lexer) {
    while (lexer_peek_next_char(lexer) != '\"' && !lexer_is_at_end(lexer)) {
        lexer_consume_char(lexer);
    }

    // TODO: maybe change to match char
    if (lexer->text[lexer->current_idx] == '\"') {
        lexer_consume_char(lexer);
        return lexer_init_token(lexer, Token_Type_String);
    }
    else {
        printf("Was not able to parse a string, closing '\"' wasn't found. \n");
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
    // Read the indentifier to then math the type of it in the switch statement
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
        case 'e': return lexer_match_keyword(lexer, 1, "lse",  3, Token_Type_Else );
        case 'i': return lexer_match_keyword(lexer, 1, "f",    1, Token_Type_If   );
        default : return lexer_init_token(lexer, Token_Type_Identifier); 
    }
}

Token lexer_match_keyword(Lexer* lexer, u32 current_token_offset, const char* rest, u32 rest_len, Token_Type type_to_match) {
    // Might be our token, need to check
    if (lexer->token_start_idx + current_token_offset + rest_len == lexer->current_idx) {
        for (int i=0; i<rest_len; ++i) {
            if (lexer->text[lexer->token_start_idx + current_token_offset + i] != rest[i])
                return lexer_init_token(lexer, Token_Type_Identifier);
        }
        return lexer_init_token(lexer, type_to_match);
    }

    return lexer_init_token(lexer, Token_Type_Identifier); 
}

bool lexer_skip_whitespaces(Lexer *lexer) {
    // Handeling all the whitespace characters myself, to not miss handeling any
    // TODO: dont forget, that token row,col data is dependant on correct implementation inside lexer_consume_char.
    //       if an unsupported whispace by lexer_consume_char is consumed here, the row,col data will be invalid.
    bool skipped = false;
    while (
        lexer_peek_next_char(lexer) == ' '  ||
        lexer_peek_next_char(lexer) == '\n' ||
        lexer_peek_next_char(lexer) == '\t' 
    ) {
        skipped = true;
        lexer_consume_char(lexer);
    }   
    
    lexer->token_start_idx = lexer->current_idx;
    return skipped;
}

bool lexer_skip_comments(Lexer* lexer) {
    // Default comment '//'
    if (lexer_peek_nth_char(lexer, 0) == '/' && lexer_peek_nth_char(lexer, 1) == '/') {
        lexer_consume_char(lexer); // '/'
        lexer_consume_char(lexer); // '/'

        while (!lexer_is_at_end(lexer) && lexer_peek_next_char(lexer) != '\n')
            lexer_consume_char(lexer);
        
        lexer_consume_char(lexer); // '\n'

        lexer->token_start_idx = lexer->current_idx;
        return true;
    }

    // Block comments '/* */'
    if (lexer_peek_nth_char(lexer, 0) == '/' && lexer_peek_nth_char(lexer, 1) == '*') {
        lexer_consume_char(lexer); // '/'
        lexer_consume_char(lexer); // '*'
        
        int comment_level = 1;
        while (!lexer_is_at_end(lexer) && comment_level != 0) {
            if (lexer_peek_nth_char(lexer, 0) == '/' && lexer_peek_nth_char(lexer, 1) == '*') {
                lexer_consume_char(lexer); // '/'
                lexer_consume_char(lexer); // '*'
                comment_level += 1;
            }
            else if (lexer_peek_nth_char(lexer, 0) == '*' && lexer_peek_nth_char(lexer, 1) == '/') {
                lexer_consume_char(lexer); // '*'
                lexer_consume_char(lexer); // '/'
                comment_level -= 1;
            }
            else
                lexer_consume_char(lexer);
        }
        return true;
    }
    return false;
}

Token lexer_init_token(Lexer *lexer, Token_Type type) {
    Token token = token_init(
        type, 
        &lexer->text[lexer->token_start_idx], 
        lexer ->current_idx - lexer->token_start_idx,

        lexer->token_start_row_n,
        lexer->token_start_col_n,
        
        lexer->token_end_row_n,
        lexer->token_end_col_n
    );
    // Manually skipping whitespaces here, so the next token is a legit token
    lexer_skip_whitespaces(lexer); 
    return token;
}



Token token_init(Token_Type type, char *lexeme, u32 length, 
                 u32 start_row_n, u32 start_col_n, u32 end_row_n, u32 end_col_n) {
    Token token = {
        .type   = type,
        .lexeme = lexeme,
        .length = length,
        
        .start_row_n = start_row_n,
        .start_col_n = start_col_n,
        
        .end_row_n   = end_row_n,
        .end_col_n   = end_col_n,
    };

    return token;
}

void token_print(Token *token) {
    printf("Token: \n");
    printf("\ttype       : %d \n", token->type);
    printf("\tlexeme     : \"");
    for (int i = 0; i < token->length; i++) {
        printf("%c", token->lexeme[i]);
    }
    printf("\" \n");
    printf("\tstart_row_n: %d \n", token->start_row_n  );
    printf("\tstart_col_n: %d \n", token->start_col_n  );
    printf("\tend_row_n  : %d \n", token->end_row_n - 1);
    printf("\tend_col_n  : %d \n", token->end_col_n    );

    printf("\n");
}



// Ctrl + K + Ctrl + 0: fold all levels (namespace, class, method, and block)
// Ctrl + K + Ctrl + 1: namespace / @Component(For Angular)
// Ctrl + K + Ctrl + 2: class / methods
// Ctrl + K + Ctrl + 3: methods / blocks
// Ctrl + K + Ctrl + 4: blocks / inner blocks
// Ctrl + K + Ctrl + [ or Ctrl + k + ]: current cursor block
// Ctrl + K + Ctrl + j: UnFold