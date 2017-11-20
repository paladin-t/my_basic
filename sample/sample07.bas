REM This program is an example of MY-BASIC
REM For more information, see https://github.com/paladin-t/my_basic/

def reserve(m, n)
	for j = len(m) to n + 2
		push(m, 0)
	next
enddef

def forward(cmd, i)
	l = len(cmd)
	k = 0
	while i < l
		h = mid(cmd, i, 1)
		if h = "[" then
			k = k + 1
		elseif h = "]" then
			k = k - 1
		endif
		if h = "]" and k = 0 then
			return i
		endif
		i = i + 1
	wend

	return i
enddef

def backward(cmd, i)
	k = 0
	while i > 0
		h = mid(cmd, i, 1)
		if h = "]" then
			k = k + 1
		elseif h = "[" then
			k = k - 1
		endif
		if h = "[" and k = 0 then
			return i
		endif
		i = i - 1
	wend

	return i
enddef

def brainfuck(cmd)
	m = list()

	i = 0
	cursor = 0

	l = len(cmd)
	while i < l
		c = mid(cmd, i, 1)
		if c = ">" then
			cursor = cursor + 1
		elseif c = "<" then
			cursor = cursor - 1
		elseif c = "+" then
			reserve(m, cursor)
			b = m(cursor)
			m(cursor) = b + 1
		elseif c = "-" then
			reserve(m, cursor)
			b = m(cursor)
			m(cursor) = b - 1
		elseif c = "." then
			reserve(m, cursor)
			print chr(m(cursor))
		elseif c = "," then
			reserve(m, cursor)
			input ch$
			m(cursor) = asc(ch$)
		elseif c = "[" then
			reserve(m, cursor)
			b = m(cursor)
			if b = 0 then
				i = forward(cmd, i)
			endif
		elseif c = "]" then
			reserve(m, cursor)
			b = m(cursor)
			if b <> 0 then
				i = backward(cmd, i)
			endif
		endif
		i = i + 1
	wend
enddef

' This is a brainfuck interpreter written with MY-BASIC
input "Input: ", cmd$
' Input "hello" to use hello-world
if cmd$ = "hello" then
	cmd$ = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>."
endif

brainfuck(cmd$)
