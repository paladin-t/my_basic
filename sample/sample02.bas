REM This program is an example of MY-BASIC
REM For more information, see https://github.com/paladin-t/my_basic/

e = 50

print "Primes in ", e, ": ", 2, ", "

for n = 3 to e
	m = 2
	isp = 1
	while m < n
		if n mod m = 0 then isp = 0 else m = m + 1
		if isp = 0 then exit
	wend
	if isp = 1 then print n, ", "
next n
