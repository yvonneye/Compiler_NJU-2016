#include <stdio.h>
#include <assert.h>
#include "semantic.h"
#include "intermediate.h"
extern struct Node *start;
int tmp_count = 0;
int label_count = 0;
intercodes head = NULL;
intercodes tail = NULL;

void add_code(intercodes code) {
	printf("add %d\n",code -> code -> kind);
	if (head == NULL) {
		head = code;
		tail = code;
		code->prev = code;
		code->next = code;
	}
	else {
		tail->next = code;
		code->prev = tail;
		code->next = head;
		tail = code;
		head->prev = tail;
	}
}


//print codes
void print_code(char* name) {
	FILE *fp = fopen(name,"w");
	if (fp == NULL)  {
		printf("creating error\n");
		return;
	}
    start = start -> child -> child;
	head = translate_ExtDef(start);
	intercodes mark = head;
	tail -> next = NULL;
//	printf("%d\n",tail->prev->code->kind);
	while (mark != NULL) {
		switch (mark->code->kind) {
			case ASSIGN_K:
				print_op(mark->code->u.assign.left,fp);
				fputs(" := ",fp);
				print_op(mark->code->u.assign.right,fp);
				fputs("\n",fp);
				break;
			case ADD_K:
				print_op(mark->code->u.binop.result,fp);
				fputs(" := ",fp);
				print_op(mark->code->u.binop.op1,fp);
				fputs(" + ",fp);
				print_op(mark->code->u.binop.op2,fp);
				fputs("\n",fp);
				break;
			case SUB_K:
				print_op(mark->code->u.binop.result,fp);
				fputs(" := ",fp);
				print_op(mark->code->u.binop.op1,fp);
				fputs(" - ",fp);
				print_op(mark->code->u.binop.op2,fp);
				fputs("\n",fp);
				break;
			case MUL_K:
				print_op(mark->code->u.binop.result,fp);
				fputs(" := ",fp);
				print_op(mark->code->u.binop.op1,fp);
				fputs(" * ",fp);
				print_op(mark->code->u.binop.op2,fp);
				fputs("\n",fp);
				break;
			case DIV_K:
				print_op(mark->code->u.binop.result,fp);
				fputs(" := ",fp);
				print_op(mark->code->u.binop.op1,fp);
				fputs(" / ",fp);
				print_op(mark->code->u.binop.op2,fp);
				fputs("\n",fp);
				break;
			case RETURN_K:
				fputs("RETURN ",fp);
				print_op(mark->code->u.one.op,fp);
				fputs("\n",fp);
				break;
			case LABEL_K:
				fputs("LABEL ",fp);
				print_op(mark->code->u.one.op,fp);
				fputs(" :\n",fp);
				break;
			case GOTO_K:
				fputs("GOTO	",fp);
				print_op(mark->code->u.one.op,fp);
				fputs("\n",fp);
				break;
			case IFGOTO_K:
				fputs("IF ",fp);
				print_op(mark->code->u.triop.c1,fp);
				print_relop_type(mark->code->u.triop.reltype,fp);
				print_op(mark->code->u.triop.c2,fp);
				fputs("GOTO	",fp);
				print_op(mark->code->u.triop.label,fp);
				fputs("\n",fp);
				break;
			case DEC_K:
				fputs("DEC ",fp);
				print_op(mark->code->u.assign.left,fp);
				fputs(" ",fp);
				fprintf(fp,"%d",mark->code->u.assign.right->u.value);
				fputs("\n",fp);
				break;
			case ARG_K:
				fputs("ARG ",fp);
				print_op(mark->code->u.one.op,fp);
				fputs("\n",fp);
				break;
			case CALL_K:
				print_op(mark->code->u.assign.left,fp);
				fputs(" := CALL ",fp);
				print_op(mark->code->u.assign.right,fp);
				fputs("\n",fp);
				break;
			case PARAM_K:
				fputs("PARAM ",fp);
				print_op(mark->code->u.one.op,fp);
				fputs("\n",fp);
				break;
			case READ_K:
				fputs("READ	",fp);
				print_op(mark->code->u.one.op,fp);
				fputs("\n",fp);
				break;
			case WRITE_K:
				fputs("WRITE ",fp);
				print_op(mark->code->u.one.op,fp);
				fputs("\n",fp);
				break;
			case FUNCTION_K:
				fputs("FUNCTION ",fp);
				print_op(mark->code->u.one.op,fp);
				fputs(" :\n",fp);
				break;
		}
		mark = mark->next;
	}
	fclose(fp);
}

