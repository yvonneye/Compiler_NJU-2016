#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "semantic.h"
#include<assert.h>
#define tableSize 5000
extern struct Node *start;

struct singleSymbol *sTable[tableSize];
struct singleSymbol *fTable[tableSize];
struct structure *structTable[tableSize];
int depth;
void init(){
	memset(sTable,0,sizeof(struct singleSymbol *)*tableSize);
	memset(fTable,0,sizeof(struct singleSymbol *)*tableSize);
	memset(structTable,0,sizeof(struct structure *)*tableSize);
	depth = 0;
}

void semantic(){
	init();
	Program(start);
//	printSTable();
//	printStructTable();
}

void printSTable(){
	int i = 0;
	for(;i<tableSize;i++){
		if(sTable[i] != NULL){
			struct singleSymbol *temp = sTable[i];
			while(temp != NULL){
				printf("%s %d %d\n",temp->sym.name,temp->sym.message.vmes->vType,i);
				
				if(temp->sym.message.vmes->vArray != NULL){
					struct array *s = temp->sym.message.vmes->vArray;
					while(s != NULL){
						printf("array:%d , %d\n",s->size,s->vType);
						s = s->next;
					}
				}
				if(temp->sym.message.vmes->vType == 3){
					struct argList *tempList = temp->structMember;
					while(tempList != NULL){
						printf(" %s , %d\n",tempList->name,tempList->aType);
					tempList = tempList->next;
					}
				}
				temp = temp->next;
			}
		}
	}
}

void printStructTable(){
	int i = 0;
	for(;i<tableSize;i++){
		if(structTable[i] != NULL){
			struct structure *temp = structTable[i];
			while(temp != NULL){
			printf("name:%s\n",temp->name);
			if(temp->structMember != NULL){
				struct argList *s  = temp->structMember;
				while(s != NULL){
					printf(" %s , %d\n",s->name,s->aType);
					s = s->next;
				}
			}
			printf("\n");
			temp = temp->next;
			}
		}
	}
}
// High-level Definitions
void Program(struct Node *node){
	if(node == NULL){
		printf("Pogram node is null\n");
		return;
	}
	if(strcmp(node->type,"Program") != 0){
		printf("The node is not a program\n");
		return;
	}
	ExtDefList(node->child);	
	
}

void ExtDefList(struct Node *node){
	if(node == NULL){
		return;
	}
	if(strcmp(node->type,"ExtDefList") != 0){
		printf("The node is not a ExtDefList\n");
		return;
	}
	if(node->child == NULL)
		return;
	struct Node *child = node->child;
	ExtDef(child);
	struct Node *sibling = child->sibling;
	ExtDefList(sibling);
}

void ExtDef(struct Node *node){
	if(node == NULL){
	   printf("The node ExtDef is null\n");
	}
	if (strcmp(node->type,"ExtDef") != 0){
		printf("The node is not a ExtDef\n");
		return;
	}
	struct Node *child = node->child;
	enum varType type;
	struct structure *singleStruct = (struct structure *)malloc(sizeof(struct structure));
	singleStruct->varOrStruct = 0;
	Specifier(child,&type,singleStruct);
	child = child->sibling;
	singleStruct->sOE = 0;
	if(strcmp(child->type,"ExtDecList") == 0){
		singleStruct->sOE = 1;
		ExtDecList(child,type,singleStruct);
	}	
	else if (strcmp(child->type,"SEMI") == 0)
			//do nothing
		;
	else if (strcmp(child->type,"FunDec") == 0){
		FunDec(child,type);
		struct Node *sibling = child->sibling;
		CompSt(sibling,type);
	}
}

void ExtDecList(struct Node *node,enum varType type,struct structure* singleStruct){
	if(node == NULL){
		printf("The node is ExtDecList is null\n");
		return;
	}
	if(strcmp(node->type,"ExtDecList") != 0){
	   printf("The node is not a ExtDecList\n");	
	   return;
	}
	struct Node *child = node->child;
	struct array *vArray = NULL;
	VarDec(child,type,vArray,singleStruct);
	struct Node *sibling = child->sibling;
	if(sibling != NULL)
		ExtDecList(sibling->sibling,type,singleStruct);

}

