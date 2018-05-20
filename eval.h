#pragma once

#include "data.h"

lvalue* eval(lvalue* code, lvalue* scope);

void print_string(lvalue* s);

void print_lvalue(lvalue* v, lvalue* bindings);