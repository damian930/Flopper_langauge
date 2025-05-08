#pragma once
#include "Ast.h"
#include "Lexer.h"
#include "my_String.h"

typedef enum Expr_type    Expr_type;

typedef struct Primary Primary;
typedef struct Unary   Unary;
typedef struct Binary  Binary;
typedef struct Expr    Expr;

enum Expr_type {
    Expr_type_primary,
    Expr_type_unary,
    Expr_type_binary,
};

struct Primary {
    Token_Type type;
    union {
        int integer;
        char* string;
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

String expr_to_string(Expr* expr);


/*{	
	String str = string_init("flopper");
	string_print(&str);

	string_add_c_string(&str, " money");
	string_print(&str);

	String other = string_init(" peter peter peter peter peter");
	string_add_string(&str, &other);
	string_print(&str);
	string_delete(&other);

	string_print(&other);

	other = string_init("other");
	string_print(&other);
	string_delete(&other);

	string_delete(&str);

	string_print(&str);
	string_print(&other);
}*/