//Specifiers
void Specifier(struct Node *node,enum varType *type,struct structure *singleStruct) {
	if(node == NULL || strcmp(node->type,"Specifier") != 0) {
	   printf("The node is not a Specifier\n");	
	   return;
	}
	struct Node *child = node->child;
	if(strcmp(child->type,"TYPE")==0){
			*type = getType(child);
		return;
	}
	else if(strcmp(child->type,"StructSpecifier") == 0){
		enum varType t;
		StructSpecifier(child,&t,singleStruct);
		*type = t;
		return;
	}
}

void StructSpecifier(struct Node *node,enum varType *type,struct structure *singleStruct){
	if(node == NULL || strcmp(node->type,"StructSpecifier") != 0){
		printf("The node is not a StructSpecifier\n");
		return;
	}
	singleStruct->varOrStruct = 1;
	*type = 3;
	struct Node *child = node->child;
	child = child->sibling;
	if(child->sibling != NULL){
		singleStruct->flag = 0;
		if(strcmp(node->child->sibling->type,"OptTag") ==0)
			child = child->sibling->sibling;
		else child = child->sibling;
		depth = depth + 1;
		DefList(child,singleStruct);
		if(strcmp(node->child->sibling->type,"OptTag") ==0)
			OptTag(node->child->sibling,singleStruct);
		singleStruct->varOrStruct = 0;
			int i = 0;
		for(;i<tableSize;i++){
			if(sTable[i] != NULL){
				struct singleSymbol *temp = sTable[i];
				while(temp != NULL){
					if(temp->depth == depth)
						temp->fail = 1;		
					temp = temp->next;
				}
			}
		}
		depth = depth -1;


	}
	else { 
		singleStruct->flag = 1;
		Tag(child,singleStruct);
	}
}

void Tag(struct Node *node,struct structure *Struct){
	struct Node *child = node->child;
	strcpy(Struct->name,child->text);
}

void OptTag(struct Node *node,struct structure *Struct){	
	if(strcmp(node->type,"OptTag") != 0){
		printf("The node is not a OPtTag\n");
		return;

	}
	struct Node *child = node->child;
	char name[32];
	if(node->child != NULL)
		strcpy(name,node->child->text);
	else {
		strcpy(name,"");
		return;
	}
	unsigned int structHash = getHash(child->text);
	struct structure *temp = structTable[structHash];
	while(temp != NULL){
		if(strcmp(temp->name,name)==0){
			printf("Error type 16 at line %d: Duplicated name \"%s\"\n",child->line,child->text);
			return;
		}
		temp = temp->next;
	}
	strcpy(Struct->name,name);
	if(structTable[structHash] ==NULL)
		structTable[structHash] = Struct;
	else {
		Struct->next = structTable[structHash];
		structTable[structHash] = Struct;
	}
}

//Declarators
void VarDec(struct Node *node,enum varType type,struct array *vArray,struct structure *singleStruct){
	if(node == NULL || strcmp(node->type,"VarDec") != 0){
		printf("The node is not a VarDec\n");
		return;
	}
	struct Node *child = node->child;
	//todo if struct dec is array
		if(strcmp(child->type,"ID")==0){
			if(vArray != NULL){
				struct array *p = vArray;
				while(p->next != NULL){
					p = p->next;
				}
				p->vType = type;
			}
			newVar(child,type,vArray,singleStruct);
			
		}
		else if(strcmp(child->type,"VarDec")==0){
			//type = 2;
			if(vArray == NULL){
				struct array *newArray = (struct array*)malloc(sizeof(struct array));
				newArray->vType = 2;
				newArray->size = atoi(child->sibling->sibling->text);
				newArray->next = NULL;
				vArray = newArray;
				VarDec(child,type,vArray,singleStruct);
			}
			else {
				struct array *newArray = (struct array*)malloc(sizeof(struct array));
				newArray->vType = 2;
				newArray->size = atoi(child->sibling->sibling->text);
				newArray->next = vArray;
				vArray = newArray;
				VarDec(child,type,vArray,singleStruct);
			}	
		}
}

