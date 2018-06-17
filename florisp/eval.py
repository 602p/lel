from data import LNum, LStr, LCons, LFunc, LBuiltin, LSym, LScope, LNil
from sys import exit
from parse import read
import functools

def error(*m):
	print(*m)
	print("* --- Not Continuing --- *");
	exit(1)

def eval(expr, scope):
	if type(expr) in [LNum, LStr, LBuiltin] or expr.is_nil():
		return expr
	if isinstance(expr, LSym):
		if LScope.contains(scope, expr.value):
			return LScope.get(scope, expr.value)
		else:
			error("Unbound name:", expr.value, "\nScope:", scope)

	if not isinstance(expr, LCons):
		error("You shouldn't eval that:", expr.l_str())

	func=expr.get_nth(0)
	print(func.l_str())
	# print(func)
	func = eval(func, scope)
	if not (isinstance(func, LFunc) or isinstance(func, LBuiltin)):
		error("Head-position did not evaluate to a function:", func)

	args = expr.value[1:]
	if not func.is_fexpr:
		args = list(map(lambda x:eval(x, scope), args))

	if isinstance(func, LFunc):
		child_scope=LScope.new_child(scope)
		LScope.add_binding(child_scope, "$args", args)
		LScope.add_binding(child_scope, "$parent", scope)
		return eval(func.code, child_scope)
	else: #LBuiltin
		return func.func(args, scope)

builtins=LNil
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
		LScope.add_binding(builtins, "builtin:"+name, LBuiltin(name, is_fexpr, _wrapped))
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

@builtin("scope", 0)
def bi_scope(args, scope):
	return scope

@builtin("eval", 2, argtypes=[None, LCons])
def bi_eval(args, scope):
	return eval(args[0], args[1])

@builtin("cons", 2, argtypes=[None, LCons])
def bi_cons(args, scope):
	return LCons(args[0], args[1])

@builtin("lhs", 1, argtypes=[LCons])
def bi_quote(args, scope):
	return args[0].value[0]

@builtin("mkfunc", 4, argtypes=[LStr, None, None, LCons])
def bi_mkfunc(args, scope):
	return LFunc(args[0].value, args[1].is_nil(), args[2], args[3])

with open("test.fl", 'r') as fd:
	code = read(fd.read())
print("Code:", code)
result = eval(code, builtins)
print("Result:", result)
print("Result.l_str():", result.l_str())