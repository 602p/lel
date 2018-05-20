#include "data.h"
#include "eval.h"
#include <stdlib.h>
#include <stdio.h>

lvalue* nil;
lvalue* builtins;
gc_cell* gc_root;

int main(){
	nil = SCALAR(-9999);
	builtins = LIST(
		CONS(STR("nil"), nil),
		CONS(STR("neg"), SCALAR(-1000)),
		CONS(STR("add"), SCALAR(-1001)),
		CONS(STR("quote"), SCALAR(-1002)),
		CONS(STR("eval"), SCALAR(-1003)),
		CONS(STR("scope"), SCALAR(-1003)),
		CONS(STR("lhs"), SCALAR(-1005)),
		CONS(STR("rhs"), SCALAR(-1006)),
		CONS(STR("x"), SCALAR(-1))		
	);
	lvalue* code = LIST(
		LIST(STR("quote"), LIST(
				STR("x"),
				LIST(STR("add"), SCALAR(1), LIST(STR("lhs"), STR("x"))),
				builtins
			)
		),
		SCALAR(3));

	lvalue* d = LIST(
		STR("quote"),
		LIST(
			STR("x"),
			LIST(
				STR("lhs"),
				STR("x")
			),
			nil
		)
	);

	print_lvalue(d, nil), printf("\n");
	return 0;

	print_lvalue(code, builtins), printf("\n");
	lvalue* v = eval(code, builtins);
	print_lvalue(v, builtins), printf("\n");
	return 0;
}