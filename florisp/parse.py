import data

def tokenize(s):
	t=[]
	i=0
	while i<len(s):
		if s[i] in "()":
			t.append(s[i])
		elif s[i] in " \t\r\n":
			pass
		elif s[i]=="\"":
			i+=1
			t.append("\"")
			while s[i]!="\"":
				if s[i]=="\\" and s[i+1]=="\"":
					i+=2
					t[-1]+="\""
					continue
				t[-1]+=s[i]
				i+=1
			t[-1]+="\""
		else:
			t.append("")
			while i<len(s) and s[i] not in "() \t\r\n":
				t[-1]+=s[i]
				i+=1
			if i<len(s) and s[i] in "()":
				i-=1
		i+=1
	return t

def parse_list(t):
	pos=1
	v=[]
	while t[pos]!=")":
		child, l = parse(t[pos:])
		pos+=l
		v.append(child)
	return data.LCons.from_py_list(v), pos+1

def parse(t):
	if t[0]=="(":
		return parse_list(t)
	if t[0][0]=="\"":
		return data.LStr(t[0][1:-1]), 1
	if t[0].isdigit():
		return data.LNum(int(t[0])), 1
	return data.LSym(t[0]), 1

def read(s):
	return parse(tokenize(s))[0]
