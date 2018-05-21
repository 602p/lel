#include "eval.h"
#include <stdio.h>

#define eerror(x) puts("E: eval: " x)
#define EQ_BUILTIN(func, name) (func==result_assert(assoclist_get_str(builtins, name)))

lvalue* eval(lvalue* code, lvalue* scope){
	if(code->scalar) return code;

	lvalue* func = eval(code->v.cons.lhs, scope);
	// printf("FUNC="), print_lvalue(func, builtins), printf("\n");
	int nr_args = list_len(code) - 1;
	if(EQ_BUILTIN(func, "neg")){
		if(nr_args==1){
			lvalue* arg = eval(list_get(code, 1), scope);
			if(arg->scalar) return SCALAR(-arg->v.scalar);
		}
		eerror("Invalid invocation to neg");
		return nil;
	}

	if(EQ_BUILTIN(func, "add")){
		if(nr_args==2){
			lvalue* arg1 = eval(list_get(code, 1), scope);
			lvalue* arg2 = eval(list_get(code, 2), scope);
			if(arg1->scalar && arg2->scalar)
				return SCALAR(arg1->v.scalar + arg2->v.scalar);
		}
		eerror("Invalid invocation to add");
		return nil;
	}

	if(EQ_BUILTIN(func, "sub")){
		if(nr_args==2){
			lvalue* arg1 = eval(list_get(code, 1), scope);
			lvalue* arg2 = eval(list_get(code, 2), scope);
			// printf("sub %i %i\n", arg1->v.scalar, arg2->v.scalar);
			if(arg1->scalar && arg2->scalar)
				return SCALAR(arg1->v.scalar - (arg2->v.scalar));
		}
		eerror("Invalid invocation to sub");
		return nil;
	}

	if(EQ_BUILTIN(func, "eq")){
		if(nr_args==2){
			lvalue* arg1 = eval(list_get(code, 1), scope);
			lvalue* arg2 = eval(list_get(code, 2), scope);
			// printf("check eq "), print_lvalue(arg1, builtins), printf(" "), print_lvalue(arg2, builtins), printf("\n");
			return lvalue_eq(arg1, arg2)?SCALAR(1):nil;
		}
		eerror("Invalid invocation to eq");
		return nil;
	}

	if(EQ_BUILTIN(func, "lt")){
		if(nr_args==2){
			lvalue* arg1 = eval(list_get(code, 1), scope);
			lvalue* arg2 = eval(list_get(code, 2), scope);
			if(arg1->scalar && arg2->scalar)
				return arg1->v.scalar<arg2->v.scalar?SCALAR(1):nil;
		}
		eerror("Invalid invocation to lt");
		return nil;
	}

	if(EQ_BUILTIN(func, "cons")){
		if(nr_args==2){
			lvalue* arg1 = eval(list_get(code, 1), scope);
			lvalue* arg2 = eval(list_get(code, 2), scope);
			return CONS(arg1, arg2);
		}
		eerror("Invalid invocation to cons");
		return nil;
	}

	if(EQ_BUILTIN(func, "let")){
		if(nr_args==3){
			lvalue* name = list_get(code, 1);
			lvalue* val  = eval(list_get(code, 2), scope);
			lvalue* body = list_get(code, 3);
			return eval(body, CONS(CONS(name, val), scope));
		}
		eerror("Invalid invocation to let");
		return nil;
	}

	if(EQ_BUILTIN(func, "letrec")){
		if(nr_args==3){
			lvalue* name = list_get(code, 1);
			lvalue* val  = eval(list_get(code, 2), scope);
			val->v.cons.lhs=CONS(CONS(name, val), val->v.cons.lhs);
			lvalue* body = list_get(code, 3);
			return eval(body, CONS(CONS(name, val), scope));
		}
		eerror("Invalid invocation to let");
		return nil;
	}

	if(EQ_BUILTIN(func, "if")){
		if(nr_args==3){
			lvalue* cond = eval(list_get(code, 1), scope);
			lvalue* iftrue  = list_get(code, 2);
			lvalue* iffalse = list_get(code, 3);
			if(lvalue_eq(cond, nil)){
				return eval(iffalse, scope);
			}else{
				return eval(iftrue, scope);
			}
		}
		eerror("Invalid invocation to if");
		return nil;
	}

	if(EQ_BUILTIN(func, "quote")){
		if(nr_args==1) return list_get(code, 1);
		eerror("Invalid invocation to quote");
		return nil;
	}

	if(EQ_BUILTIN(func, "print")){
		if(nr_args==1){
			lvalue* v = eval(list_get(code, 1), scope);
			printf("print: "), print_lvalue(v, scope), printf("\n");
			return v;
		}
		eerror("Invalid invocation to print");
		return nil;
	}

	if(EQ_BUILTIN(func, "eval")){
		if(nr_args==2) return eval(eval(list_get(code, 1), scope), eval(list_get(code, 2), scope));
		eerror("Invalid invocation to eval");
		return nil;
	}

	if(EQ_BUILTIN(func, "scope")){
		if(nr_args==0) return scope;
		eerror("Invalid invocation to scope");
		return nil;
	}

	if(EQ_BUILTIN(func, "lhs")){
		lvalue* arg1 = eval(list_get(code, 1), scope);
		if(nr_args==1 && !arg1->scalar) return arg1->v.cons.lhs;
		eerror("Invalid invocation to lhs");
		return nil;
	}

	if(EQ_BUILTIN(func, "rhs")){
		lvalue* arg1 = eval(list_get(code, 1), scope);
		if(nr_args==1 && !arg1->scalar) return arg1->v.cons.rhs;
		eerror("Invalid invocation to rhs");
		return nil;
	}

	if(code->v.cons.lhs->scalar && is_list(code)){ //Name Resolution
		lvalue_result res = assoclist_get(scope, code);
		if(res.success){
			return res.data;
		}
		eerror("Unbound Name");
		print_lvalue(code, builtins), printf("\n");
		return nil;
	}


	if(list_len(func)==2){
		lvalue* func_code = list_get(func, 1);
		lvalue* closure_scope = list_get(func, 0);

		printf("$args: "), print_lvalue(code->v.cons.rhs, builtins), printf("\n");
		printf("func_code: "), print_lvalue(func_code, builtins), printf("\n");

		closure_scope=CONS(CONS(STR("$parent"), scope), CONS(CONS(STR("$args"), code->v.cons.rhs), closure_scope));
		lvalue* res = eval(func_code, closure_scope);
		printf("res: "), print_lvalue(res, builtins), printf("\n");
		lvalue* term = eval(res, scope);
		printf("term: "), print_lvalue(term, builtins), printf("\n");

		return term;
	}

	eerror("Function was not a function");
	print_lvalue(func, builtins), printf("\n");
	return nil;
}


