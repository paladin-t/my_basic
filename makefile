OS := $(shell uname -s)
IS_APPLE := $(shell echo $(OS)|grep -i darwin)

ifdef IS_APPLE
my_basic: my_basic_x86_app.o my_basic_arm_app.o
	lipo -create -output output/my_basic my_basic_x86_app.o my_basic_arm_app.o
my_basic_x86_app.o: my_basic_x86.o main_x86.o
	cc -o my_basic_x86_app.o my_basic_x86.o main_x86.o -target x86_64-apple-macos10.12
my_basic_arm_app.o: my_basic_arm.o main_arm.o
	cc -o my_basic_arm_app.o my_basic_arm.o main_arm.o -target arm64-apple-macos11
my_basic_x86.o: core/my_basic.c core/my_basic.h
	cc -Os -c core/my_basic.c -Wno-multichar -Wno-overflow -Wno-unused-result -o my_basic_x86.o -target x86_64-apple-macos10.12
main_x86.o: shell/main.c core/my_basic.h
	cc -Os -c shell/main.c -Wno-unused-result -o main_x86.o -target x86_64-apple-macos10.12
my_basic_arm.o: core/my_basic.c core/my_basic.h
	cc -Os -c core/my_basic.c -Wno-multichar -Wno-overflow -Wno-unused-result -o my_basic_arm.o -target arm64-apple-macos11
main_arm.o: shell/main.c core/my_basic.h
	cc -Os -c shell/main.c -Wno-unused-result -o main_arm.o -target arm64-apple-macos11
else
my_basic: my_basic.o main.o
	cc -o output/my_basic my_basic.o main.o -lm -lrt
my_basic.o: core/my_basic.c core/my_basic.h
	cc -Os -c core/my_basic.c -Wno-multichar -Wno-overflow -Wno-unused-result
main.o: shell/main.c core/my_basic.h
	cc -Os -c shell/main.c -Wno-unused-result
endif

clean:
	rm -f *.o output/my_basic output/my_basic.exe
