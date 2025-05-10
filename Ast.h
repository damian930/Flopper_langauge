#pragma once
#include "Ast.h"
#include "Lexer.h"
#include "my_String.h"
#include "Array_Stmt.h"  // This include is now safe

// ==================================
typedef enum Expr_type Expr_type;

typedef struct Primary Primary;
typedef struct Unary   Unary;
typedef struct Binary  Binary;
typedef struct Expr    Expr;
// ==================================

// ==================================
typedef enum Stmt_type Stmt_type;
typedef struct Stmt    Stmt;
// ==================================

// ==================================
typedef enum Evaluation_type Evaluation_type; // TODO: change the name, make is smaller
typedef struct Evaluation Evaluation; 
// ==================================


// ==================================
typedef struct Parser     Parser;
// ==================================

// ========================================================================================

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
// void epxr_delete(Expr* expr);   // TODO: need to delete it from heap
String expr_to_string(Expr* expr); // For debugging


// ========================================================================================


enum Stmt_type{
    Stmt_type_expr,
    Stmt_type_print,
};

struct Stmt{
    Stmt_type type;
    Expr* expr;
};


// ========================================================================================


typedef enum {
    Evaluation_type_integer,
    Evaluation_type_boolean,
} Evaluation_type;

struct Evaluation {
    Evaluation_type type;
    union {
        int  integer;
        bool boolean;
    } union_;
};


// ========================================================================================


struct Parser {
    Lexer lexer;
    Array_stmt stmt_arr;
    bool  had_error;
};

Parser parser_init(const char* text); // NOTE: create its own Lexer
void   parser_parse(Parser* parser);
Evaluation parser_evalueate_expression(Expr* expr);






