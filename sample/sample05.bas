' This script is an example of MY-BASIC
' Copyright (c) 2011 - 2015 Wang Renxin. All rights reserved.
' For more information, see https://github.com/paladin-t/my_basic/

DEF AREA(a, b)
	RETURN CALL MUL(a, b)
ENDDEF

DEF MUL(a, b)
	return a * b
ENDDEF

a = 3
b = 4
PRINT a; b; AREA(a, b);