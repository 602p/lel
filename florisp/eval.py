from data import LNum, LStr, LCons, LFunc, LBuiltin, LSym, LScope, LNil
from sys import exit
from parse import read
import functools

def error(*m):
	print(*m)
	print("* --- Not Continuing --- *");
	exit(1)

def eval(expr, scope):
	print(" - eval", expr.l_str())
	if type(expr) in [LNum, LStr, LBuiltin] or expr == LNil:
		return expr
	if isinstance(expr, LSym):
		if LScope.contains(scope, expr.value):
			return LScope.get(scope, expr.value)
		else:
			error("Unbound name:", expr.value, "\nScope:", scope.l_str())

	if not isinstance(expr, LCons):
		error("You shouldn't eval that:", expr.l_str())

	func=expr.get_nth(0)
	func = eval(func, scope)
	if not (isinstance(func, LFunc) or isinstance(func, LBuiltin)):
		error("Head-position did not evaluate to a function:", func)

	args = expr.rhs
	if not func.is_fexpr:
		args = LCons.from_py_list(list(map(lambda x:eval(x, scope), args.to_py_list())))

	if isinstance(func, LFunc):
		child_scope=func.scope.clone()
		LScope.add_binding(child_scope, "$args", args)
		LScope.add_binding(child_scope, "$parent", scope)
		return eval(func.code, child_scope)
	else: #LBuiltin
		return func.func(args.to_py_list(), scope)

builtins=LCons(LNil, LNil)
def builtin(name, nargs=None, is_fexpr=False, argtypes=None):
	def _wrap(f):
		@functools.wraps(f)
		def _wrapped(args, scope):
			if nargs!=None and len(args) != nargs:
				error("Wrong number of arguments to builtin", name)
			if argtypes:
				for idx, v in enumerate(args):
					if argtypes[idx]:
						if not isinstance(v, argtypes[idx]):
							error("Bad arg type to builtin", name)
			return f(args, scope)
		global builtins
		LScope.add_binding(builtins, "^"+name, LBuiltin(name, is_fexpr, _wrapped))
		return _wrapped
	return _wrap

@builtin("+", 2, argtypes=[LNum, LNum])
def bi_add(args, scope):
	return LNum(args[0].value + args[1].value)

@builtin("-", 2, argtypes=[LNum, LNum])
def bi_add(args, scope):
	return LNum(args[0].value - args[1].value)

@builtin("*", 2, argtypes=[LNum, LNum])
def bi_add(args, scope):
	return LNum(args[0].value * args[1].value)

@builtin("/", 2, argtypes=[LNum, LNum])
def bi_add(args, scope):
	return LNum(args[0].value / args[1].value)

@builtin("quote", 1, is_fexpr=True)
def bi_quote(args, scope):
	return args[0]

@builtin("list")
def bi_list(args, scope):
	return LCons.from_py_list(args)

@builtin("block")
def bi_block(args, scope):
	return args[-1]

@builtin("scope", 0)
def bi_scope(args, scope):
	return scope

@builtin("eval", 2, argtypes=[None, LCons])
def bi_eval(args, scope):
	return eval(args[0], args[1])

@builtin("let", 3, is_fexpr=True, argtypes=[LSym, None, None])
def bi_let(args, scope):
	child_scope=scope.clone()
	LScope.add_binding(child_scope, args[0].value, eval(args[1], scope))
	return eval(args[2], child_scope)

@builtin("set-in!", 3, is_fexpr=True, argtypes=[LCons, LSym, None])
def bi_set_in(args, scope):
	the_scope=eval(args[0], scope)
	the_name=args[1].value
	the_value=eval(args[2], scope)
	print("set-in!", the_scope, the_name)
	if not LScope.contains(the_scope, the_name):
		LScope.add_binding(the_scope, the_name, the_value)
	else:
		LScope.set_binding(the_scope, the_name, the_value)

@builtin("cons", 2, argtypes=[None, LCons])
def bi_cons(args, scope):
	return LCons(args[0], args[1])

@builtin("eq", 2)
def bi_eq(args, scope):
	return LNum(1) if args[0]==args[1] else LNil

@builtin("lt", 2, argtypes=[LNum, LNum])
def bi_lt(args, scope):
	return LNum(1) if args[0].value < args[1].value else LNil

@builtin("lhs", 1, argtypes=[LCons])
def bi_lhs(args, scope):
	if args[0]==LNil:
		error("lhs on ()")
	return args[0].lhs

@builtin("get-func-scope", 1, argtypes=[LFunc])
def bi_get_func_scope(args, scope):
	return args[0].scope

@builtin("rhs", 1, argtypes=[LCons])
def bi_rhs(args, scope):
	if args[0]==LNil:
		error("rhs on ()")
	return args[0].rhs

@builtin("if", 3, is_fexpr=True)
def bi_if(args, scope):
	if eval(args[0], scope)==LNil:
		return eval(args[2], scope)
	else:
		return eval(args[1], scope)

@builtin("mkfunc", 4, argtypes=[LStr, None, None, LCons])
def bi_mkfunc(args, scope):
	return LFunc(args[0].value, args[1].is_nil(), args[2], args[3])

with open("test.fl", 'r') as fd:
	code = read(fd.read())
print("Code:", code.l_str())
result = eval(code, builtins)
print("Result:", result)
print("Result.l_str():", result.l_str())