void print_op(operand op,FILE *fp) {
	if (op == NULL)  	return;
	switch (op->kind) {
		case TEMPVAR:
			fprintf(fp,"t%d",op->u.tmp_no);
			break;
		case VARIABLE:
			fprintf(fp,"v%d",op->u.var_no);
			break;
		case CONSTANT:
			fprintf(fp,"#%d",op->u.value);
			break;
		case REFERENCE:
			fprintf(fp,"&v%d",op->u.var_no);
			break;
		case ADDRESS:
			fprintf(fp,"*t%d",op->u.tmp_no);
			break;
		case LABEL:
			fprintf(fp,"label%d",op->u.label_no);
			break;
		case FUNCTION:
			fprintf(fp,"%s",op->u.func_name);
			break;
		case PARAM:
			fprintf(fp,"%s",op->u.param_name);
			break;
	}
}

void print_relop_type(relop_type type,FILE *fp) {
	switch (type) {
		case EQ:
			fputs(" == ",fp);
			break;
		case NE:
			fputs(" != ",fp);
			break;
		case MT:
			fputs(" > ",fp);
			break;
		case LT:
			fputs(" < ",fp);
			break;
		case ME:
			fputs(" >= ",fp);
			break;
		case LE:
			fputs(" <= ",fp);
			break;
	}
}


//generate
operand new_tmp() {
	operand op = (operand)malloc(sizeof(struct Operand));
	op->kind = TEMPVAR;
	op->u.tmp_no = tmp_count++;
	return op;
}

operand new_tmp_id(int id) {
	operand op = (operand)malloc(sizeof(struct Operand));
	op->kind = TEMPVAR;
	op->u.tmp_no = id;
	return op;
}

operand new_var(int id) {
	operand op = (operand)malloc(sizeof(struct Operand));
	op->kind = VARIABLE;
	op->u.var_no = id;
	return op;
}

operand new_constant(int value) {
	operand op = (operand)malloc(sizeof(struct Operand));
	op->kind = CONSTANT;
	op->u.value = value;
	return op;
}

operand new_reference(int id) {
	operand op = (operand)malloc(sizeof(struct Operand));
	op->kind = REFERENCE;
	op->u.addr_no = id;
	return op;
}

operand new_label() {
	operand op = (operand)malloc(sizeof(struct Operand));
	op->kind = LABEL;
	op->u.label_no = label_count++;
	return op;
}

operand new_function(char *func_name) {
	operand op = (operand)malloc(sizeof(struct Operand));
	op->kind = FUNCTION;
	op->u.func_name = func_name;
	return op;
}

operand new_param(char *param_name) {
 	operand op = (operand)malloc(sizeof(struct Operand));
	op->kind = PARAM;
	op->u.param_name = param_name;
	return op;
}

intercodes gen_assign(kind2 kind,operand left,operand right) {
	intercodes ic = (intercodes)malloc(sizeof(struct InterCodes));
	ic->code = (intercode)malloc(sizeof(struct InterCode));
	ic->code->kind = kind;
	ic->code->u.assign.left = left;
	ic->code->u.assign.right = right;
	add_code(ic);
	return ic;
}

intercodes gen_binop(kind2 kind,operand result,operand op1,operand op2) {
	intercodes ic = (intercodes)malloc(sizeof(struct InterCodes));
	ic->prev = ic->next = NULL;
	ic->code = (intercode)malloc(sizeof(struct InterCode));
	ic->code->kind = kind;
	ic->code->u.binop.result = result;
	ic->code->u.binop.op1 = op1;
	ic->code->u.binop.op2 = op2;
	add_code(ic);
	return ic;
}

