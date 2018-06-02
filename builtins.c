
lvalue* builtin_add(lvalue* args, lvalue* scope){
	lvalue* arg1 = lvr_assert(lvlist_get(args, 0), "builtin_add missing arg 0");
	lvalue* arg2 = lvr_assert(lvlist_get(args, 1), "builtin_add missing arg 1");

	if(arg1->type != TY_INT || arg2->type != TY_INT || lvlist_len(args) != 2){
		puts("F: builtin_add bad arg(s)");
		exit(78);
	}

	return mk_int(arg1->v.integer + arg2->v.integer);
}

lvalue* builtin_sub(lvalue* args, lvalue* scope){
	int len = lvlist_len(args);
	if(len==0 || len>2){
		puts("F: builtin_sub bad arg(s)");
		exit(78);
	}
	lvalue* arg1 = lvr_assert(lvlist_get(args, 0), "builtin_sub missing arg 0");

	if(arg1->type != TY_INT){
		puts("F: builtin_sub bad type(s)");
		exit(78);
	}

	if(len==1){
		return mk_int(-(arg1->v.integer));
	}

	lvalue* arg2 = lvr_assert(lvlist_get(args, 1), "builtin_sub missing arg 1");

	if(arg2->type != TY_INT){
		puts("F: builtin_sub bad type(s)");
		exit(78);
	}

	return mk_int(arg1->v.integer - arg2->v.integer);
}

lvalue* builtin_let(lvalue* args, lvalue* scope){
	lvalue* name = lvr_assert(lvlist_get(args, 0), "builtin_let missing name");
	lvalue* value = lvr_assert(lvlist_get(args, 1), "builtin_let missing value");
	lvalue* body = lvr_assert(lvlist_get(args, 2), "builtin_let missing code");

	value = lvr_assert(eval(value, scope), "builtin_let error evaluating binding value");

	if(name->type != TY_SYM){
		puts("F: builtin_let name must be a symbol (it will not be evaluated)");
		exit(78);
	}

	return lvr_assert(eval(body, mk_cons(mk_cons(name, value), scope)),
		"builtin_let error evaluating body");
}

lvalue* builtin_letrec(lvalue* args, lvalue* scope){
	lvalue* name = lvr_assert(lvlist_get(args, 0), "builtin_letrec missing name");
	lvalue* value = lvr_assert(lvlist_get(args, 1), "builtin_letrec missing value");
	lvalue* body = lvr_assert(lvlist_get(args, 2), "builtin_letrec missing code");

	value = lvr_assert(eval(value, scope), "builtin_letrec error evaluating binding value");

	if(value->type != TY_CODE || value->v.func.builtin){
		puts("letrec only for user functions, sorry");
		exit(78);
	}

	if(name->type != TY_SYM){
		puts("F: builtin_letrec name must be a symbol (it will not be evaluated)");
		exit(78);
	}

	value->v.func.code.interpreted.closure = mk_cons(
		mk_cons(name, value), value->v.func.code.interpreted.closure);

	return lvr_assert(eval(body, mk_cons(mk_cons(name, value), scope)),
		"builtin_letrec error evaluating body");
}

lvalue* builtin_mk_func(lvalue* args, lvalue* scope){
	lvalue* name = lvr_assert(lvlist_get(args, 0), "builtin_mk_func missing name");
	lvalue* lv_macro = lvr_assert(lvlist_get(args, 1), "builtin_mk_func missing macro");
	lvalue* func_scope = lvr_assert(lvlist_get(args, 2), "builtin_mk_func missing scope");
	lvalue* body = lvr_assert(lvlist_get(args, 3), "builtin_mk_func missing code");

	if(name->type != TY_SYM){
		puts("F: builtin_mk_func name must be a symbol (it will not be evaluated)");
		exit(78);
	}

	bool macro = lv_is_true(lv_macro);

	return mk_closure(name->v.ptr, macro, body, func_scope);
}

lvalue* builtin_get_scope(lvalue* args, lvalue* scope){
	if(lvlist_len(args)!=0){
		puts("F: builtin_get_scope bad args");
		exit(78);
	}

	return scope;
}

lvalue* builtin_quote(lvalue* args, lvalue* scope){
	lvalue* v = lvr_assert(lvlist_get(args, 0), "builtin_quote missing value");

	return v;
}

lvalue* builtin_cons(lvalue* args, lvalue* scope){
	lvalue* lhs = lvr_assert(lvlist_get(args, 0), "builtin_cons missing lhs");
	lvalue* rhs = lvr_assert(lvlist_get(args, 1), "builtin_cons missing rhs");

	return mk_cons(lhs, rhs);
}

lvalue* builtin_eval(lvalue* args, lvalue* scope){
	// puts("builtin_eval");
	lvalue* code = lvr_assert(lvlist_get(args, 0), "builtin_eval missing code");
	lvalue* c_scope = lvr_assert(lvlist_get(args, 1), "builtin_eval missing scope");

	return lvr_assert(eval(code, c_scope), "builtin_eval eval error");
}

lvalue* builtin_lhs(lvalue* args, lvalue* scope){
	lvalue* v = lvr_assert(lvlist_get(args, 0), "builtin_lhs missing value");

	if(v->type != TY_CONS){
		printf("F: builtin_lhs on non-cons: ");
		println_lvalue(v);
		exit(78);
	}

	return v->v.pair.lhs;
}

lvalue* builtin_rhs(lvalue* args, lvalue* scope){
	lvalue* v = lvr_assert(lvlist_get(args, 0), "builtin_rhs missing value");

	if(v->type != TY_CONS){
		printf("F: builtin_rhs on non-cons: ");
		println_lvalue(v);
		exit(78);
	}

	return v->v.pair.rhs;
}

lvalue* builtin_if(lvalue* args, lvalue* scope){
	lvalue* condition = lvr_assert(lvlist_get(args, 0), "builtin_if missing condition");
	lvalue* iftrue = lvr_assert(lvlist_get(args, 1), "builtin_if missing if-true");
	lvalue* iffalse = lvr_assert(lvlist_get(args, 2), "builtin_if missing if-false");

	condition = lvr_assert(eval(condition, scope), "builtin_if error evaluating condition");

	if(lv_is_true(condition)){
		return lvr_assert(eval(iftrue, scope), "builtin_if error true");
	}else{
		return lvr_assert(eval(iffalse, scope), "builtin_if error false");
	}
}

lvalue* builtin_eq(lvalue* args, lvalue* scope){
	lvalue* arg1 = lvr_assert(lvlist_get(args, 0), "builtin_if missing operand 1");
	lvalue* arg2 = lvr_assert(lvlist_get(args, 1), "builtin_if missing operand 2");

	return lv_equal(arg1, arg2)?mk_int(1):lv_nil;
}
