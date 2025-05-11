#pragma once
#include "Lexer.h"
#include "my_String.h"
#include "Array.h"

// ==================================
typedef enum Expr_type Expr_type;

typedef struct Primary Primary;
typedef struct Unary   Unary;
typedef struct Binary  Binary;
typedef struct Expr    Expr;
typedef struct Var_const_decl    Var_const_decl;
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

        String identifier;
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

// TODO: why do i have 2 forawrd declarations, up top and here
typedef enum   Stmt_type            Stmt_type; 
typedef struct Stmt_expr            Stmt_expr;
typedef struct Stmt_print           Stmt_print; 
typedef struct Stmt_var_decl  Stmt_var_decl; 


enum Stmt_type {
    Stmt_type_expr,
    Stmt_type_print,
    Stmt_type_declaration,
};

struct Stmt_expr {
    Expr* expr;
};

struct Stmt_print {
    Expr* expr;
};

struct Stmt_var_decl {
    Token var_name;
    Expr* init_expr;
};

struct Stmt {
    Stmt_type type;
    union {
        Stmt_expr  stmt_expr;
        Stmt_print print;
        Stmt_var_decl var_decl;
    } union_;
};

Stmt print_stmt(Lexer* lexer);
Stmt expression_stmt(Lexer* lexer);
Stmt statement(Lexer* lexer);
Stmt var_declaration(Lexer* lexer);
Stmt declaration(Lexer* lexer);
Stmt program(Lexer* lexer);


// ========================================================================================

typedef enum {
    Evaluation_type_integer,
    Evaluation_type_boolean,
    Evaluation_type_absent,
} Evaluation_type;

struct Evaluation {
    Evaluation_type type;
    union {
        int  integer;
        bool boolean;
    } union_;
};
void evaluation_print(Evaluation* eval);


// ========================================================================================

struct Parser {
    Lexer lexer;
    Array stmt_arr;
    bool  had_error;
};

Parser parser_init(const char* text); // NOTE: create its own Lexer
void   parser_parse(Parser* parser);
// TODO: RALLY need a delete function for parser








