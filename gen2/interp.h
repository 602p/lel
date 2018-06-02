#pragma once

#include "data.h"

#include <stdlib.h>
#include <stdio.h>

#define LV_ASSERT(x, msg) {if(!(x)){printf("F: LV_ASSERT: " msg "\n"); exit(1);}}

void print_lvalue(lvalue* lv);
void println_lvalue(lvalue* lv);

lvalue_result eval(lvalue* code, lvalue* scope);

lvalue* lvr_assert(lvalue_result v, char* err);
void l_fatal(char* err);