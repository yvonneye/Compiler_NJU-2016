#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Operand *operand;
typedef struct InterCode *intercode;
typedef struct InterCodes *intercodes;
typedef enum {TEMPVAR,VARIABLE,CONSTANT,REFERENCE,ADDRESS,LABEL,FUNCTION,PARAM} kind1;
typedef enum {ASSIGN_K,ADD_K,SUB_K,MUL_K,DIV_K,RETURN_K,LABEL_K,GOTO_K,IFGOTO_K,DEC_K,ARG_K,CALL_K,PARAM_K,READ_K,WRITE_K,FUNCTION_K} kind2;
typedef enum {EQ,NE,MT,LT,ME,LE} relop_type;

struct Operand  {
	kind1 kind;
	union {
		int var_no;
		int tmp_no;
		int value;
		int addr_no;
		int label_no;
		char *func_name;
		char *param_name;
	} u;
	operand next;
};

struct InterCode {
	kind2 kind;
	union {
		struct { operand op; } one; //return,label,goto,read,write,arg,function,param
		struct { operand right,left;} assign; // call,at,assign
		struct { operand result,op1,op2;} binop;// add,sub,mul,div
		struct { 
			operand c1,c2,label;
			relop_type reltype;
		} triop;//if_goto_
	} u;
};

struct InterCodes {
	intercode code;
	intercodes prev;
	intercodes next;
};

extern intercodes head,tail;
extern int tmp_count,label_count;

//extern void init();
//extern void intermediate();
extern void add_code(intercodes code);
//extern void delete_code(intercodes code);
extern void print_code(char *name);
extern void print_op(operand op,FILE *fp);
extern void print_relop_type(relop_type type,FILE *fp);
extern operand new_tmp();
extern operand new_tmp_id(int id);
extern operand new_var(int id);
extern operand new_constant(int value);
extern operand new_reference(int id);
extern operand new_label();
extern operand new_function(char *func_name);
extern operand new_param(char *param_name);
extern intercodes gen_assign(kind2 kind,operand left,operand right);
extern intercodes gen_binop(kind2 kind,operand result,operand op1,operand op2);
extern intercodes gen_one(kind2 kind,operand op);
extern intercodes gen_triop(relop_type type,operand c1,operand c2,operand label);
extern intercodes translate_Exp(struct Node *node,operand place);
extern intercodes translate_Cond(struct Node *node,operand label_true,operand label_false);
extern intercodes translate_Stmt(struct Node *node);
extern intercodes translate_CompSt(struct Node *node);
extern intercodes translate_StmtList(struct Node *node);
extern intercodes translate_VarDec(struct Node *node,operand *place);
extern intercodes translate_DefList(struct Node *node);
extern intercodes translate_Def(struct Node *node);
extern intercodes translate_DecList(struct Node *node);
extern intercodes translate_Dec(struct Node *node);
extern intercodes translate_FunDec(struct Node *node);
extern intercodes translate_VarList(struct Node *node);
extern intercodes translate_ParamDec(struct Node *node);
extern intercodes translate_ExtDef(struct Node *node);
extern intercodes translate_ExtDecList(struct Node *node);
extern intercodes translate_Args(struct Node *node,operand *arg_listi,int *arg_num);
extern intercodes translate_array(struct Node *node,operand place,union varp *list,enum varType *listtype);






extern intercodes link(intercodes l1,intercodes l2);
extern relop_type get_relop(struct Node *node);
extern int get_structure_size(union varp vp,enum varType type);

#endif