intercodes gen_one(kind2 kind,operand op) {
	intercodes ic = (intercodes)malloc(sizeof(struct InterCodes));
	ic->prev = ic->next = NULL;
	ic->code = (intercode)malloc(sizeof(struct InterCode));
	ic->code->kind = kind;
//	printf("%d\n",kind);
	ic->code->u.one.op = op;
	add_code(ic);
	return ic;
}

intercodes gen_triop(relop_type type,operand c1,operand c2,operand label) {
	intercodes ic = (intercodes)malloc(sizeof(struct InterCodes));
	ic->prev = ic->next = NULL;
	ic->code = (intercode)malloc(sizeof(struct InterCode));
	ic->code->kind = IFGOTO_K;
	ic->code->u.triop.reltype = type;
	ic->code->u.triop.label = label;
	ic->code->u.triop.c1 = c1;
	ic->code->u.triop.c2 = c2;
	add_code(ic);
	return ic;
}

//translate
intercodes translate_Exp(struct Node *node, operand place) {
	struct Node *child = node->child;
	assert(child != NULL);
	intercodes code1 = NULL;
	intercodes code2 = NULL;
	intercodes code3 = NULL;
	intercodes code4 = NULL;
	intercodes code5 = NULL;
	//INT
	if (strcmp(child->type,"INT") == 0) {
		operand c1 = new_constant(atoi(child->text));
		code1 = gen_assign(ASSIGN_K,place,c1);
		return code1;
	}
	//ID
	if (strcmp(child->type,"ID") == 0 && child->sibling == NULL) {
		int id = getVarID(child);
		operand v1 = new_var(id);
		code1 = gen_assign(ASSIGN_K,place,v1);
		return code1;
	}
	//Exp1 ASSINGOP Exp2
	if (strcmp(child->type,"Exp") == 0 && strcmp(child->sibling->type,"ASSIGNOP") == 0) {
		//Exp1 is ID (ID = Exp2)
//		printf("%s\n",child->child->type);
		if (strcmp(child->child->type,"ID") == 0) {
			operand t1 =  new_tmp();
			int id = getVarID(child->child);
			operand v1 = new_var(id);
			code1 = translate_Exp(child->sibling->sibling,t1);
			code2 = gen_assign(ASSIGN_K,v1,t1);
		//	code1 = link(code1,code2);
			if (place != NULL) {
				code3 = gen_assign(ASSIGN_K,place,v1);
				code1 = link(code1,code3);
			}
			return code1;
		}
		//Exp[Exp] = Exp2
	}
	//Exp1 PLUS/MINUS/STAR/DIV Exp2
	if (strcmp(child->type,"Exp") == 0 &&
		(strcmp(child->sibling->type,"PLUS") == 0 
		|| strcmp(child->sibling->type,"MINUS") == 0
		|| strcmp(child->sibling->type,"STAR") == 0
		|| strcmp(child->sibling->type,"DIV") == 0)) {
		operand t1 = new_tmp();
		operand t2 = new_tmp();
		code1 = translate_Exp(child,t1);
		code2 = translate_Exp(child->sibling->sibling,t2);
		//printf("%d %d\n", t1->u.tmp_no, t2 -> u.tmp_no);
		if (strcmp(child->sibling->type,"PLUS") == 0) {
			code3 = gen_binop(ADD_K,place,t1,t2);
		}
		else if (strcmp(child->sibling->type,"MINUS") == 0)
			code3 = gen_binop(SUB_K,place,t1,t2);
		else if (strcmp(child->sibling->type,"STAR") == 0)
			code3 = gen_binop(MUL_K,place,t1,t2);
		else if (strcmp(child->sibling->type,"DIV") == 0)
			code3 = gen_binop(DIV_K,place,t1,t2);
		//code1 = link(code1,code2);
		//code1 = link(code1,code3);
		return code1;
	}
	//MINUS Exp1
	if (strcmp(child->type,"MINUS") == 0)  {
		operand t1 = new_tmp();
		code1 = translate_Exp(child,t1);
		operand c1 = new_constant(0);
		code2 = gen_binop(SUB_K,place,c1,t1);
		code1 = link(code1,code2);
		return code1;
	}
	//Exp1 RELOP Exp2
	//NOT Exp1
	//Exp1 AND Exp2
	//Exp1 OR Exp2
	if (strcmp(child->type,"NOT") == 0 
	|| strcmp(child->sibling->type,"RELOP") == 0
	|| strcmp(child->sibling->type,"AND") == 0
	|| strcmp(child->sibling->type,"OR") == 0) {
		operand label1 = new_label();
		operand label2 = new_label();
		operand c1 = new_constant(0);
		operand c2 = new_constant(1);
		code1 = gen_assign(ASSIGN_K,place,c1);
		code2 = translate_Cond(node,label1,label2);
		code3 = gen_one(LABEL_K,label1);
		code4 = gen_assign(ASSIGN_K,place,c2);
		code5 = gen_one(LABEL_K,label2);
		code1 = link(code1,code2);
		code1 = link(code1,code3);
		code1 = link(code1,code4);
		code1 = link(code1,code5);
		return code1;
	}
	//ID LP RP
	if (strcmp(child->sibling->sibling->type,"LP") == 0 && strcmp(child->sibling->sibling->type,"RP") == 0) {
		char *func_name = child->text;
		if (strcmp(func_name,"read") == 0) {
			if (place == NULL) {
				operand t1 = new_tmp();
				code1 = gen_one(READ_K,t1);
			}
			else 
				code1 = gen_one(READ_K,place);
			return code1;
		}
		struct funMes *func_node = getFunMes(child);
		assert(func_node != NULL);
		operand f1 = new_function(func_name);
		if (place != NULL && place->kind != ADDRESS) 
			code1 = gen_assign(CALL_K,place,f1);
		else if (place != NULL && place->kind == ADDRESS) {
			operand t2 = new_tmp();
			code1 = gen_assign(CALL_K,t2,f1);
			code2 = gen_assign(ASSIGN_K,place,t2);
			code1 = link(code1,code2);
		}
		else {
			operand t2 = new_tmp();
			code1 = gen_assign(CALL_K,t2,f1);
		}
		return code1;
	}
	//ID LP Args RP
	if (strcmp(child->sibling->type,"LP") == 0 && strcmp(child->sibling->sibling->type,"Args") == 0) {
		char *func_name = child->text;
		operand *arg_list = (operand *)malloc(sizeof(operand) * 10);
		int arg_num = 0;
		code1 = translate_Args(child->sibling->sibling,arg_list,&arg_num);
		if (strcmp(func_name,"write") == 0) {
			assert(arg_num == 1);
			operand t1;
			if (arg_list[0]->kind == ADDRESS) {
				t1 = new_tmp();
				code2 = gen_assign(ASSIGN_K,t1,arg_list[0]);
				code1 = link(code1,code2);
			}
			else 
				t1 = arg_list[0];
			code3 = gen_one(WRITE_K,t1);
			code1 = link(code1,code3);
			return code1;
		}
		int i;
		for (i = 0;i < arg_num;i++) {
			code2 = gen_one(ARG_K,arg_list[i]);
			code1 = link(code1,code2);
		}
		operand f1 = new_function(func_name);
		if (place != NULL && place->kind != ADDRESS) 
			code3 = gen_assign(CALL_K,place,f1);
		else if (place != NULL && place->kind == ADDRESS) {
			operand t2 = new_tmp();
			code1 = gen_assign(CALL_K,t2,f1);
			code2 = gen_assign(ASSIGN_K,place,t2);
			code1 = link(code1,code2);
		}
		else {
			operand t2 = new_tmp();
			code3 = gen_assign(CALL_K,t2,f1);
		}
		code1 = link(code1,code3);
		return code1;
	}
	//LP Exp RP
	if (strcmp(child->type,"LP") == 0) {
		return translate_Exp(child->sibling,place);
	}
	//Exp1 LB Exp2 RB
	if (strcmp(child->type,"LB") == 0) {
		return translate_array(node,place,NULL,NULL);
	}
	//Exp DOT ID
	return NULL;

}

