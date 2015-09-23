my_basic : main.o my_basic.o
	cc -o output/my_basic_bin main.o my_basic.o -lm

main.o : shell/main.c core/my_basic.h
	cc -Os -c shell/main.c -Wno-unused-result

my_basic.o : core/my_basic.c core/my_basic.h
	cc -Os -c core/my_basic.c -Wno-multichar -Wno-overflow -Wno-unused-result

clean :
	rm -f main.o my_basic.o output/my_basic_bin output/my_basic_bin.exe