void print_string(lvalue* s){
	if(s->scalar) return;
	while(!s->scalar && s->v.cons.lhs->scalar){
		printf("%c", s->v.cons.lhs->v.scalar);
		s=s->v.cons.rhs;
	}
}

bool is_list(lvalue* v){
	return v==nil || (!v->scalar && is_list(v->v.cons.rhs));
}

typedef struct cyclelink {
	lvalue* v;
	struct cyclelink* prev;
} cyclelink;

bool is_cyclical(lvalue* v, cyclelink* c){
	return v==c->v || (c->prev!=NULL && is_cyclical(v, c->prev));
}

void print_lvalue_cd(lvalue* v, lvalue* bindings, cyclelink* prev_cyclelink){
	cyclelink cl = {v, prev_cyclelink};
	lvalue_result res = assoclist_get(bindings, v);
	if(v==nil) printf("()");
	else if(res.success) print_string(v);
	else if(v->scalar) printf("%i", v->v.scalar);
	else if(is_list(v)){
		printf("(");
		while(v!=nil){
			if (!is_cyclical(v->v.cons.lhs, &cl)) print_lvalue_cd(v->v.cons.lhs, bindings, &cl);
			else printf("<cycle>");
			if(v->v.cons.rhs!=nil) printf(" ");
			v=v->v.cons.rhs;
		}
		printf(")");
	}else{
		printf("(cons ");
		if (!is_cyclical(v->v.cons.lhs, &cl)) print_lvalue_cd(v->v.cons.lhs, bindings, &cl);
		else printf("<cycle>");
		printf(" ");
		if (!is_cyclical(v->v.cons.rhs, &cl)) print_lvalue_cd(v->v.cons.rhs, bindings, &cl);
		else printf("<cycle>");
		printf(")");
	}
}

void print_lvalue(lvalue* v, lvalue* bindings){
	print_lvalue_cd(v, bindings, NULL);
}