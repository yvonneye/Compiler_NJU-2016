#include <stdio.h>
#include "semantic.h"
extern FILE* yyin;
extern struct Node* start;
extern int errorFlag;
extern int yylineno;
extern int errorPrintFlag;
int main(int argc,char** argv){
	if(argc<=1)
		return 1;
	FILE* f = fopen(argv[1],"r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}
	errorFlag = 0;
	yylineno = 1;
	errorPrintFlag = 1;
	yyrestart(f);
	yyparse();
	//if(start != NULL&&errorFlag==0)
	//	printNodeTree(start,0);
	semantic();
	return 0;
}
