%{
	#include <stdio.h>
	#include <string.h>
	#include <stdarg.h>
	#include <stdlib.h>
	#include "lex.yy.c"
	
	struct Node *start;
	struct Node *buildTree(char *type,int count,...);
	int errorFlag;
	int errorPrintFlag;
	#define YYERROR_VERBOSE
%}

%union {struct Node *node;}
%token <node> INT FLOAT SEMI COMMA ID TYPE
%right <node> ASSIGNOP
%left <node> OR
%left <node> AND
%left <node> RELOP
%left <node> PLUS MINUS
%left <node> STAR DIV
%right <node> NOT
%left <node> DOT LB RB LP RP LC RC

%nonassoc <node> LOWER_THAN_ELSE
%nonassoc <node> IF ELSE WHILE STRUCT RETURN 

%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt
%type <node> DefList Def DecList Dec
%type <node> Exp Args 


%%
Program         : ExtDefList					{$$=buildTree("Program",1,$1);start=$$;}
	            ;
ExtDefList      : ExtDef ExtDefList				{$$=buildTree("ExtDefList",2,$1,$2);}
		        | /*empty*/						{$$=NULL;}
		        ;
ExtDef          : Specifier ExtDecList SEMI		{$$=buildTree("ExtDef",3,$1,$2,$3);}
                | Specifier FunDec CompSt		{$$=buildTree("ExtDef",3,$1,$2,$3);}
				| Specifier SEMI                {$$=buildTree("ExtDef",2,$1,$2);}
				;
ExtDecList      : VarDec 						{$$=buildTree("ExtDecList",1,$1);}
		        | VarDec COMMA ExtDecList		{$$=buildTree("ExtDecList",3,$1,$2,$3);}
				;
Specifier       : TYPE							{$$=buildTree("Specifier",1,$1);} 
		        | StructSpecifier				{$$=buildTree("Specifier",1,$1);}
                ;
StructSpecifier : STRUCT OptTag LC DefList RC	{$$=buildTree("StructSpecifier",5,$1,$2,$3,$4,$5);}
			    | STRUCT Tag					{$$=buildTree("StructSpecifier",2,$1,$2);}		
				;
OptTag          : ID							{$$=buildTree("OptTag",1,$1);}
	            | /*empty*/						{$$=NULL;}
	            ;
Tag             : ID							{$$=buildTree("Tag",1,$1);}
                ;
VarDec          : ID							{$$=buildTree("VarDec",1,$1);}
	            | VarDec LB INT RB				{$$=buildTree("VarDec",4,$1,$2,$3,$4);}
				;
FunDec          : ID LP VarList RP				{$$=buildTree("FunDec",4,$1,$2,$3,$4);}
	            | ID LP RP						{$$=buildTree("FunDec",3,$1,$2,$3);}
                | error RP						{errorFlag = 1;}
				; 
VarList         : ParamDec COMMA VarList		{$$=buildTree("VarList",3,$1,$2,$3);}
	            | ParamDec						{$$=buildTree("VarList",1,$1);}
				;
ParamDec        : Specifier VarDec				{$$=buildTree("ParamDec",2,$1,$2);}
		        | error COMMA                   {errorFlag=1;}
				| error RB                      {errorFlag=1;}
				;
CompSt          : LC DefList StmtList RC		{$$=buildTree("CompSt",4,$1,$2,$3,$4);}
				| error RC                      {errorFlag=1;}
				;
StmtList        : Stmt StmtList					{$$=buildTree("StmtList",2,$1,$2);}
		        | /*empty*/						{$$=NULL;}
		        ;
