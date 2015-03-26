' This script is an example of MY-BASIC
' Copyright (c) 2011 - 2015 W. Renxin. All rights reserved.
' For more information, see https://github.com/paladin-t/my_basic/

e = 50

PRINT "Primes in ", e, ": ", 2, ", "

FOR n = 3 TO e
    m = 2
    is = 1
    WHILE m < n
        IF n MOD m = 0 THEN is = 0 ELSE m = m + 1
        IF is = 0 THEN EXIT
    WEND
    IF is = 1 THEN PRINT n, ", "
NEXT n