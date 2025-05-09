#pragma once
#include "Ast.h"
#include "Lexer.h"
#include "my_String.h"

typedef enum Expr_type Expr_type;

typedef struct Primary Primary;
typedef struct Unary   Unary;
typedef struct Binary  Binary;
typedef struct Expr    Expr;

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

enum Expr_type {
    Expr_type_primary,
    Expr_type_unary,
    Expr_type_binary,
};

struct Primary {
    Token_Type type;
    union {
        int integer;
        bool boolean;
        // char* string;
    } union_;
};

struct Unary {
    Token operator;
    Expr* right;
};

struct Binary {
    Token operator;
    Expr* left;
    Expr* right;
} ;

struct Expr {
    Expr_type type;
    union {
        Primary primary;
        Unary unary;
        Binary binary;
    } union_;
} ;

Expr* primary(Lexer* lexer);
Expr* unary(Lexer* lexer);
Expr* factor(Lexer* lexer);
Expr* term(Lexer* lexer);
Expr* comparison(Lexer* lexer);
Expr* equality(Lexer* lexer);
Expr* expression(Lexer* lexer);

// void epxr_delete(Expr* expr);

// For debugging
String expr_to_string(Expr* expr);



typedef struct {
    Lexer lexer;
    Expr* ast;

    bool had_error;
} Parser;

Parser parser_init(const char* text);

void parser_parse(Parser* parser);


typedef enum {
    Evaluation_type_integer,
    Evaluation_type_boolean,
} Evaluation_type;

typedef struct {
    Evaluation_type type;
    union {
        int  integer;
        bool boolean;
    } union_;
} Evaluation;

void parser_evaluate_expr(Parser* parser);
Evaluation evalueate_expression(Expr* expr);

