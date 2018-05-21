#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct lvalue {
	bool scalar;
	union {
		int scalar;
		struct {
			struct lvalue* lhs;
			struct lvalue* rhs; 
		} cons;
	} v;
} lvalue;

typedef struct gc_cell {
	lvalue* data;
	struct gc_cell* next;
} gc_cell;

typedef struct lvalue_result {
	bool success;
	lvalue* data;
} lvalue_result;

#define SUCCEED(x) ((lvalue_result){true, x})
#define FAILURE ((lvalue_result){false, nil})
#define FAILURE_I(x) ((lvalue_result){false, x})
lvalue* result_assert(lvalue_result v);

lvalue* make_scalar(int val);
#define SCALAR(x) make_scalar(x)

extern lvalue* nil;
extern lvalue* builtins;

lvalue* make_cons(lvalue* lhs, lvalue* rhs);
#define CONS(x, y) make_cons(x, y)

lvalue* make_list(int nr, ...);
lvalue* make_string(char* s);
#define STR(x) make_string(x)

#define _GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N
#define COUNT_VARARGS(...) _GET_NTH_ARG(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define LIST(...) make_list(COUNT_VARARGS(__VA_ARGS__), __VA_ARGS__)

bool lvalue_eq(lvalue* a, lvalue* b);
bool lvalue_eq_ascii(lvalue* l, char* str);

lvalue_result assoclist_get(lvalue* list, lvalue* key);
lvalue_result assoclist_rev_get(lvalue* list, lvalue* key);
lvalue_result assoclist_get_str(lvalue* list, char* key);
int list_len(lvalue* list);
lvalue* list_get(lvalue* list, int pos);

extern gc_cell* gc_root;
void lvalue_free(lvalue* l);
bool gc_get_used_in(lvalue* structure, lvalue* l);
bool gc_get_used(lvalue* l);