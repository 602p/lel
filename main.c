#include "data.h"
#include "interp.h"
#include "parse.h"

#include "builtins.c"

lvalue* lv_nil;
lvalue* lv_builtins;

lvalue* al_add_builtin(lvalue* list, char* name, bool macro, builtin_fn* func){
	lvalue* sym = mk_sym(name);
	return mk_cons(mk_cons(sym, mk_builtin(macro, func)), list);
}

int main(){
	lv_nil = mk_sym("nil");
	lv_builtins = mk_cons(mk_cons(lv_nil, lv_nil), lv_nil);
	lv_builtins = al_add_builtin(lv_builtins, "+", false, builtin_add);

	FILE* file = fopen("test.lel", "r");
	char buf[4096];
	buf[fread(&buf, 1, sizeof(buf), file)] = 0;
	fclose(file);

	printf("--text--\n%s\n--------\n", buf);

	char** tokens = tokenize(buf);

	// for(int i=0; tokens[i]!=NULL; i++) printf("%i: '%s'\n", i, tokens[i]);

	lvalue* obj = parse(tokens);

	printf("Code: "), print_lvalue(obj), printf("\n");
	// printf("Builtins: "), print_lvalue(lv_builtins), printf("\n");

	// lvalue_result res = assoclist_lookup(lv_builtins, obj);

	lvalue_result res = eval(obj, lv_builtins);

	printf("OK? %i\n", res.ok);
	printf("Res: "), print_lvalue(res.result), printf("\n");

	return 0;
}