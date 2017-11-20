REM This program is an example of MY-BASIC
REM For more information, see https://github.com/paladin-t/my_basic/

def area(a, b)
	return call mul(a, b)
enddef

def mul(a, b)
	return a * b
enddef

a = 3
b = 4
print a; b; area(a, b);