Stmt            : Exp SEMI						{$$=buildTree("Stmt",2,$1,$2);}
	            | CompSt						{$$=buildTree("Stmt",1,$1);}
                | RETURN Exp SEMI				{$$=buildTree("Stmt",3,$1,$2,$3);}
                | IF LP Exp RP Stmt	%prec LOWER_THAN_ELSE			{$$=buildTree("Stmt",5,$1,$2,$3,$4,$5);}
	            | IF LP Exp RP Stmt ELSE Stmt	{$$=buildTree("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
                | WHILE LP Exp RP Stmt			{$$=buildTree("Stmt",5,$1,$2,$3,$4,$5);}
				| error SEMI                    {errorFlag = 1;}
				| error WHILE  LP Exp RP	    {errorFlag = 1;}
				;
DefList         : Def DefList					{$$=buildTree("DefList",2,$1,$2);}
		        |/*empty*/						{$$=NULL;}
	            ;
Def             : Specifier DecList SEMI        {$$=buildTree("Def",3,$1,$2,$3);}
				| Specifier DecList error       {errorFlag = 1;}
				;
DecList         : Dec							{$$=buildTree("DecList",1,$1);}
	            | Dec COMMA DecList				{$$=buildTree("DecList",3,$1,$2,$3);}
	            ;
Dec             : VarDec						{$$=buildTree("Dec",1,$1);}
                | VarDec ASSIGNOP Exp			{$$=buildTree("Dec",3,$1,$2,$3);}
				;
Exp             : Exp ASSIGNOP Exp				{$$=buildTree("Exp",3,$1,$2,$3);}
                | Exp AND Exp					{$$=buildTree("Exp",3,$1,$2,$3);}
	            | Exp OR Exp					{$$=buildTree("Exp",3,$1,$2,$3);}
	            | Exp RELOP Exp					{$$=buildTree("Exp",3,$1,$2,$3);}
	            | Exp PLUS Exp					{$$=buildTree("Exp",3,$1,$2,$3);}
	            | Exp MINUS Exp					{$$=buildTree("Exp",3,$1,$2,$3);}
               	| Exp STAR Exp					{$$=buildTree("Exp",3,$1,$2,$3);}
				| Exp DIV Exp					{$$=buildTree("Exp",3,$1,$2,$3);}
				| LP Exp RP						{$$=buildTree("Exp",3,$1,$2,$3);}
				| MINUS Exp						{$$=buildTree("Exp",2,$1,$2);}
				| NOT Exp						{$$=buildTree("Exp",2,$1,$2);}
				| ID LP Args RP					{$$=buildTree("Exp",4,$1,$2,$3,$4);}
				| ID LP RP						{$$=buildTree("Exp",3,$1,$2,$3);}
				| Exp LB Exp RB					{$$=buildTree("Exp",4,$1,$2,$3,$4);}
				| Exp DOT ID					{$$=buildTree("Exp",3,$1,$2,$3);}
				| ID							{$$=buildTree("Exp",1,$1);}
				| INT							{$$=buildTree("Exp",1,$1);}
				| FLOAT							{$$=buildTree("Exp",1,$1);}
				;
Args			: Exp COMMA Args				{$$=buildTree("Args",3,$1,$2,$3);}
				| Exp							{$$=buildTree("Args",1,$1);}
				;
%%

yyerror(char* msg){
		fprintf(stderr,"Error type B at line %d:%s\n",yylineno,msg);
} 

struct Node *buildTree(char *type,int count,...){
	struct Node *root = (struct Node *)malloc(sizeof(struct Node));
	root->tokenFlag = 0;
	struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
	va_list nodeBool;
	va_start (nodeBool,count);
	temp = va_arg(nodeBool,struct Node*);
	if(temp==NULL)	
		return NULL;
	strcpy(root->type,type);
	root->line = temp->line;
	root->child = temp;
	temp->parent = root;
	int i;
	for(i=1;i<count;i++){
		temp->sibling = va_arg(nodeBool,struct Node*);
		if(temp->sibling!=NULL)
			temp = temp->sibling;
	}
	temp->sibling = NULL;
	va_end(nodeBool);
	return root;
}

void printNodeTree(struct Node *root,int depth){
	if(root == NULL) return;
	int i;
	for(i=0;i<depth;i++)
		printf("  ");
	if(root->tokenFlag){
		if((strcmp(root->type,"INT")==0)|(strcmp(root->type,"TYPE")==0)|
			(strcmp(root->type,"ID")==0))
			printf("%s: %s\n",root->type,root->text);
		else if(strcmp(root->type,"FLOAT")==0){
			int len = strlen(root->text);
			int temp = 1;
			int i;
			for(i=0;i<len;i++){
				if(root->text[i]=='E'||root->text[i]=='e')				
					temp = 0;
			}
			if(temp)
				printf("%s: %lf\n",root->type,atof(root->text));
			else printf("%s: %lf\n",root->type,strtod(root->text,NULL));
		}
		else printf("%s\n",root->type);
	}	
	else {
		printf("%s (%d)\n",root->type,root->line);
		printNodeTree(root->child,depth+1);
	}	
	printNodeTree(root->sibling,depth);
}
