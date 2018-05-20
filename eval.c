#include "eval.h"
#include <stdio.h>

#define eerror(x) puts("E: eval: " x)
#define EQ_BUILTIN(func, name) (func==result_assert(assoclist_get_str(builtins, name)))

lvalue* eval(lvalue* code, lvalue* scope){
	if(code->scalar) return code;

	if(code->v.cons.lhs->scalar){ //Name Resolution
		lvalue_result res = assoclist_get(scope, code);
		if(res.success){
			return res.data;
		}
		eerror("Unbound Name");
		return nil;
	}

	lvalue* func = eval(code->v.cons.lhs, scope);
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
				return SCALAR(arg1->v.scalar+arg2->v.scalar);
		}
		eerror("Invalid invocation to add");
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
			lvalue* body = eval(list_get(code, 3), scope);
			return eval(body, CONS(CONS(name, val), scope));
		}
		eerror("Invalid invocation to cons");
		return nil;
	}

	if(EQ_BUILTIN(func, "quote")){
		if(nr_args==1) return list_get(code, 1);
		eerror("Invalid invocation to quote");
		return nil;
	}

	if(EQ_BUILTIN(func, "eval")){
		if(nr_args==2) return eval(list_get(code, 1), eval(list_get(code, 2), scope));
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


	if(list_len(func)==2){
		lvalue* func_code = list_get(func, 1);
		lvalue* closure_scope = list_get(func, 0);

		printf("func_code: "), print_lvalue(func_code, builtins), printf("\n");
		printf("closure_scope: "), print_lvalue(closure_scope, builtins), printf("\n");

		return eval(func_code, CONS(CONS(STR("$args"), code->v.cons.rhs), closure_scope));
	}

	eerror("Function was not a function");
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

void print_lvalue(lvalue* v, lvalue* bindings){
	lvalue_result res = assoclist_get(bindings, v);
	if(v==nil) printf("()");
	else if(res.success) print_string(v);
	else if(v->scalar) printf("%i", v->v.scalar);
	else if(is_list(v)){
		printf("(");
		while(v!=nil){
			print_lvalue(v->v.cons.lhs, bindings);
			if(v->v.cons.rhs!=nil) printf(" ");
			v=v->v.cons.rhs;
		}
		printf(")");
	}else{
		printf("(cons ");
		print_lvalue(v->v.cons.lhs, bindings);
		printf(" ");
		print_lvalue(v->v.cons.rhs, bindings);
		printf(")");
	}
}