void FunDec(struct Node *node,enum varType type){
	if(node == NULL || strcmp(node->type,"FunDec") != 0){
		printf("The nod is not a FunDec\n");
		return;
	}
	struct Node *child = node->child;
	if(strcmp(child->type,"ID")==0){
		struct argList *arg;
		int num = 0;
		if(strcmp(child->sibling->sibling->type,"VarList")==0){
			arg = VarList(child->sibling->sibling);
			struct argList *temp = arg;
			while(temp!=NULL){
				num++;
				temp = temp->next;
			}
			newFun(child,type,arg,num);
		}
		else {
			arg = NULL;
			newFun(child,type,arg,num);
		}

	}
}

struct argList* VarList(struct Node *node){
	if(node == NULL || strcmp(node->type,"VarList") != 0){
		printf("The node is not a VarList,is %s\n",node->type);
		return NULL;
	}
	struct Node *child = node->child;
	struct argList *p =(struct argList *)malloc(sizeof(struct argList));
	struct argList *temp=ParamDec(child);
	p->next=NULL;
	strcpy(p->name,temp->name);
	p->aType = temp->aType;
	child = child->sibling;
	if(child != NULL)
		p->next = VarList(child->sibling);
	return p;
}

struct argList* ParamDec(struct Node *node){
	if(node == NULL || strcmp(node->type,"ParamDec") != 0){
		printf("The node is not a ParamDec\n");
		return NULL;
	}
	//array haven't been dealt with 
	struct argList *temp =(struct argList *)malloc(sizeof(struct argList));
	enum varType type;
	struct Node *child = node->child;
	//maybe have errors
	struct structure *singleStruct = (struct structure *)malloc(sizeof(struct structure));
	singleStruct->varOrStruct = 0;
	Specifier(child,&type,singleStruct);
	child = child->sibling;
	struct array *vArray = NULL;
	VarDec(child,type,vArray,singleStruct);
	strcpy(temp->name,child->child->text);
	temp->aType = type;
	return temp;
}

//Statements
void CompSt(struct Node *node,enum varType type){
	if(node == NULL || strcmp(node->type,"CompSt") != 0){
		return;
	}
	struct Node *child = node->child;
	assert(strcmp(child->type,"LC")==0);
	depth = depth + 1;
	child = child->sibling;
	struct structure *singleStruct = (struct structure *)malloc(sizeof(struct structure));
	if(strcmp(child->type,"DefList")==0){
		DefList(child,singleStruct);
		child = child->sibling;
		StmtList(child,type);
	}
	else 
		StmtList(child,type);
	int i = 0;
	for(;i<tableSize;i++){
		if(sTable[i] != NULL){
			struct singleSymbol *temp = sTable[i];
			while(temp != NULL){
				if(temp->depth == depth)
					temp->fail = 1;		
				temp = temp->next;
			}
		}
	}
	depth = depth -1;
}

void StmtList(struct Node *node,enum varType type){
	if(node == NULL || (strcmp(node->type,"StmtList") != 0)){
		return;
	}
	struct Node *child = node->child;
	Stmt(child,type);
	StmtList(child->sibling,type);
}

void Stmt(struct Node *node,enum varType type){
	if (node == NULL || strcmp(node->type,"Stmt") != 0){
		printf("The node is not a Stmt\n");
		return;
	}
	struct Node *child = node->child;
	if(strcmp(child->type,"Exp") == 0){
		enum varType newType;
		int num = 0;
		struct structure *singleStruct = (struct structure*)malloc(sizeof(struct structure));
		singleStruct->sOE = 0;
		Exp(child,type,&newType,&num,singleStruct);
	}
	else if(strcmp(child->type,"CompSt") == 0){
		CompSt(child,type);
	}
	else if(strcmp(child->type,"RETURN") == 0){
		child = child->sibling;
		enum varType newType;
		int num = 0;
		struct structure *singleStruct = (struct structure*)malloc(sizeof(struct structure));
		singleStruct->sOE = 0;
		Exp(child,type,&newType,&num,singleStruct);
		//printf("%d %d",type,newType);
		if(type != newType)
			printf("Error type 8 at Line %d:Type mismatched for return\n",child->line);

	}
	else if(strcmp(child->type,"IF") == 0){
		child = child->sibling;
		if(child == NULL || strcmp(child->type,"LP") != 0){
			printf("The node is not a LP\n");	
			
			return;
		}
		child = child->sibling;
		enum varType newType;
		int num = 0;
		struct structure *singleStruct = (struct structure*)malloc(sizeof(struct structure));
		singleStruct->sOE = 3;
		Exp(child,type,&newType,&num,singleStruct);
		if(newType != 0){
			printf("Error type 7 at Line %d:Type mismatched for operands\n",child->line);
		}
		child = child->sibling;
		if(child == NULL || strcmp(child->type,"RP") != 0){
			printf("The node is not a RP\n");	
			return;
		}
		child = child->sibling;
		Stmt(child,type);
		if(child->sibling != NULL){
			Stmt(child->sibling->sibling,type);
		}
	}
	else if(strcmp(child->type,"WHILE") == 0){
		child = child->sibling;//LP
		child = child->sibling;//Exp	
		enum varType newType;
		int num = 0;
		struct structure *singleStruct = (struct structure*)malloc(sizeof(struct structure));
		singleStruct->sOE = 2;
		Exp(child,type,&newType,&num,singleStruct);
		if(newType != 0){
			printf("Error type 7 at Line %d:Type mismatched for operands\n",child->line);
		}
		child = child->sibling;//RP
		child = child->sibling;//Stmt
		Stmt(child,type);
	}
}

