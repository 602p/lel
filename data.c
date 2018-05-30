#include "data.h"
#include "interp.h"
#include <stdlib.h>
#include <string.h>

bool lv_is_true(lvalue* v){
	return v != lv_nil;
}

lvalue* mk_int(int v){
	lvalue* d = malloc(sizeof(lvalue));
	d->type = TY_INT;
	d->v.integer = v;
	return d;
}

lvalue* mk_cons(lvalue* l, lvalue* r){
	lvalue* d = malloc(sizeof(lvalue));
	d->type = TY_CONS;
	d->v.pair.lhs = l;
	d->v.pair.rhs = r;
	return d;
}

lvalue* mk_sym(char* sym){
	lvalue* d = malloc(sizeof(lvalue));
	//TODO: Copy sym?
	d->type = TY_SYM;
	d->v.ptr = sym;
	return d;
}

lvalue* mk_builtin(bool macro, builtin_fn* func){
	lvalue* d = malloc(sizeof(lvalue));
	d->type = TY_CODE;
	d->v.func.macro = macro;
	d->v.func.builtin = true;
	d->v.func.code.builtin = func;
	return d;
}

bool lv_equal(lvalue* a, lvalue* b){
	// printf("cmp %u %u\n", a, b);
	// printf("A: "), println_lvalue(a);
	// printf("B: "), println_lvalue(b);
	if(a == b) return true;
	if(a->type == TY_INT){
		return b->type == TY_INT && a->v.integer == b->v.integer;
	}else if(a->type == TY_SYM){
		// return a == b;
		return b->type == TY_SYM && (strcmp(a->v.ptr, b->v.ptr) == 0);
	}else{
		return b->type == TY_CONS &&
				lv_equal(a->v.pair.lhs, b->v.pair.lhs) &&
				lv_equal(a->v.pair.rhs, b->v.pair.rhs);
	}
}

lvalue_result assoclist_lookup(lvalue* list, lvalue* key){
	// printf("Lookup %u in %u\n", key, list);
	// printf("List: "), println_lvalue(list);
	if(lv_equal(list, lv_nil)) return LVR_FAILURE_I(mk_sym("not-found"));
	if(list->type!=TY_CONS)
		return LVR_FAILURE_I(mk_sym("assoclist-not-a-cons"));

	// printf("Get entry\n");
	lvalue* entry = list->v.pair.lhs;
	// printf("Check entry\n");
	if(entry->type!=TY_CONS)
		return LVR_FAILURE_I(mk_sym("assoclist-entry-not-a-cons"));

	// printf("About to check lhs and key\n");
	if(lv_equal(entry->v.pair.lhs, key)) return LVR_SUCCEED(entry->v.pair.rhs);
	else return assoclist_lookup(list->v.pair.rhs, key);
}

lvalue_result list_get(lvalue* list, int i){
	if(lv_equal(list, lv_nil) || i<0)
		return LVR_FAILURE_I(mk_sym("out-of-bounds"));

	if(i==0) return LVR_SUCCEED(list->v.pair.lhs);
	else return list_get(list->v.pair.rhs, i-1);
}

lvalue* lvr_assert(lvalue_result v, char* err){
	if(!v.ok){
		printf("F: lvr_assert: %s\n", err);
		exit(77);
	}
	return v.result;
}