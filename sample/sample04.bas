' This script is an example of MY-BASIC
' Copyright (c) 2011 - 2017 Wang Renxin. All rights reserved.
' For more information, see https://github.com/paladin-t/my_basic/

begin:
	n = 10
	dim arr(n)
	gosub calc
	gosub show
end

calc:
	arr(0) = 1
	for i = 1 to n - 1
		if i = 1 then arr(i) = 1 else arr(i) = arr(i - 1) + arr(i - 2)
	next
return

show:
	for i = 0 to n - 1
		print arr(i), ", "
	next
return
