#pragma once

#include "data.h"

char** tokenize(char* text);

lvalue* parse(char** tokens);