//Local Definitions
void DefList(struct Node *node,struct structure *singleStruct){
	if(node == NULL || strcmp(node->type,"DefList") != 0){
	//	printf("The node is not a DefList");
		return;
	}
	struct Node *child = node->child;
	if(child == NULL)//do nothing
	{
		return;
		
	}
	else {
		Def(child,singleStruct);
		DefList(child->sibling,singleStruct);
	}
}

void Def(struct Node *node,struct structure *singleStruct){
	if(node == NULL || strcmp(node->type,"Def") != 0){
		printf("The node is not a Def\n");
		return;
	}
	struct Node *child = node->child;
	enum varType type;
	Specifier(child,&type,singleStruct);
	child = child->sibling;
	DecList(child,type,singleStruct);
}


void DecList(struct Node *node,enum varType type,struct structure *singleStruct){
	if(node == NULL || strcmp(node->type,"DecList") != 0){
		printf("The node is not a DecList\n");
		return;
	}
	struct Node *child = node->child;
	Dec(child,type,singleStruct);
	if(child->sibling != NULL){
		DecList(child->sibling->sibling,type,singleStruct);
	}
}

void Dec(struct Node *node,enum varType type,struct structure *singleStruct){
	if(node == NULL || strcmp(node->type,"Dec") != 0){
		printf("The node is not a Dec\n");
		return;
	}
	struct Node *child = node->child;
	struct array *vArray = NULL;
					//printf("14\n");
	VarDec(child,type,vArray,singleStruct);
					//printf("15\n");
	if(vArray != NULL)
		type = 2;
	if(child->sibling != NULL){
		if(singleStruct->varOrStruct == 1)
			printf("Error type 15 at Line %d:Redefined field or initialize variable \"%s\"\n",node->line,child->child->text);
		struct Node *sibling = child->sibling->sibling;
		enum varType newType;
		int num = 0;
		//todo
		Exp(sibling,type,&newType,&num,singleStruct);
		//printf("%d %d",type,newType);
		if(type != newType)
			printf("Error type 5 at Line %d:Type mismatched for assignment\n",child->line);
			return ;
	//	int flag = 0;
	//	if(strcmp(child->type,"ID") == 0)
	//		flag = 1;
		//other two kinds
	//	if(flag == 0)
	//		printf("Error type 6 at Line %d:The left-hand side of an assignment must be a variable\n",child->line);
	}
}