intercodes translate_Cond(struct Node *node,operand label_true,operand label_false) {
	struct Node *child = node->child;
	assert(child != NULL);
	intercodes code1,code2,code3,code4;
	//Exp1 RELOP Exp2
	if (child->sibling != NULL && strcmp(child->sibling->type,"RELOP") == 0) {
		operand t1 = new_tmp();
		operand t2 = new_tmp();
		code1 = translate_Exp(child,t1);
		code2 = translate_Exp(child->sibling->sibling,t2);
		relop_type type = get_relop(child->sibling);
		code3 = gen_triop(type,t1,t2,label_true);
		code4 = gen_one(GOTO_K,label_false);
		code1 = link(code1,code2);
		code1 = link(code1,code3);
		code1 = link(code1,code4);
		return code1;
	}
	//NOT Exp1
	if (strcmp(child->type,"NOT") == 0) 
		return translate_Cond(child->sibling,label_true,label_false);
	//Exp1 AND Exp2
	if (child->sibling != NULL && strcmp(child->sibling->type,"AND") == 0) {
		operand label1 = new_label();
		code1 = translate_Cond(child,label1,label_false);
		code2 = translate_Cond(child->sibling->sibling,label_true,label_false);
		code3 = gen_one(LABEL_K,label1);
		code1 = link(code1,code3);
		code1 = link(code1,code2);
		return code1;
	}
	//Exp1 OR Exp2
	if (child->sibling != NULL && strcmp(child->sibling->type,"OR") == 0) {
		operand label1 = new_label();
		code1 = translate_Cond(child,label_true,label1);
		code2 = translate_Cond(child->sibling->sibling,label_true,label_false);
		code3 = gen_one(LABEL_K,label1);
		code1 = link(code1,code3);
		code1 = link(code1,code2);
		return code1;
	}
	//other cases
	operand t1 = new_tmp();
	code1 = translate_Exp(node,t1);
	operand c1 = new_constant(0);
	code2 = gen_triop(NE,t1,c1,label_true);
	code3 = gen_one(LABEL_K,label_false);
	code1 = link(code1,code2);
	code1 = link(code1,code3);
	return code1;
}

