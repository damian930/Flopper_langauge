#pragma once
#include "Lexer.h"
#include "my_String.h"
#include "Array.h"




// ==================================
typedef enum Expr_type Expr_type;

typedef struct Primary Primary;
typedef struct Unary   Unary;
typedef struct Binary  Binary;
typedef struct Func_call Func_call;
typedef struct Expr    Expr;
typedef struct Var_const_decl    Var_const_decl;
// ==================================

// ==================================
typedef enum   Stmt_type            Stmt_type; 
typedef struct Stmt                 Stmt;
typedef struct Stmt_expr            Stmt_expr;
typedef struct Stmt_print           Stmt_print; 
typedef struct Stmt_var_decl        Stmt_var_decl; 
typedef struct Stmt_var_decl_auto   Stmt_var_decl_auto;
typedef struct Stmt_scope           Stmt_scope;
typedef struct Tuple__expr_scope    Tuple__expr_scope;
typedef struct Stmt_if              Stmt_if;
typedef struct For_loop_range       For_loop_range;
typedef struct Stmt_for_loop        Stmt_for_loop;       
typedef struct Stmt_while_loop      Stmt_while_loop;
typedef struct Stmt_var_assignment  Stmt_var_assignment;
//typedef struct Stmt_func_decl       Stmt_func_decl;
// ==================================

// ==================================
typedef enum   Evaluation_type Evaluation_type; 
typedef struct Evaluation      Evaluation; 
// ==================================

// ==================================
typedef struct Parser  Parser;
// ==================================

Array_create_type(Stmt, Stmt_array)
Array_create_type(Tuple__expr_scope, Expr_scope_array)

// ========================================================================================
// == Expressions 

enum Expr_type {
    Expr_type_primary,
    Expr_type_unary,
    Expr_type_binary,
};

// struct Func_call {
//     Token name;
//     Array args_as_expr_p; // Array of epxrs pointers
// };

struct Primary {
    Token_Type type;
    union {
        // NOTE: These are the supported default types for now
        int  integer;
        bool boolean;
        Token identifier;

        //Func_call func_call;
        // char* string;

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
};



struct Expr {
    Expr_type type;
    union {
        Primary   primary;
        Unary     unary;
        Binary    binary;
    } union_;
};

Expr* primary   (Lexer* lexer);
Expr* unary     (Lexer* lexer);
Expr* factor    (Lexer* lexer);
Expr* term      (Lexer* lexer);
Expr* comparison(Lexer* lexer);
Expr* equality  (Lexer* lexer);
Expr* expression(Lexer* lexer);

void   expr_delete   (Expr* expr);   
String expr_to_string(Expr* expr); // For debugging


// ========================================================================================
// == Statement 

enum Stmt_type {
    Stmt_type_expr,
    Stmt_type_print,
    Stmt_type_declaration,
    Stmt_type_declaration_auto,
    Stmt_type_var_assignment,
    Stmt_type_scope,
    Stmt_type_if,
    Stmt_type_for_loop,
    Stmt_type_while_loop,
    //Stmt_type_func_decl,
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

struct Stmt_var_assignment {
    Token var_name;
    Expr* assigment_expr;
};



struct Stmt_scope {
    Stmt_array statements;
};


// == Structured needed for Stmt_if to work
struct Tuple__expr_scope {
    Expr*      expr;
    Stmt_scope scope;
};

// Only after the struct is fully defined:
//Array_create_type (Tuple__expr_scope, Expr_scope_array);

// ============================================================

struct Stmt_if {
    Expr*      main_if_expr;
    Stmt_scope main_if_scope;

    Expr_scope_array expr_scope_arr;     
    
    bool       is_else_set;
    Stmt_scope else_scope;       
};


// == Structures needed for Stmt_for_loop to work
struct For_loop_range {
    Expr* start;
    Expr* end;
    Expr* increment;
    bool  include_end_value;
};
// ============================================================

struct Stmt_for_loop {
    For_loop_range range;
    Stmt_scope     scope; 
    Token     identifier;
};

struct Stmt_while_loop {
    Expr*  condition;
    Stmt_scope scope;    
};

// struct Stmt_func_decl {
//     Token name;
//     Stmt_scope scope;
//     Array arg_names_as_tokens;
//     Array arg_types_as_tokens;

//     // Array args_and_types_as_tokens;
//     // Array of var names and types
//     // i will need names to then create varibles. I need the types, so when i evaluate expression and pass them into the func exetions, i can see if those are valid types for the function. How do i store types
// };

struct Stmt {
    Stmt_type type;
    union {
        Stmt_expr           stmt_expr;
        Stmt_print          print;
        Stmt_var_decl       var_decl;
        Stmt_var_decl_auto  var_decl_auto;
        Stmt_var_assignment var_assignment;
        Stmt_scope          scope;
        Stmt_if             if_else;
        Stmt_for_loop       for_loop;
        Stmt_while_loop     while_loop;
        //Stmt_func_decl      func_decl;
    } union_;
};

Stmt print_stmt          (Lexer* lexer);
Stmt expression_stmt     (Lexer* lexer);
Stmt statement           (Lexer* lexer);
Stmt var_assignment      (Lexer* lexer);
Stmt var_declaration_auto(Lexer* lexer);
Stmt var_declaration     (Lexer* lexer);
Stmt if_condition        (Lexer* lexer);
Stmt for_loop            (Lexer* lexer);
Stmt while_loop          (Lexer* lexer);
Stmt declaration         (Lexer* lexer);
Stmt program             (Lexer* lexer);

void   stmt_delete   (Stmt* stmt);   
String stmt_to_string(Stmt* stmt); // For debugging


// ========================================================================================
// == Evaluation - wrapper over all primitive types to then return when executing 

typedef enum {
    Evaluation_type_integer,
    Evaluation_type_boolean,
    Evaluation_type_absent
} Evaluation_type;

struct Evaluation {
    Evaluation_type type;
    union {
        int   integer;
        bool  boolean;
        Token identifier;
    } union_;
};
void evaluation_print(Evaluation* eval); // For debugging


// ========================================================================================
// == Parser

struct Parser {
    Lexer      lexer;
    Stmt_array stmt_arr;
    //bool       had_error; // Is not yet supported
};

Parser parser_init  (const char* text); // NOTE: create its own Lexer
void   parser_parse (Parser* parser);
void   parser_delete(Parser* parser);

// void report_parsing_error()

Stmt stmt_scope_to_stmt(Stmt_scope* scope);

//typedef struct Stmt_array Stmt_array;

//Array_create_type (Stmt, Stmt_array);
