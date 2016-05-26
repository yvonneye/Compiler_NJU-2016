#include <stdio.h>
#include "semantic.h"
#include "intermediate.h"
extern FILE* yyin;
extern struct Node* start;
extern int errorFlag;
extern int yylineno;
extern int errorPrintFlag;
//extern intercodes ir;
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
	FILE*f1 = fopen(argv[2],"w");
	//if(start != NULL&&errorFlag==0)
	//printNodeTree(start,0);
	semantic();
	print_code(argv[2]);
	fclose(f);
	fclose(f1);
	return 0;
}