intercodes translate_Stmt(struct Node *node) {
	struct Node *child = node->child;
	assert(child != NULL);
	intercodes code1,code2,code3,code4,code5,code6,code7;
	//Exp SEMI
	if (strcmp(child->type,"Exp") == 0)  {
		return translate_Exp(child,NULL);
	}
	//CompSt
	if (strcmp(child->type,"CompSt") == 0) return translate_CompSt(child);
	//RETURN Exp SEMI
	if (strcmp(child->type,"RETURN") == 0) {
		operand t1 = new_tmp();
		code1 = translate_Exp(child->sibling,t1);
		code2 = gen_one(RETURN_K,t1);
	//	code1 = link(code1,code2);
		return code1;
	}
	if (strcmp(child->type,"IF") == 0) {
		struct Node *brother = child->sibling->sibling->sibling->sibling->sibling;
	//IF LP Exp RP Stmt1
		if (brother == NULL) {
			operand label1 = new_label();
			operand label2 = new_label();
			code1 = translate_Cond(child->sibling->sibling,label1,label2);
			code2 = translate_Stmt(child->sibling->sibling->sibling->sibling);
			code3 = gen_one(LABEL_K,label1);
			code4 = gen_one(LABEL_K,label2);
			code1 = link(code1,code3);
			code1 = link(code1,code2);
			code1 = link(code1,code4);
			return code1;
		}
	//IF LP Exp RP Stmt1 ELSE Stmt2
		else {
			operand label1 = new_label();
			operand label2 = new_label();
			operand label3 = new_label();
			code1 = translate_Cond(child->sibling->sibling,label1,label2);
			code2 = translate_Stmt(child->sibling->sibling->sibling->sibling);
			code3 = translate_Stmt(brother->sibling);
			code4 = gen_one(LABEL_K,label1);
			code5 = gen_one(LABEL_K,label2);
			code6 = gen_one(LABEL_K,label3);
			code7 = gen_one(GOTO_K,label3);
			code1 = link(code1,code4);
			code1 = link(code1,code2);
			code1 = link(code1,code7);
			code1 = link(code1,code5);
			code1 = link(code1,code3);
			code1 = link(code1,code6);
			return code1;
		}
	}
	//WHILE LP Exp RP Stmt1
	if (strcmp(child->type,"WHILE") == 0) {
		operand label1 = new_label();
		operand label2 = new_label();
		operand label3 = new_label();
		code1 = translate_Cond(child->sibling->sibling,label2,label3);
		code2 = translate_Stmt(child->sibling->sibling->sibling->sibling);
		code3 = gen_one(LABEL_K,label1);
		code4 = gen_one(LABEL_K,label2);
		code5 = gen_one(LABEL_K,label3);
		code6 = gen_one(GOTO_K,label1);
		code1 = link(code3,code1);
		code1 = link(code1,code4);
		code1 = link(code1,code2);
		code1 = link(code1,code6);
		code1 = link(code1,code5);
		return code1;
	}
	return NULL;
}

