#include "data.h"
#include "eval.h"
#include "parse.h"
#include <stdlib.h>
#include <stdio.h>

lvalue* nil;
lvalue* builtins;
gc_cell* gc_root;

int main(){
	nil = SCALAR(-9999);
	builtins = LIST(
		CONS(STR("nil"), nil),
		CONS(STR("neg"), STR("neg")),
		CONS(STR("add"), STR("add")),
		CONS(STR("sub"), STR("sub")),
		CONS(STR("print"), STR("print")),
		CONS(STR("eq"), STR("eq")),
		CONS(STR("lt"), STR("lt")),
		CONS(STR("quote"), STR("quote")),
		CONS(STR("eval"), STR("eval")),
		CONS(STR("scope"), STR("scope")),
		CONS(STR("cons"), STR("cons")),
		CONS(STR("lhs"), STR("lhs")),
		CONS(STR("rhs"), STR("rhs")),
		CONS(STR("let"), STR("let")),
		CONS(STR("letrec"), STR("letrec")),
		CONS(STR("if"), STR("if")),
		CONS(STR("lst"), SCALAR(-2001)),
		CONS(STR("$parent"), SCALAR(-2001)),
		CONS(STR("nth"), SCALAR(-2001)),
		CONS(STR("$args"), SCALAR(-2002))
	);

	// char* code = "((cons (scope) (quote ((add (lhs $args) 1)))) 5)";
	// char* code = "(let list (cons (scope) (quote ((cons quote (cons $args nil))))) (list 1 2 3))";
	// char* code = "(let list (cons (scope) (quote ((quote quote)))) ((list) (1 2 3)))";
	// char* code = "(add 1 1)";
	// char* code = "((eval (eval add (scope)) (scope)) 5 5)";
	// char* code = "(let add (cons (scope) (quote ((add (lhs $args) 1)))) (add 1))";

	FILE* file = fopen("list_tools.lel", "r");
	char buf[4096];
	buf[fread(&buf, 1, sizeof(buf), file)] = 0;
	fclose(file);

	printf("--text--\n%s\n--------\n", buf);

	char** tokens = tokenize(buf);

	// for(int i=0; tokens[i]!=NULL; i++) printf("%i: '%s'\n", i, tokens[i]);

	lvalue* obj = parse(tokens);

	printf("Code: "), print_lvalue(obj, builtins), printf("\n");

	lvalue* res = eval(obj, builtins);

	printf("Result: "), print_lvalue(res, builtins), printf("\n");

	return 0;
}