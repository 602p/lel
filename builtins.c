
lvalue* builtin_add(lvalue* args){
	lvalue* arg1 = lvr_assert(list_get(args, 0), "builtin_add missing arg 0");
	lvalue* arg2 = lvr_assert(list_get(args, 1), "builtin_add missing arg 1");
	if(arg1->type != TY_INT || arg2->type != TY_INT){
		printf("F: builtin_add bad type(s)\n");
		exit(78);
	}

	return mk_int(arg1->v.integer + arg2->v.integer);
}