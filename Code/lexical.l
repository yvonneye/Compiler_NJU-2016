%option yylineno

%{ 
	#include <stdio.h>
	#include <string.h>
	#include "syntax.tab.h"
	#include "node.h"	
//	struct Node{
//		int tokenFlag;
//		int line;
//		char type[32];
//		char text[32];
//		struct Node *child;
//		struct Node *sibling;
//		struct Node *parent;
//	};
	extern int errorFlag;
	void foundNode(char *type,char *text);
%}

DELIM [ \t\n\r]
WS {DELIM}+
LETTER [A-Za-z_]
DIGIT [0-9]
INT [-]?[0]|[1-9][0-9]*
FLOAT ([0-9]+\.[0-9]+)|(([0-9]+\.[0-9]+)|([0-9]+\.)|(\.[0-9]+))?[eE]([\+\-]?[0-9]+)
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE (int)|(float)
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
ID ({LETTER})({LETTER}|{DIGIT})*
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while

%%
{WS} 
{TYPE}		{foundNode("TYPE",yytext);return TYPE;}
{INT}		{foundNode("INT",yytext);return INT;}
{FLOAT}		{foundNode("FLOAT",yytext);return FLOAT;}
{SEMI}		{foundNode("SEMI",yytext);return SEMI;}
{COMMA}		{foundNode("COMMA",yytext);return COMMA;}
{ASSIGNOP}  {foundNode("ASSIGNOP",yytext);return ASSIGNOP;}
{RELOP}		{foundNode("RELOP",yytext);return RELOP;}
{PLUS}		{foundNode("PLUS",yytext);return PLUS;}
{MINUS}		{foundNode("MINUS",yytext);return MINUS;}
{STAR}		{foundNode("STAR",yytext);return STAR;}
{DIV}		{foundNode("DIV",yytext);return DIV;}
{AND}		{foundNode("AND",yytext);return AND;}
{OR}		{foundNode("OR",yytext);return OR;}
{DOT}		{foundNode("DOT",yytext);return DOT;}
{NOT}		{foundNode("NOT",yytext);return NOT;}
{LP}		{foundNode("LP",yytext);return LP;}
{RP}		{foundNode("RP",yytext);return RP;}
{LB}		{foundNode("LB",yytext);return LB;}
{RB}		{foundNode("RB",yytext);return RB;}
{LC}		{foundNode("LC",yytext);return LC;}
{RC}		{foundNode("RC",yytext);return RC;}
{STRUCT}	{foundNode("STRUCT",yytext);return STRUCT;}
{RETURN}	{foundNode("RETURN",yytext);return RETURN;}
{IF}		{foundNode("IF",yytext);return IF;}
{ELSE}		{foundNode("ELSE",yytext);return ELSE;}
{WHILE}		{foundNode("WHILE",yytext);return WHILE;}
{ID}		{foundNode("ID",yytext);return ID;}
.			{errorFlag = 1;printf("Error type A at line %d: Mysterious character %s\n",yylineno,yytext);}

%%

void foundNode(char *type,char *text){
	yylval.node = (struct Node*)malloc(sizeof(struct Node));
	yylval.node->line = yylineno;
	yylval.node->tokenFlag = 1;
	strcpy(yylval.node->type,type);
	strcpy(yylval.node->text,text);
	yylval.node->child = NULL;
	yylval.node->sibling = NULL;
	yylval.node->parent = NULL;
}
