#ifndef SEMANTIC_H
#define SEMANTIC_H
#include <assert.h>
#include  "node.h"
enum varType{
	varInt,varFloat,varArray,varStruct
};

struct argList{
	enum varType aType;
	char name[32];
	struct array *varArray;
	struct argList *next;
	char structName[32];
	int defineOrVar;//0 is define,1 is var
};

struct array{
	int size;
	enum varType vType;
	struct array *next;
};

struct varMes{
	enum varType vType;
	struct array *vArray;
	int deminsion;
};

struct funMes{
	enum varType fType;
	int argNum;
	struct argList *arg;
};

union mesFV{
	struct varMes *vmes;
	struct funMes *fmes;
};


struct symbol{
	char name[32];
	int funcOrVar;//0 is func,1 is variable
	int lineNumber;
	union mesFV message;
};

struct structure{
	char name[32];		
	struct argList *structMember;
	struct structure *next;
	int flag;//flag 0 is OptTag, 1 is Tag
	int varOrStruct;//0 is var,1 is struct
	int sOE;//0 is specifier,1 is ExtDecList
	int depth;
	int fail;//0 is live,1 is death
};

struct singleSymbol{
	struct symbol sym;
	struct singleSymbol *next;
	char structName[32];
	struct argList *structMember;
	int depth;
	int fail;//0 is live,1 is death
};

//struct func

extern void init();
extern void semantic();
//Hign-level Definitions
extern void Program(struct Node *node);
extern void ExtDefList(struct Node *node);
extern void ExtDef(struct Node *node);
extern void ExtDecList(struct Node *node,enum varType type,struct structure *singleStruct);
//Specifiers
extern void Specifier(struct Node *node,enum varType *type,struct structure *singleStruct);
extern void StructSpecifier(struct Node *node,enum varType *type,struct structure *singleStruct);
extern void OptTag(struct Node *node,struct structure *singleStruct);
extern void Tag(struct Node *node,struct structure *singleStruct);
//Declarators
extern void VarDec(struct Node *node,enum varType type,struct array *vArray,struct structure *singleStruct);
extern void FunDec(struct Node *node,enum varType type);
extern struct argList* VarList(struct Node *node);
extern struct argList* ParamDec(struct Node *node);
//Statements
extern void CompSt(struct Node *node,enum varType type);
extern void StmtList(struct Node *node,enum varType type);
extern void Stmt(struct Node *node,enum varType type);

//Local Definitions
extern void DefList(struct Node *node,struct structure *singleStruct);
extern void Def(struct Node *node,struct structure *singleStruct);
extern void DecList(struct Node *node,enum varType type,struct structure *singleStruct);
extern void Dec(struct Node *node,enum varType type,struct structure *singleStruct);

//Expressions
extern void Exp(struct Node *node,enum varType type,enum varType *newType,int *dimension,struct structure *singleStruct);
extern struct argList* Args(struct Node *node,enum varType type);

//myself functions
extern unsigned int getHash(char *name);
extern enum varType getType(struct Node *node);
extern enum varType getTypeOfID(struct Node *node);
extern char *varTypeToType(enum varType type);
extern void newVar(struct Node *node,enum varType type,struct array *vArray,struct structure *singleStruct);
extern void newFun(struct Node *node,enum varType type,struct argList *arg,int argNum);
extern void printSTable();
extern void printStructTable();
#endif
