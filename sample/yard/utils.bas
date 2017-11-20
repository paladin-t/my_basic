REM Yet Another RPG Dungeon is a text based game.
REM It's aimed to be a comprehensive example and a tutorial of MY-BASIC.
REM For more information, see https://github.com/paladin-t/my_basic/

def cls()
	if os() = "WINDOWS" then
		sys("cls")
	else
		sys("clear")
	endif
enddef

def to_lower_case(c)
	_asc = asc(c)
	if _asc >= asc("A") and _asc <= asc("Z") then
		_asc = _asc + asc("a") - asc("A")
	endif

	return chr(_asc)
enddef

def bit_and(a, b)
	c = 0
	for i = 0 to 31
		if (a mod 2) and (b mod 2) then
			t = 1
		else
			t = 0
		endif
		c = c + t * (2 ^ i)
		a = fix(a / 2)
		b = fix(b / 2)
	next

	return c
enddef

def bit_or(a, b)
	c = 0
	for i = 0 to 31
		if (a mod 2) or (b mod 2) then
			t = 1
		else
			t = 0
		endif
		c = c + t * (2 ^ i)
		a = fix(a / 2)
		b = fix(b / 2)
	next

	return c
enddef

def bit_xor(a, b)
	c = 0
	for i = 0 to 31
		if (a mod 2) <> (b mod 2) then
			t = 1
		else
			t = 0
		endif
		c = c + t * (2 ^ i)
		a = fix(a / 2)
		b = fix(b / 2)
	next

	return c
enddef

class point
	var x = 0
	var y = 0

	def to_string()
		return "[" + str(x) + ", " + str(y) + "]"
	enddef
endclass

class log
	def m(msg)
		print "Message: " + msg;
	enddef

	def w(msg)
		print "Warning: " + msg;
	enddef

	def e(msg)
		print "Error: " + msg;
	enddef
endclass