intercodes translate_CompSt(struct Node *node) {
	intercodes code1 = NULL;
	intercodes code2 = NULL;
	struct Node *child = node->child;
	assert(child != NULL);
	//LC DefList StmtList RC
	child = child->sibling;
	if (child != NULL && strcmp(child->type,"DefList") == 0) { 
		code1 = translate_DefList(child);
	}
	if (child->sibling != NULL && strcmp(child->sibling->type,"StmtList") == 0) {
		code2 = translate_StmtList(child->sibling);
	}
	if (code1 != NULL) 
		code1 = link(code1,code2);
	else 
		code1 = code2;
	return code1;
}

intercodes translate_StmtList(struct Node *node) {
	intercodes code1 = NULL;
	intercodes code2 = NULL;
	//NULL
	if (node == NULL) return NULL;
	struct Node *child = node->child;
	assert(child != NULL);
	//Stmt StmtList
	code1 = translate_Stmt(child);
	code2 = translate_StmtList(child->sibling);
	//code1 = link(code1,code2);
	return code1;
}

intercodes translate_Args(struct Node *node,operand *arg_list,int *arg_num) {
	intercodes code1,code2,code3,code4;
	struct Node *child = node->child;
	assert(child != NULL);
	//Exp
	if (child->sibling == NULL) {
		operand t1 = new_tmp();
		code1 = translate_Exp(child,t1);
		arg_list[*arg_num++] = t1;
		return code1;
	}
	//Exp COMMA Args
	if (strcmp(child->sibling->type,"COMMA") == 0) {
		operand t1 = new_tmp();
		code1 = translate_Exp(child,t1);
		arg_list[*arg_num++] = t1;
		code2 = translate_Args(child->sibling->sibling,arg_list,arg_num);
		code1 = link(code1,code2);
		return code1;
	}
}

intercodes translate_VarDec(struct Node *node,operand *place) {
	intercodes code1,code2,code3,code4;
	struct Node *child = node->child;
	assert(child != NULL);
	//ID
	if (strcmp(child->type,"ID") == 0) {
		int id = getVarID(child);
		enum varType type;
		union varp vp = getVar(child,&type);
		operand v1 = new_var(id);
		if (place != NULL) {
			*place = v1;
		}
		if (type == varInt || type == varFloat) { 
			return NULL;
		}
		else if (type == varArray || type == varStruct) {
			int size = get_structure_size(vp,type);
			operand c1 = new_constant(size);
			code1 = gen_assign(DEC_K,v1,c1);
			return code1;
		}
		else {
			printf("type is varError%d\n",type == varError);
			exit(-1);
		}
	}
	//VarDec INT LB INT RB
	return translate_VarDec(child,NULL);
}

