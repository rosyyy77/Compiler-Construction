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
 * This file implements some lexical analysis rules to scan the input file 
 * and return the tokens which will be used for parser 
 */

%{
/* C/C++ Stuff (headers, declarations, variables, etc.) */
#include <cstdio>
#include <string>
#include <vector>
#include "parser.hpp"
#include "node.hpp"

int yyerror (char *s);

int yylex();

static int eof_sent = 0;

/**
 * scan_only indicates whehter the flex scanner should print out the token information for debug.
 * The token information is in the following form:
 *   <token-class, lexeme>
 */
extern int scan_only;

void printToken(const char *tokenClass, const char *lexeme) {
    if(scan_only) {
        printf("%s %s\n", tokenClass, lexeme);
    }
}
%}

/* TODO: define some regular expressions here */

%%

[ \t]	; // ignore all whitespace
"--"[^\n]*\n ;
"begin"     { printToken("BEGIN_", yytext); return BEGIN_; }
"end"       { printToken("END", yytext); return END; }
"read"      { printToken("READ", yytext); return READ; }
"write"     { printToken("WRITE", yytext); return WRITE; }
"("         { printToken("LPAREN", yytext); return LPAREN; }
")"         { printToken("RPAREN", yytext); return RPAREN; }
";"         { printToken("SEMICOLON", yytext); return SEMICOLON; }
","         { printToken("COMMA", yytext); return COMMA; }
":="        { printToken("ASSIGNOP", yytext); return ASSIGNOP; }
"+"         { printToken("PLUSOP", yytext); return PLUSOP; }
"-"         { printToken("MINUSOP", yytext); return MINUSOP; }
[a-zA-Z][a-zA-Z0-9_]{0,31}        { yylval.strval = strdup(yytext); printToken("ID", yytext); return ID; }
-?[0-9]+    { yylval.strval = strdup(yytext); printToken("INTLITERAL", yytext); return INTLITERAL; }
<<EOF>>     {   if (!eof_sent) {
                    printToken("SCANEOF", "EOF");
                    eof_sent = 1;
                    return SCANEOF;
                } else {
                    return 0;
                }
            }
      

%%

int yywrap (void) {return 1;}
