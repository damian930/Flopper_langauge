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
typedef enum   Stmt_type           Stmt_type; 
typedef struct Stmt                Stmt;
typedef struct Stmt_expr           Stmt_expr;
typedef struct Stmt_print          Stmt_print; 
typedef struct Stmt_var_decl       Stmt_var_decl; 
typedef struct Stmt_var_decl_auto  Stmt_var_decl_auto;
typedef struct Stmt_scope          Stmt_scope;
typedef struct Tuple__expr_scope   Tuple__expr_scope; 
typedef struct Stmt_if             Stmt_if;
typedef struct For_loop_range      For_loop_range;
typedef struct Stmt_for_loop       Stmt_for_loop;       
// ==================================

// ==================================
typedef enum Evaluation_type Evaluation_type; 
typedef struct Evaluation Evaluation; 
// ==================================

// ==================================
typedef struct Parser     Parser;
// ==================================

// ========================================================================================
// == Expressions 

enum Expr_type {
    Expr_type_primary,
    Expr_type_unary,
    Expr_type_binary,
};

struct Primary {
    Token_Type type;
    union {
        // NOTE: These are the supported default types for now
        int  integer;
        bool boolean;
        // char* string;

        String identifier;
    } union_;
};

struct Unary {
    Token operator; // Token is here intesd of Token_Type as in Primary for error printing with column and line info
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
        Unary   unary;
        Binary  binary;
    } union_;
} ;

Expr* primary   (Lexer* lexer);
Expr* unary     (Lexer* lexer);
Expr* factor    (Lexer* lexer);
Expr* term      (Lexer* lexer);
Expr* comparison(Lexer* lexer);
Expr* equality  (Lexer* lexer);
Expr* expression(Lexer* lexer);

void   epxr_delete   (Expr* expr);   
String expr_to_string(Expr* expr); // For debugging


// ========================================================================================
// == Statement 

enum Stmt_type {
    Stmt_type_expr,
    Stmt_type_print,
    Stmt_type_declaration,
    Stmt_type_declaration_auto,
    Stmt_type_scope,
    Stmt_type_if,
    Stmt_type_for_loop,
};

struct Stmt_expr {
    Expr* expr;
};

struct Stmt_print {
    Expr* expr;
};

struct Stmt_var_decl {
    Token var_name;
    Token var_specified_type;
    Expr* init_expr;
};

struct Stmt_var_decl_auto {
    Token var_name;
    Expr* init_expr;
};

struct Stmt_scope {
    Array statements;
};

// == Structured needed for Stmt_if to work
struct Tuple__expr_scope {
    Expr*      expr;
    Stmt*      scope;
};
// Deletion should be done manually 
// ======================================

// TODO: maybe rename the scope to statements, since scope is a statement also

// TODO: hate having Stmt inside Stmt_if as pointers. make the flow of statements way worse
struct Stmt_if {
    Expr* main_if_expr;
    Stmt* main_if_scope;

    Array expr_scope_tuples;     
    Stmt* else_scope;        // Scope sctruct. It will have a zero length array of statements if not found
};
// TODO: dont forget to then delete these dynamic statements

// == Structured needed for Stmt_for_loop to work
// struct For_loop_range {
//     s32 start;
//     s32 end;
//     s32 increment;
// };
// // ======================================

// struct Stmt_for_loop {
//     For_loop_range range;
//     Stmt_scope     scope;
//     Token     identifier;
// };

struct Stmt {
    Stmt_type type;
    union {
        Stmt_expr          stmt_expr;
        Stmt_print         print;
        Stmt_var_decl      var_decl;
        Stmt_var_decl_auto var_decl_auto;
        Stmt_scope         scope;
        Stmt_if            if_else;
        //Stmt_for_loop      for_loop;
    } union_;
};

Stmt print_stmt          (Lexer* lexer);
Stmt expression_stmt     (Lexer* lexer);
Stmt statement           (Lexer* lexer);
Stmt var_declaration_auto(Lexer* lexer);
Stmt var_declaration     (Lexer* lexer);
Stmt if_condition        (Lexer* lexer);
Stmt for_loop            (Lexer* lexer);
Stmt declaration         (Lexer* lexer);
Stmt program             (Lexer* lexer);

void   stmt_delete   (Stmt* stmt);   
String stmt_to_string(Stmt* stmt); // For debugging


// ========================================================================================
// == Evaluation - wrapper over all primitive types to then return when executing 

typedef enum {
    Evaluation_type_integer,
    Evaluation_type_boolean,
    Evaluation_type_absent,  // NOTE: absent return from variables hash map, 
                             //       to then be handled at execution level.
} Evaluation_type;

struct Evaluation {
    Evaluation_type type;
    union {
        int  integer;
        bool boolean;
    } union_;
};
void evaluation_print(Evaluation* eval); // For debugging


// ========================================================================================
// == Parser

struct Parser {
    Lexer lexer;
    Array stmt_arr;
    bool  had_error;
};

Parser parser_init  (const char* text); // NOTE: create its own Lexer
void   parser_parse (Parser* parser);
void   parser_delete(Parser* parser);