//Expressions
void Exp(struct Node *node,enum varType type,enum varType *newType,int *dimension,struct structure* singleStruct){
	if(node == NULL || strcmp(node->type,"Exp") != 0){
		printf("The node is not a Exp\n");
		return;
	}
	struct Node *child = node->child;
	if(strcmp(child->type,"Exp") == 0){
		struct Node *sibling = child->sibling;
		if(strcmp(sibling->type,"ASSIGNOP")==0){
			int flag = 0;
			if(strcmp(child->child->type,"ID")==0&&child->child->sibling == NULL)
				flag = 1;
			struct Node *p = child->child;
			if((strcmp(p->type,"Exp") == 0) && (strcmp(p->sibling->type,"LB") == 0)
					&& (strcmp(p->sibling->sibling->type,"Exp") == 0)
					&& (strcmp(p->sibling->sibling->sibling->type,"RB") == 0))
				flag = 1;
			if((strcmp(p->type,"Exp") == 0) && (strcmp(p->sibling->type,"DOT") == 0)
					&& (strcmp(p->sibling->sibling->type,"ID") == 0))
				flag = 1;
			if(flag == 0){
				printf("Error type 6 at Line %d:The left-hand side of an assignment must be a variable\n",child->line);
				return;
			}
			int numLeft = 0;
			int numRight = 0;
			enum varType leftType;
			enum varType rightType;
			char nameLeft[32];
			char nameRight[32];
			Exp(child,type,&leftType,&numLeft,singleStruct);
			if(leftType == 3)
				strcpy(nameLeft,singleStruct->name);
			Exp(child->sibling->sibling,type,&rightType,&numRight,singleStruct);
			if(rightType == 3)
				strcpy(nameRight,singleStruct->name);
			//todo:judge struct is assigno
			//printf("%d %d\n ",rightType,leftType);
			if(rightType != leftType){
				printf("Error type 5 at Line %d:Type mismatched for assignment\n",child->line);
				return;
			}
			if(leftType == 3){
				struct singleSymbol *tempLeft = sTable[getHash(nameLeft)];
				while(tempLeft != NULL){
					if(strcmp(nameLeft,tempLeft->sym.name) == 0)
						break;
					tempLeft = tempLeft->next;
				}
				struct singleSymbol *tempRight = sTable[getHash(nameRight)];
				while(tempRight != NULL){
					if(strcmp(nameRight,tempRight->sym.name) == 0)
						break;
					tempRight = tempRight->next;
				}
				if(tempRight != NULL && tempLeft != NULL){
				   	if(strcmp(tempRight->structName,tempLeft->structName) != 0)
						printf("Error type 5 at Line %d:Type mismatched for assignment\n",child->line);
				}
			}
		}
		else if((strcmp(sibling->type,"AND")==0)||(strcmp(sibling->type,"OR")==0)||
			(strcmp(sibling->type,"RELOP")==0)||(strcmp(sibling->type,"PLUS")==0)||
			(strcmp(sibling->type,"MINUS")==0)||(strcmp(sibling->type,"STAR")==0)||
			(strcmp(sibling->type,"DIV")==0)){
			enum varType leftType;
			enum varType rightType;
			int numLeft = 0;
			int numRight = 0;
			Exp(child,type,&leftType,&numLeft,singleStruct);
			Exp(child->sibling->sibling,type,&rightType,&numRight,singleStruct);
			*newType = leftType;
			if(rightType != leftType){
				printf("Error type 7 at Line %d:Type mismatched for operands\n",child->line);
			}
		}
		
		else if(strcmp(sibling->type,"LB") == 0){
			enum varType temp;
			*dimension = *dimension + 1;
			//printf("%d\n",*dimension);
			Exp(child,type,&temp,dimension,singleStruct);
			if(temp != 2)
				printf("Error type 10 at Line %d:\"%s\"is not an array\n",child->line,child->child->text);
			*newType = 2;
			char tempName[32];
			strcpy(tempName,singleStruct->name);
			Exp(child->sibling->sibling,type,&temp,dimension,singleStruct);	
			//unsigned int hashNumber = getHash(tempName);
			//printf("%d\n",hashNumber);		
			//struct singleSymbol *ss = sTable[hashNumber];
			//while(ss != NULL){
			//	if(strcmp(tempName,ss->sym.name)==0)
			//		break;
			//	ss = ss->next;
			//}			
			//printf("===%s===\n",ss->structName);
			//*newType = ss->sym.message.vmes->vArray->vType;

			*newType = temp;
		}
		else if(strcmp(sibling->type,"DOT") == 0){
			enum varType newType1;
			Exp(child,type,&newType1,dimension,singleStruct);
			char varName[32];
			strcpy(varName,singleStruct->name);
			unsigned int hashVar = getHash(child->child->text);
			struct singleSymbol *var = sTable[hashVar];
			char structName[32];
			while(var != NULL){
				if(strcmp(child->child->text,var->sym.name)==0){
					strcpy(structName,var->structName);
					break;
				}
				var = var->next;
			}
			if(newType1 != 3){
				if(newType1 != 2)
					printf("Error type 13 at Line %d:Illegal use of \".\"\n",child->line);
				return;
			}
			int structTemp = 0;
			int number = 0;
			int nullName = 0;
			if(strcmp(structName,"") != 0)
				number = getHash(structName);
			else nullName = 1;
			struct structure *nowStruct = structTable[number];
			struct argList *t = (struct argList*)malloc(sizeof(struct argList));
			while(nowStruct != NULL){
				if(strcmp(structName,nowStruct->name)==0){
					t = nowStruct->structMember;
					break;
				}
				nowStruct = nowStruct->next;
			}
			while(t != NULL){
				if(strcmp(child->sibling->sibling->text,t->name)==0){
					structTemp = 1;
					break;
				}
				t = t->next;
			}
			if(structTemp == 0 && nullName != 1){
				printf("Error type 14 at Line %d:Non-existent field \"%s\"\n",child->line,sibling->sibling->text);
				return;
			}
	//		if(nullName == 1){
	//			unsigned int nullNum = getHash(varName);
	//			struct singleSymbol *nullSymbol= sTable[nullNum];	
	//			while(nullSymbol != NULL){
	//				if(strcmp(varName,nullSymbol->sym.name) == 0){
	//					break;
	//				}
	//				nullSymbol = nullSymbol->next;
	//			}
	//			struct argList *t = (struct argList*)malloc(sizeof(struct argList));
	//			t = nullSymbol->structMember;
	//		}
			*newType =  getTypeOfID(sibling->sibling);
			//printf("%d\n",*newType);
		}
		// not and minus exp not deal with
	
	}
	else if (strcmp(child->type,"MINUS") == 0){
			Exp(child->sibling,type,newType,dimension,singleStruct);
	}
	else if(strcmp(child->type,"ID") == 0){
		strcpy(singleStruct->name,child->text);
		if(child->sibling == NULL){
			unsigned int hashVal = getHash(child->text);	
			struct singleSymbol *temp = sTable[hashVal];
			int  flag = 0;
			while(temp != NULL){
				if(strcmp(child->text,temp->sym.name)==0)
					flag = 1;
				temp = temp->next;
			}
			*newType = getTypeOfID(child);
			//if(*newType == 3)
			//todo			
			if(flag == 0)
				printf("Error type 1 at Line %d:Undefined variable'%s'\n",child->line,child->text);
			return;
		}
		else {
			//may be some errors needed to be tested
			unsigned int hashFun = getHash(child->text);
			struct singleSymbol *temp = fTable[hashFun];
			struct singleSymbol *savedFunc = fTable[hashFun];
			int flag = 0;
			while(temp != NULL){
				if(strcmp(child->text,temp->sym.name)==0){
					*newType = temp->sym.message.fmes->fType;
					flag = 1;
					savedFunc = temp;
				}
				temp = temp->next;
			}
			if(flag == 0){
				temp = sTable[hashFun];
				while(temp != NULL) {
					if(strcmp(child->text,temp->sym.name)==0) {
						printf("Error type 11 at Line %d:\"%s\" is not a function\n",child->line,child->text);
						return;
					}
					temp = temp->next;
				}
				if(flag == 0){
					printf("Error type 2 at Line %d:Undefined function'%s'\n",child->line,child->text);
					return;
				}
			}
			if(strcmp(child->sibling->sibling->type,"Args") == 0){
				struct argList * arg = Args(child->sibling->sibling,type);
				struct argList * par = savedFunc->sym.message.fmes->arg;
				int flag = 0;
				int num1 = 0;
				int num2 = 0;
				while(arg != NULL){
					num1 ++;
					arg = arg->next;
				}
				arg = Args(child->sibling->sibling,type);
				while( par != NULL){
					num2 ++;
					par = par->next;
				}
				par = savedFunc->sym.message.fmes->arg;
				if(num1 != num2)
					flag = 1;
				else {
					while(arg != NULL){
						if(arg->aType != par->aType)
							flag = 1;
						arg = arg->next;
						par = par->next;
					}
				}
				if(flag == 1){
					printf("Error type 9 at Line %d:Function \"(",child->line);
					arg = Args(child->sibling->sibling,type);
					while(arg != NULL){
						if(arg->next != NULL)
							printf("%s ",varTypeToType(arg->aType));
						else 
							printf("%s",varTypeToType(arg->aType));
						arg = arg->next;
					}
					printf(")\" is not applicable for arguments(\"");
					par = savedFunc->sym.message.fmes->arg;
					while(par != NULL){
						if(par->next != NULL)
							printf("%s ",varTypeToType(par->aType));
						else 
							printf("%s",varTypeToType(par->aType));
						par = par->next;
					}
					printf("\")\n");
				}
			}
			else if(strcmp(child->sibling->sibling->type,"RP") == 0){
				if(savedFunc->sym.message.fmes->argNum != 0) {
					printf("Error type 9 at Line %d:Function \"(",child->line);
					struct argList *arg = savedFunc->sym.message.fmes->arg;
					while(arg != NULL){
						if(arg->next != NULL)
							printf("%s ",varTypeToType(arg->aType));
						else 
							printf("%s",varTypeToType(arg->aType));
						arg = arg->next;
					}
					printf(")\" is not applicable for arguments()\n");
				}
				if(strcmp(child->sibling->sibling->type,"Args") == 0){
				}
			}
		}
	}
	else if(strcmp(child->type,"INT") == 0){
		if(*newType != 2)
			*newType = 0;
	}
	else if(strcmp(child->type,"FLOAT") == 0){
		if(*newType == 2)
			printf("Error type 12 at Line %d: \"%s\" is not an integer\n",child->line,child->text);
		else
			*newType = 1;
	}

}

