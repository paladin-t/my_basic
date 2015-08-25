' This script is an example of MY-BASIC
' Copyright (c) 2011 - 2015 Wang Renxin. All rights reserved.
' For more information, see https://github.com/paladin-t/my_basic/

BEGIN:
	n = 10
	DIM arr(n)
	GOSUB CALC
	GOSUB SHOW
END

CALC:
	arr(0) = 1
	FOR i = 1 TO n - 1
		IF i = 1 THEN arr(i) = 1 ELSE arr(i) = arr(i - 1) + arr(i - 2)
	NEXT
RETURN

SHOW:
	FOR i = 0 TO n - 1
		PRINT arr(i), ", "
	NEXT
RETURN