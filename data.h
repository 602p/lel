#pragma once

#include <stdbool.h>

typedef enum {
	TY_INT = 1,
	TY_CONS = 2,
	TY_SYM = 3,
	TY_CODE = 4,
	TY_STRING = 5
} lvalue_type;

typedef struct lvalue* builtin_fn(struct lvalue*);

typedef struct lvalue {
	lvalue_type type;
	union {
		int integer;
		struct {
			struct lvalue* lhs;
			struct lvalue* rhs;
		} pair;
		char* ptr;
		struct {
			bool macro;
			bool builtin;
			union {
				builtin_fn* builtin;
				struct {
					struct lvalue* code;
					struct lvalue* closure;
				} interpreted;
			} code;
		} func;
	} v;
} lvalue;

typedef struct {
	bool ok;
	lvalue* result;
} lvalue_result;

#define LVR_SUCCEED(x) ((lvalue_result){true, x})
#define LVR_FAILURE ((lvalue_result){false, nil})
#define LVR_FAILURE_I(x) ((lvalue_result){false, x})

extern lvalue* lv_nil;
extern lvalue* lv_builtins;

bool lv_is_true(lvalue* v);
lvalue* mk_int(int v);
lvalue* mk_cons(lvalue* l, lvalue* r);
lvalue* mk_sym(char* sym);
lvalue* mk_builtin(bool macro, builtin_fn* func);

bool lv_equal(lvalue* a, lvalue* b);
lvalue_result assoclist_lookup(lvalue* list, lvalue* key);
lvalue_result list_get(lvalue* list, int i);