struct argList* Args(struct Node *node,enum varType type){
	struct Node *child = node->child;
	enum varType argType;
	int *dimension;
	struct structure *singleStruct = (struct structure *)malloc(sizeof(struct structure));
   	singleStruct->sOE = 0;
	Exp(child,type,&argType,dimension,singleStruct);
	struct argList *args = (struct argList *)malloc(sizeof(struct argList));
	args->aType = argType;
	args->next = NULL;
	if(child->sibling != NULL)
		args->next = Args(child->sibling->sibling,type);
	return args;
}

//
unsigned int getHash(char *name){
	unsigned int val = 0,i;
	for(;*name;++name){
		val = (val << 2) + *name;
		if(i = val & ~0x1388)
		val = (val^(i >> 12)) & 0x1388;
	}
	return val;
}

enum varType getTypeOfID(struct Node* node){
	unsigned int hashVal = getHash(node->text);	
	struct singleSymbol *temp = sTable[hashVal];
	while(temp != NULL){
		if(strcmp(node->text,temp->sym.name)==0)
			return temp->sym.message.vmes->vType;	
		temp = temp->next;
	}
}

enum varType getType(struct Node* node){
		if(strcmp(node->text,"int") == 0)
			return varInt;
		else if(strcmp(node->text,"float") == 0)
			return varFloat;
}

