#include "interp.h"

bool is_list(lvalue* v){
	return v==lv_nil || (v->type==TY_CONS && is_list(v->v.pair.rhs));
}

void print_lvalue(lvalue* lv){
	if(lv==lv_builtins) printf("{lv_builtins}");
	if(lv->type == TY_INT) printf("%i", lv->v.integer);
	else if(lv->type == TY_SYM){
		printf("%s", lv->v.ptr);
	}else if(lv->type == TY_CODE){
		printf("{%sfunction %s ", lv->v.func.macro?"macro-":"", lv->v.func.name);
		if(lv->v.func.builtin){
			printf("{builtin}");
		}else{
			print_lvalue(lv->v.func.code.interpreted.code);
			// printf(", closure: ");
			// print_lvalue(lv->v.func.code.interpreted.closure);
		}
		printf("}");
	}else{
		if(is_list(lv)){
			printf("(");
			while(lv!=lv_nil){
				print_lvalue(lv->v.pair.lhs);
				if(lv->v.pair.rhs!=lv_nil){
					printf(" ");
				}
				lv=lv->v.pair.rhs;

				if(lv==lv_builtins){
					printf("...{lv_builtins}");
					break;
				}
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
	// puts("eval: ");
	// println_lvalue(code);
	if(code->type==TY_INT || code->type==TY_CODE) return LVR_SUCCEED(code);
	if(code->type==TY_SYM) return assoclist_lookup(scope, code);

	lvalue_result temp;

	REQUIRE_TO(func, eval(code->v.pair.lhs, scope), "func-eval-error");

	if(func->type != TY_CODE) return LVR_FAILURE_I(mk_sym("not-a-function"));

	lvalue* args = code->v.pair.rhs;
	lvalue* passed_args = args;
	if(!func->v.func.macro){
		passed_args = lv_nil;
		while(args!=lv_nil){
			REQUIRE_TO(arg, eval(args->v.pair.lhs, scope), "arg-error");
			passed_args = lvlist_append(passed_args, arg);
			args = args->v.pair.rhs;
		}
	}

	if(func->v.func.builtin){
		return LVR_SUCCEED(func->v.func.code.builtin(passed_args, scope));
	}else{
		REQUIRE_TO(result, eval(func->v.func.code.interpreted.code,
			mk_cons(mk_cons(mk_sym("builtin:args"), passed_args),
			mk_cons(mk_cons(mk_sym("builtin:parent_scope"), scope),
					func->v.func.code.interpreted.closure))
			), "func-call-error");
		return LVR_SUCCEED(result);
	}
}