intercodes translate_DefList(struct Node *node) {
	intercodes code1,code2;
	//NULL
	if (node == NULL) return NULL;
	struct Node *child = node->child;
	assert(child != NULL);
	//Def DefList
	code1 = translate_Def(child);
	code2 = translate_DefList(child->sibling);
	code1 = link(code1,code2);
	return code1;
}

intercodes translate_Def(struct Node *node) {
	struct Node *child = node->child;
	assert(child != NULL);
	//Specifier DecList SEMI
	return translate_DecList(child->sibling);
}

intercodes translate_DecList(struct Node *node) {
	struct Node *child = node->child;
	assert(child != NULL);
	intercodes code1 = NULL;
	intercodes code2 = NULL;
	//Dec
	if (child->sibling == NULL)
		return translate_Dec(child);
	//Dec COMMA DecList
	if (child->sibling != NULL && strcmp(child->sibling->type,"COMMA") == 0) {
		code1 = translate_Dec(child);
		code2 = translate_DecList(child->sibling->sibling);
		code1 = link(code1,code2);
		return code1;
	}
}

intercodes translate_Dec(struct Node *node) {
	struct Node *child = node->child;
	assert(child != NULL);
	intercodes code1,code2,code3;
	//VarDec
	if (child->sibling == NULL) { 
		return translate_VarDec(child,NULL);
	}
	//VarDec ASSIGNOP Exp
	if (child->sibling != NULL && strcmp(child->sibling->type,"ASSIGNOP") == 0) {
		operand t1 = NULL;
		code1 = translate_VarDec(child,&t1);
		operand t2 = new_tmp();
		code2 = translate_Exp(child->sibling->sibling,t2);
		code3 = gen_assign(ASSIGN_K,t1,t2);
		code1 = link(code1,code2);
		code1 = link(code1,code3);
		return code1;
	}
}

intercodes translate_FunDec(struct Node *node) {
	intercodes code1,code2;
	struct Node *child = node->child;
	assert(child != NULL);
	char *func_name = child->text;
//	struct funMes *func_node = getFunMes(child);
//	assert(func_node != NULL);
	operand f1 = new_function(func_name);
	code1 = gen_one(FUNCTION_K,f1);
	//ID LP VarList RP
	if (child->sibling->sibling->sibling != NULL) {
/*		struct argList *param = func_node->arg;
		while (param != NULL) {
			if (param->aType == varStruct) {
				printf("Cannot translate: Code contains variables or parameters of structure type.\n");
				exit(-1);
			}
			operand p1 = new_var(getVarIDbyName(param->name));
			code2 = gen_one(PARAM_K,p1);
			code1 = link(code1,code2);
			param = param->next;
		}*/
		code2 = translate_VarList(child->sibling->sibling);
//		code1 = link(code1,code2);
		return code1;
	}
	//ID LP RP
	return code1;
}

intercodes translate_VarList(struct Node *node) {
	intercodes code1,code2;
	struct Node *child = node->child;
	assert(child != NULL);
	code1 = translate_ParamDec(child);
	//ParamDec COMMA VarList
	if (child->sibling != NULL) {
		if (strcmp(child->sibling->sibling->type,"VarList") == 0) {
			code2 = translate_VarList(child->sibling->sibling);
			code1 = link(code1,code2);
			return code1;
		}
	}
	//ParamDec
	return code1;
}

intercodes translate_ParamDec(struct Node *node) {
	intercodes code1;
	struct Node *child = node->child;
	assert(child != NULL);
	if (strcmp(child->sibling->type,"VarDec") == 0) {
		code1 = translate_VarDec(child->sibling,NULL);
		return code1;
	}
	return NULL;
}