char *varTypeToType(enum varType type){
	char *s = (char *)malloc(32*sizeof(char));
	switch(type){
		case 0 : strcpy(s,"int");break;
		case 1 : strcpy(s,"float");break;
		case 2 : strcpy(s,"array");break;
		case 3 : strcpy(s,"struct");break;
	//	default:strcpy(s,"error");break;
	}
	return s;

}

void newVar(struct Node *node,enum varType type,struct array *vArray,struct structure *singleStruct){
	unsigned int hashVal = getHash(node->text);	
	struct singleSymbol *temp = sTable[hashVal];
	while(temp != NULL){
		if(strcmp(node->text,temp->sym.name)==0&&temp->fail == 0&&depth<=temp->depth){
			//test weather is redefined field
			if(singleStruct->varOrStruct == 1){
				struct argList *structList = singleStruct->structMember;
				while(structList != NULL){
					if(strcmp(node->text,structList->name) == 0) {
						printf("Error type 15 at Line %d:Redefined field \"%s\"\n",node->line,node->text);
						return;
					}
					structList = structList->next;
				}

			}
			printf("Error type 3 at Line %d:Redefined variable '%s',old define at line %d\n",node->line,node->text,temp->sym.lineNumber);
			return;
		}
		temp = temp->next;
	}
	if( (type == 0) || (type == 1)){
		unsigned int tempVal = getHash(node->text);
		struct structure *tempStruct = structTable[tempVal];
		while(tempStruct != NULL){
			if(strcmp(node->text,tempStruct->name) == 0){
				printf("Error type 3 at Line %d:Redefined variable '%s'\n",node->line,node->text);
				return;
			}
			tempStruct = tempStruct->next;
		}

	}
	if(type == 3 && singleStruct->flag == 1){
		unsigned int hashStruct = 0;
		if(strcmp(singleStruct->name,"") != 0);
			 hashStruct = getHash(singleStruct->name);
		struct structure *s = structTable[hashStruct];
		int structCount = 0;
		while(s != NULL){
			if(strcmp(singleStruct->name,s->name)==0)
				structCount = 1;
			s = s->next;
		}
		if(structCount == 0){
			printf("Error type 17 at Line %d:Undefined structure \"%s\"\n",node->line,singleStruct->name);
			return;
		}
	}
	struct singleSymbol *newSymbol = (struct singleSymbol *)malloc(sizeof(struct singleSymbol));
	strcpy(newSymbol->sym.name,node->text);
	newSymbol->sym.lineNumber = node->line;
	newSymbol->depth = depth;
	newSymbol->fail = 0;
	newSymbol->fail = 0;
	newSymbol->sym.funcOrVar =1;
	newSymbol->sym.message.vmes = malloc(sizeof(struct varMes));
	if(vArray != NULL){
		newSymbol->sym.message.vmes->vArray = vArray;
		newSymbol->sym.message.vmes->vType = 2;
		type = 2;
		struct array *temp;
		temp = vArray;
		int count = 0;
		while(temp != NULL){
			count ++;
			temp = temp->next;
		}
		newSymbol->sym.message.vmes->deminsion = count;
		//printf("%d",newSymbol->sym.message.vmes->deminsion);
	}
	newSymbol->sym.message.vmes->vType = type;
	newSymbol->next = NULL;
	if(type == 3){
		strcpy(newSymbol->structName,singleStruct->name);
		 if(singleStruct->sOE == 1){
			newSymbol->structMember = singleStruct->structMember;
			struct argList *tempList = singleStruct->structMember;
					while(tempList != NULL){
			//			printf(" %s , %d\n",tempList->name,tempList->aType);
						tempList = tempList->next;
					}
		 }
	}
	if(sTable[hashVal]==NULL)
		sTable[hashVal] = newSymbol;
	else {
		newSymbol->next = sTable[hashVal];
		sTable[hashVal] = newSymbol;
	}
	if(singleStruct->varOrStruct == 1&&singleStruct->sOE ==0){
		struct argList *temp = (struct argList *)malloc(sizeof(struct argList));
		temp->aType = type;
		if(type == 2 || type == 4)
			temp->varArray = vArray;
		strcpy(temp->name,node->text);	
		temp->next = NULL;
		if(singleStruct->structMember == NULL){
			singleStruct->structMember = (struct argList *)malloc(sizeof(struct argList));
			singleStruct->structMember = temp;
			return;
		}
		struct argList *p = singleStruct->structMember;
		while(p->next != NULL)
			p = p->next;
		p->next = temp;
	}
}

