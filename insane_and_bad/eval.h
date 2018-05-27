#pragma once

#include "data.h"

lvalue* eval(lvalue* code, lvalue* scope);

void print_string(lvalue* s);

void print_lvalue(lvalue* v, lvalue* bindings);
bool is_list(lvalue* v);

void print_assoclist(lvalue* list);