intercodes translate_ExtDef(struct Node *node) {
	intercodes code1,code2;
	struct Node *child = node->child;
	assert(child != NULL);
	//Specifier ExtDecList SEMI
	if(child -> sibling!= NULL)	{
		if (strcmp(child->sibling->type,"ExtDecList") == 0)  {
			return translate_ExtDecList(child->sibling);
		}
		//Specifier FunDec CompSt
		if (strcmp(child->sibling->type,"FunDec") == 0) {
			code1 = translate_FunDec(child->sibling);
			code2 = translate_CompSt(child->sibling->sibling);
			code1 = link(code1,code2);
			return code1;
		}
	}
	//Specifier SIMI
	return NULL;
}

intercodes translate_ExtDecList(struct Node *node) {
	intercodes code1,code2;
	struct Node *child = node->child;
	assert(child != NULL);
	//VarDec
	if (child->sibling == NULL) 
		return translate_VarDec(child,NULL);
	if (child->sibling != NULL && strcmp(child->sibling->type,"COMMA") == 0) {
		code1 = translate_VarDec(child,NULL);
		code2 = translate_ExtDecList(child->sibling->sibling);
		code1 = link(code1,code2);
		return code1;
	}
	return NULL;
}

intercodes translate_array(struct Node *node,operand place,union varp *list,enum varType *listtype) {
	struct Node *child = node->child;
	assert(child != NULL);
	intercodes code1 = NULL;
	intercodes code2,code3,code4;
	int size = 0;
	operand t1;
	//ID[Exp]
	if (strcmp(child->sibling->type,"ID") == 0) {
		int id = getVarID(child->child);
		int flag = getFlag(child->child->text);
		if (flag == 1)
			t1 = new_var(id);
		else 
			t1 = new_reference(id);
		enum varType type;
		union varp vp = getVar(child->child,&type);
		if (vp.ap->basetype == varStruct) 
			size = get_structure_size(vp.ap->base,vp.ap->basetype);
		if (vp.ap->basetype == varInt || vp.ap->basetype == varFloat) 
			size = 4;
		if (list != NULL) {
			*list = vp.ap->base;
			*listtype = vp.ap->basetype;
		}
	}
	//Exp[Exp][Exp]
//	if (child->child->sibling != NULL && strcmp(child->child->sibling->type,"LB") == 0) {
//	}
	//handle Exp2
	operand t2 = new_tmp();
	code2 = translate_Exp(child->sibling->sibling,t2);
	code1 = link(code1,code2);
	//calculate offset
	operand c1 = new_constant(size);
	operand t3 = new_tmp();
	code3 = gen_binop(MUL_K,t3,t2,c1);
	//Add array address
	operand t4 = new_tmp_id(place->u.tmp_no);
	code4 = gen_binop(ADD_K,t4,t1,t3);
	code1 = link(code1,code4);
	place->kind = ADDRESS;
	return code1;
}


intercodes link(intercodes l1,intercodes l2) {
	intercodes tmp;
	if (l1 == NULL && l2 == NULL) return NULL;
	if (l1 == NULL) {
		add_code(l2);
		return l2;
	}
	if (l2 == NULL) {
		add_code(l1);
		return l1;
	}
	tmp = l1;
	tmp->next = l2;
	l2->prev = tmp;
	return l1;
}

relop_type get_relop(struct Node *node) {
	char *value = node->text;
	if (strcmp(value,"==") == 0) return EQ;
	if (strcmp(value,"!=") == 0) return NE;
	if (strcmp(value,">") == 0) return MT;
	if (strcmp(value,"<") == 0) return LT;
	if (strcmp(value,">=") == 0) return ME;
	if (strcmp(value,"<=") == 0) return LE;
}

int get_structure_size(union varp vp,enum varType type) {
	int size = 0;
	if (type == varError)
		return 0;
	if (type == varInt || type == varFloat)
		return 4;
	if (type == varArray) {
	//dimension > 1 
		size = vp.ap->size * get_structure_size(vp.ap->base,vp.ap->basetype);
		return size;
	}
	if (type == varStruct) {
		struct singleSymbol *mem = vp.sp->memlist;
		while (mem != NULL) {
			size += get_structure_size(mem->sym.message.vmes->tp,mem->sym.message.vmes->vType);
			mem = mem->next;
		}
		return size;
	}
	return 0;
}