void newFun(struct Node *node,enum varType type,struct argList *arg,int argNum){
	unsigned int hashFun = getHash(node->text);
	struct singleSymbol *temp = fTable[hashFun];
	while(temp!=NULL){
		if(strcmp(node->text,temp->sym.name)==0){
			printf("Error type 4 at Line %d:Redefined function '%s',old define at line %d\n",node->line,node->text,temp->sym.lineNumber);
			return;
		}
		temp = temp->next;
	}
	struct singleSymbol *newFunc = (struct singleSymbol *)malloc(sizeof(struct singleSymbol));
	strcpy(newFunc->sym.name,node->text);
	newFunc->sym.lineNumber = node->line;
	newFunc->sym.funcOrVar = 0;
	newFunc->fail = 0;
	newFunc->depth = 1;
	newFunc->sym.message.fmes = malloc(sizeof(struct funMes));
	newFunc->sym.message.fmes->fType = type;
	newFunc->sym.message.fmes->argNum = argNum;
	newFunc->sym.message.fmes->arg = arg;
	newFunc->next = NULL;
	//fTable[hashFun] = newFunc;
	if(fTable[hashFun]==NULL)
		fTable[hashFun] = newFunc;
	else {
		newFunc->next = fTable[hashFun];
		fTable[hashFun] = newFunc;
	}
}
