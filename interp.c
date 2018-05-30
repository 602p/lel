#include "interp.h"

bool is_list(lvalue* v){
	return v==lv_nil || (v->type==TY_CONS && is_list(v->v.pair.rhs));
}

void print_lvalue(lvalue* lv){
	if(lv->type == TY_INT) printf("%i", lv->v.integer);
	else if(lv->type == TY_SYM){
		printf("%s", lv->v.ptr);
	}else{
		if(is_list(lv)){
			printf("(");
			while(lv!=lv_nil){
				print_lvalue(lv->v.pair.lhs);
				if(lv->v.pair.rhs!=lv_nil){
					printf(" ");
				}
				lv=lv->v.pair.rhs;
			}
			printf(")");
		}else{
			printf("(^cons ");
			print_lvalue(lv->v.pair.lhs);
			printf(" ");
			print_lvalue(lv->v.pair.rhs);
			printf(")");
		}
	}
}

void println_lvalue(lvalue* lv){
	print_lvalue(lv), printf("\n");
}

#define REQUIRE_TO(name, code, error) \
	temp = (code);\
	if(!temp.ok)\
		return LVR_FAILURE_I(mk_cons(mk_sym(error), mk_cons(temp.result, lv_nil)));\
	lvalue* name = temp.result;

lvalue_result eval(lvalue* code, lvalue* scope){
	if(code->type==TY_INT) return LVR_SUCCEED(code);
	if(code->type==TY_SYM) return assoclist_lookup(scope, code);

	lvalue_result temp;

	REQUIRE_TO(func, eval(code->v.pair.lhs, scope), "func-eval-error");

	if(func->type != TY_CODE) return LVR_FAILURE_I(mk_sym("not-a-function"));

	lvalue* args = code->v.pair.rhs;
	lvalue* passed_args = args;
	if(!func->v.func.macro){
		lvalue* passed_args = lv_nil;
		while(args!=lv_nil){
			REQUIRE_TO(arg, eval(args->v.pair.lhs, scope), "arg-error");
			passed_args = mk_cons(arg, passed_args);
			args = args->v.pair.rhs;
		}
	}

	if(func->v.func.builtin){
		return LVR_SUCCEED(func->v.func.code.builtin(passed_args));
	}else{
		return LVR_FAILURE_I(mk_sym("not-implemented"));
	}
}