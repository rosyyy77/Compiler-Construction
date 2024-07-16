/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 1: Micro Language Compiler
 * --------------------------------------
 * Author: Mr.Liu Yuxuan
 * Position: Teaching Assisant
 * Date: January 25th, 2024
 * Email: yuxuanliu1@link.cuhk.edu.cn
 * 
 * This file implements some syntax analysis rules and works as a parser
 * The grammar tree is generated based on the rules and MIPS Code is generated
 * based on the grammar tree and the added structures and functions implemented
 * in File: added_structure_function.c
 */

%{
/* C declarations used in actions */
#include <cstdio>     
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <ctype.h>

#include "node.hpp"

int yyerror (char *s);

int yylex();

extern int cst_only;

struct Node* root_node = nullptr;
%}

// TODO: define yylval data types with %union
%union{
    struct Node* node;
	char* strval;
}
// TODO: define terminal symbols with %token. Remember to set the type.
%token BEGIN_ END READ WRITE LPAREN RPAREN SEMICOLON COMMA ASSIGNOP SCANEOF 
%left PLUSOP MINUSOP
%token <strval> ID INTLITERAL

// Start Symbol
%start start

// TODO: define Non-Terminal Symbols with %type. Remember to set the type.
%type <node> program statement_list statement id_list expr_list expression primary

%%
/**
 * Format:
 * Non-Terminal  :  [Non-Terminal, Terminal]+ (production rule 1)   { parser actions in C++ }
 *                  | [Non-Terminal, Terminal]+ (production rule 2) { parser actions in C++ }
 *                  ;
 */


// TODO: your production rule here
// The tree generation logic should be in the operation block of each production rule

start
    : program SCANEOF { 
        if (cst_only==1){
            root_node = new Node(SymbolClass::START, "start");
            root_node->append_child($1);
            root_node->append_child(new Node(SymbolClass::SCANEOF, "SCANEOF"));
        } else {

        }
      }

program
    : BEGIN_ statement_list END {
        Node* programNode = new Node(SymbolClass::PROGRAM, "program");
        if (cst_only == 1){
            // cst-only: preserve BEGIN_ and END
            programNode->append_child(new Node(SymbolClass::BEGIN_, "begin"));
            programNode->append_child($2);  // statement_list
            programNode->append_child(new Node(SymbolClass::END, "end"));
            $$ = programNode;
        } else {
            // ast: program is the root_node
            programNode->append_child($2);  
            root_node = programNode;  
        }
    }
    ;

statement_list
    : statement {
        Node* thisNode=new Node(SymbolClass::STATEMENT_LIST, "statement_list");
        $$=thisNode;
        thisNode -> append_child($1);
    }
    | statement_list statement {
        if (cst_only != 1) {
            $1->append_child($2);
            $$ = $1;
        } else {
            Node* thisNode=new Node(SymbolClass::STATEMENT_LIST, "statement_list");
            $$=thisNode;
            thisNode -> append_child($1);
            thisNode -> append_child($2);
        }
    }
    ;


statement
    : ID ASSIGNOP expression SEMICOLON {
        if (cst_only!=1){
            Node* thisNode=new Node(SymbolClass::ASSIGNOP,":=");
            $$=thisNode;
            thisNode -> append_child(new Node(SymbolClass::ID, $1));
            thisNode -> append_child($3);
        } else{
            Node* thisNode=new Node(SymbolClass::STATEMENT, "statement");
            $$=thisNode;
            thisNode -> append_child(new Node(SymbolClass::ID, $1));
            thisNode -> append_child(new Node(SymbolClass::ASSIGNOP, ":="));
            thisNode -> append_child($3);
            thisNode -> append_child(new Node(SymbolClass::SEMICOLON, ";"));
        }
    }
    | READ LPAREN id_list RPAREN SEMICOLON {
        if(cst_only!=1){
            Node* thisNode = new Node(SymbolClass::READ, "read");
            for (auto child : $3->children) {
                thisNode->append_child(child);
            }
            $$=thisNode;
        } else{
            Node* thisNode=new Node(SymbolClass::STATEMENT, "statement");
            $$=thisNode;
            thisNode->append_child(new Node(SymbolClass::READ, "read"));
            thisNode->append_child(new Node(SymbolClass::LPAREN,"("));
            thisNode->append_child($3);
            thisNode->append_child(new Node(SymbolClass::RPAREN,")"));
            thisNode->append_child(new Node(SymbolClass::SEMICOLON,";"));
        }
    }
    | WRITE LPAREN expr_list RPAREN SEMICOLON {
        if(cst_only!=1){
            Node* thisNode = new Node(SymbolClass::WRITE, "write");
            for (auto child : $3->children) {
                thisNode->append_child(child);
            }
            $$=thisNode;
        } else{
            Node* thisNode=new Node(SymbolClass::STATEMENT, "statement");
            $$=thisNode;
            thisNode->append_child(new Node(SymbolClass::WRITE,"write"));
            thisNode->append_child(new Node(SymbolClass::LPAREN,"("));
            thisNode->append_child($3);
            thisNode->append_child(new Node(SymbolClass::RPAREN,")"));
            thisNode->append_child(new Node(SymbolClass::SEMICOLON,";"));        
        }
    }
    ;

