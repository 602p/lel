import attr

class LVal:
	def is_nil(self):
		return False

class LNum(LVal, attr.make_class("LNum", ["value"])):
	def l_str(self):
		return repr(self.value)

class LStr(LVal, attr.make_class("LStr", ["value"])):
	def l_str(self):
		return "\""+self.value.replace("\"", "\\\"")+"\""

class LCons(LVal, attr.make_class("LList", ["lhs", "rhs"])):
	def l_str(self):
		return "("+(" ".join(map(lambda x:x.l_str(), self.to_py_list())))+")"

	def to_py_list(self):
		r=[]
		x=self
		while x!=LNil:
			r.append(x.lhs)
			x=x.rhs
		return r

	def get_nth(self, v):
		i=0
		x=self
		while x!=LNil:
			if i==v:
				return x.lhs
			x=x.rhs
			i+=1
		raise IndexError()

	def len(self):
		i=0
		x=self
		while x!=LNil:
			i+=1
			x=x.rhs
		return i

	@classmethod
	def from_py_list(self, l):
		if l==[]:
			return LNil
		else:
			r = LCons(l[-1], LNil)
			for i in reversed(l[:-1]):
				r=LCons(i, r)
			return r

	def clone(self):
		return LCons.from_py_list(self.to_py_list())

LNil = LCons(None, None)

class LFunc(LVal, attr.make_class("LFunc", ["name", "is_fexpr", "code", "scope"])):
	def l_str(self):
		return "{"+repr(self)+"}"

class LBuiltin(LVal, attr.make_class("LFunc", ["name", "is_fexpr", "func"])):
	def l_str(self):
		return "{"+repr(self)+"}"

#TODO: Interning
class LSym(LVal, attr.make_class("LSym", ["value"])):
	def l_str(self):
		return self.value

class LScope:
	@staticmethod
	def add_binding(self, k, v):
		self.lhs=LCons(LCons.from_py_list([LSym(k), v, LNil]), self.lhs)

	@staticmethod
	def get(self, k):
		self=self.lhs
		while self!=LNil:
			if self.lhs.lhs.value==k:
				return self.lhs.rhs.lhs
			self=self.rhs
		raise IndexError()

	@staticmethod
	def contains(self, k):
		self=self.lhs
		while self!=LNil:
			if self.lhs.lhs.value==k:
				return True
			self=self.rhs
		return False

	@staticmethod
	def set_binding(self, k, v):
		self=self.lhs
		x=self
		while x!=LNil:
			if x.lhs.lhs.value==k:
				x.lhs.rhs.lhs=v
				return
			x=x.rhs
		raise IndexError((self.l_str(), k, v))