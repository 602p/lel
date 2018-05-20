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


	if(list_len(func)==3){
		lvalue* bind_name = list_get(func, 0);
		lvalue* func_code = list_get(func, 1);
		lvalue* closure_scope = list_get(func, 2);

		printf("bind_name: "), print_lvalue(bind_name, builtins), printf("\n");
		printf("func_code: "), print_lvalue(func_code, builtins), printf("\n");
		printf("closure_scope: "), print_lvalue(closure_scope, builtins), printf("\n");

		lvalue* args = nil;
		lvalue* hd = code->v.cons.rhs;
		while(hd!=nil){
			args=CONS(hd->v.cons.lhs, args);
			hd=hd->v.cons.rhs;
		}
		
		printf("args: "), print_lvalue(args, builtins), printf("\n");

		return eval(func_code, CONS(CONS(bind_name, args), closure_scope));
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

void print_lvalue(lvalue* v, lvalue* bindings){
	lvalue_result res = assoclist_get(bindings, v);
	if(v==nil) printf("()");
	else if(res.success) print_string(v);
	else if(v->scalar) printf("%i", v->v.scalar);
	else{
		printf("(");
		print_lvalue(v->v.cons.lhs, bindings);
		printf(" ");
		print_lvalue(v->v.cons.rhs, bindings);
		printf(")");
	}
}