id_list
    : ID {
        if(cst_only!=1){
            Node* thisNode = new Node(SymbolClass::ID_LIST);
            thisNode->append_child(new Node(SymbolClass::ID,$1));
            $$=thisNode;
        } else{
            Node* thisNode=new Node(SymbolClass::ID_LIST);
            $$=thisNode;
            thisNode->append_child(new Node(SymbolClass::ID,$1));
        }
    }
    | id_list COMMA ID {
        if(cst_only!=1){
            $1->append_child(new Node(SymbolClass::ID,$3));
            $$ = $1;
        } else{
            Node* thisNode=new Node(SymbolClass::ID_LIST);
            $$=thisNode;
            thisNode->append_child($1);
            thisNode->append_child(new Node(SymbolClass::COMMA));
            thisNode->append_child(new Node(SymbolClass::ID,$3)); 
        }
    }
    ;

expr_list
    : expression {
        if(cst_only!=1){
            Node* thisNode = new Node(SymbolClass::EXPRESSION_LIST);
            thisNode->append_child($1);
            $$=thisNode;
        } else {
            Node* thisNode=new Node(SymbolClass::EXPRESSION_LIST);
            $$=thisNode;
            thisNode->append_child($1);
        }
    }
    | expr_list COMMA expression {
        if(cst_only!=1){
            $$=$1;
            $$->append_child($3);
        } else{
            Node* thisNode=new Node(SymbolClass::EXPRESSION_LIST);
            $$=thisNode;
            thisNode->append_child($1);
            thisNode->append_child(new Node(SymbolClass::COMMA));
            thisNode->append_child($3);
        }
    }
    ;

expression
    : primary {
        if(cst_only!=1){
            $$ = $1;
        } else{
            Node* thisNode=new Node(SymbolClass::EXPRESSION);
            $$=thisNode;
            thisNode->append_child($1);
        }
    }
    | expression PLUSOP primary{
        if(cst_only!=1){
            Node* thisNode=new Node(SymbolClass::PLUSOP,"+");
            thisNode->append_child($1);
            thisNode->append_child($3);
            $$=thisNode;
        }else{
            Node* thisNode=new Node(SymbolClass::EXPRESSION);
            $$=thisNode;
            thisNode->append_child($1);
            thisNode->append_child(new Node(SymbolClass::PLUSOP,"+"));
            thisNode->append_child($3);
        }
    }
    | expression MINUSOP primary{
        if(cst_only!=1){
            Node* thisNode=new Node(SymbolClass::MINUSOP,"-");
            thisNode->append_child($1);
            thisNode->append_child($3);
            $$=thisNode;
        }else{
            Node* thisNode=new Node(SymbolClass::EXPRESSION);
            $$=thisNode;
            thisNode->append_child($1);
            thisNode->append_child(new Node(SymbolClass::MINUSOP,"-"));
            thisNode->append_child($3);
        }
    }
    ;

primary
    : LPAREN expression RPAREN {
        if (cst_only!=1){
            $$ = $2;
        } else{
            Node* thisNode=new Node(SymbolClass::PRIMARY);
            $$=thisNode;
            thisNode->append_child(new Node(SymbolClass::LPAREN));
            thisNode->append_child($2);
            thisNode->append_child(new Node(SymbolClass::RPAREN));
        }
    }
    | ID {
        if (cst_only!=1){
            Node *thisNode=new Node(SymbolClass::ID, $1);
            $$ = thisNode;
        } else {
            Node* thisNode=new Node(SymbolClass::PRIMARY);
            $$=thisNode;
            thisNode->append_child(new Node(SymbolClass::ID,$1));
        }
    }
    | INTLITERAL{
        if (cst_only!=1){
            Node* thisNode=new Node(SymbolClass::INTLITERAL, $1);
            $$=thisNode;
        } else {
            Node* thisNode=new Node(SymbolClass::PRIMARY);
            $$=thisNode;
            thisNode->append_child(new Node(SymbolClass::INTLITERAL,$1));
        }
    }
    ;


%%

int yyerror(char *s) {
	printf("Syntax Error on line %s\n", s);
	return 0;
}
