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
	builtins = nil;
	builtins = CONS(CONS(STR("nil"), nil), builtins);
	builtins = CONS(CONS(STR("neg"), STR("neg")), builtins);
	builtins = CONS(CONS(STR("add"), STR("add")), builtins);
	builtins = CONS(CONS(STR("sub"), STR("sub")), builtins);
	builtins = CONS(CONS(STR("print"), STR("print")), builtins);
	builtins = CONS(CONS(STR("eq"), STR("eq")), builtins);
	builtins = CONS(CONS(STR("lt"), STR("lt")), builtins);
	builtins = CONS(CONS(STR("quote"), STR("quote")), builtins);
	builtins = CONS(CONS(STR("hyperquote"), STR("hyperquote")), builtins);
	builtins = CONS(CONS(STR("eval"), STR("eval")), builtins);
	builtins = CONS(CONS(STR("scope"), STR("scope")), builtins);
	builtins = CONS(CONS(STR("cons"), STR("cons")), builtins);
	builtins = CONS(CONS(STR("lhs"), STR("lhs")), builtins);
	builtins = CONS(CONS(STR("rhs"), STR("rhs")), builtins);
	builtins = CONS(CONS(STR("let"), STR("let")), builtins);
	builtins = CONS(CONS(STR("letrec"), STR("letrec")), builtins);
	builtins = CONS(CONS(STR("if"), STR("if")), builtins);
	builtins = CONS(CONS(STR("lst"), SCALAR(-2001)), builtins);
	builtins = CONS(CONS(STR("$parent"), SCALAR(-2001)), builtins);
	builtins = CONS(CONS(STR("nth"), SCALAR(-2001)), builtins);
	builtins = CONS(CONS(STR("$args"), SCALAR(-2002)), builtins);
	builtins = CONS(CONS(STR("add1"), SCALAR(-2003)), builtins);
	builtins = CONS(CONS(STR("add1-code"), SCALAR(-2004)), builtins);

	// char* code = "((cons (scope) (quote ((add (lhs $args) 1)))) 5)";
	// char* code = "(let list (cons (scope) (quote ((cons quote (cons $args nil))))) (list 1 2 3))";
	// char* code = "(let list (cons (scope) (quote ((quote quote)))) ((list) (1 2 3)))";
	// char* code = "(add 1 1)";
	// char* code = "((eval (eval add (scope)) (scope)) 5 5)";
	// char* code = "(let add (cons (scope) (quote ((add (lhs $args) 1)))) (add 1))";

	FILE* file = fopen("simple.lel", "r");
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