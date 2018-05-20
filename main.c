#include "data.h"
#include "eval.h"
#include "parse.h"
#include <stdlib.h>
#include <stdio.h>

lvalue* nil;
lvalue* builtins;
gc_cell* gc_root;

int main(){
	char* code = "(foo (bar baz) quux oof)";
	char** tokens = tokenize(code);

	for(int i=0; tokens[i]!=NULL; i++){
		printf("'%s'\n", tokens[i]);
	}

	return 0;
}

int test_eval(){
	nil = SCALAR(-9999);
	builtins = LIST(
		CONS(STR("nil"), nil),
		CONS(STR("neg"), SCALAR(-1000)),
		CONS(STR("add"), SCALAR(-1001)),
		CONS(STR("quote"), SCALAR(-1002)),
		CONS(STR("eval"), SCALAR(-1003)),
		CONS(STR("scope"), SCALAR(-1007)),
		CONS(STR("cons"), SCALAR(-1004)),
		CONS(STR("lhs"), SCALAR(-1005)),
		CONS(STR("rhs"), SCALAR(-1006)),
		CONS(STR("let"), SCALAR(-1008)),
		CONS(STR("x"), SCALAR(-1)),
		CONS(STR("$args"), SCALAR(-2))
	);

	lvalue* func = LIST(
		STR("cons"),
		LIST(STR("scope")),
		LIST(
			STR("cons"),
			LIST(STR("quote"), LIST(STR("add"), SCALAR(1), LIST(STR("lhs"), STR("$args")))),
			nil
		)
	);

	lvalue* code = LIST(func, SCALAR(3));

	print_lvalue(code, builtins), printf("\n");
	lvalue* v = eval(code, builtins);
	print_lvalue(v, builtins), printf("\n");
	return 0;
}