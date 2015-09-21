![](resource/icon.ico)

Copyright (C) 2011 - 2015 [Wang Renxin](https://twitter.com/wangrenxin). All rights reserved.

[LinkedIn](https://cn.linkedin.com/pub/wang-renxin/43/494/20)

Why are existing script interpreters so complex? Why is it so difficult to integrate with them? Why not try MY-BASIC today! Scripting should be simple and enjoyable.

## Introduction

MY-BASIC is a lightweight cross-platform easy extendable BASIC interpreter written in pure C with about 7000 lines of source code. Its grammar is similar to structured BASIC. It is aimed to be either an embeddable scripting language or a standalone interpreter. The core is pretty light; all in a C source file and an associated header file. You are able to easily combine MY-BASIC with an existing project in C, C++, Objective-C, etc. Scripting driven can make your projects more powerful, elegant and neat.

## Main features

* It is totally free to use MY-BASIC for individual or commercial purpose under the MIT license
* Written in clean ANSI C, source code portable
* With most common BASIC syntax
* Lightweight (within memory usage less than 128KB) and fast
* Case-insensitive tokenization
* Integer, float point, string, boolean, user defined data types with array support, etc.
* Structured `IF-THEN-ELSEIF-ELSE-ENDIF` support
* Structured `FOR-TO-STEP-NEXT/WHILE-WEND/DO-UNTIL` support
* Structured user costomized sub routine definition by `DEF/ENDDEF` support
* `GOTO/GOSUB-RETURN` support
* Standard numeric functions, and standard string functions
* Debug APIs
* Customizable memory pool
* High expansibility, easy to use APIs, easy to write customized scripting interfaces
* It's able to use it as a standalone interpreter, or integrate it with existing projects in C, C++, Objective-C, etc.
* It's able to learn how to build an interpreter from scratch from MY-BASIC
* It's able to build your own dialect based on MY-BASIC
* More features/modules under developing

## Scripting at a glance

Come along with a traditional "hello world" script in MY-BASIC:

	print "What is your name: "
	input n$

	def foo(a, b)
		return a + " " + b + " by " + n$ + "."
	enddef

	print foo("Hello", "world");

Read the [MY-BASIC Quick Reference](MY-BASIC%20Quick%20Reference.pdf) (especially the "**Programming with BASIC**" section) to get more details about how to program in MY-BASIC.

## [Interpreter workflow diagram](https://github.com/paladin-t/my_basic/wiki/Interpreter-workflow-diagram)

![](https://github.com/paladin-t/my_basic/blob/master/interpreter%20workflow%20diagram.png)

## Installation

### Use standalone interpreter binary

This repository contains precompiled binaries for [Windows](output/my_basic.exe) and [OS X](output/my_basic_mac), it's efficient to download it and have a first impressive playground. Or you could make a build as follow.

* Open the Visual Studio solution `my_basic.sln` on Windows, open the XCode solution `my_basic_mac.xcodeproj` on OS X, or use the `makefile` with "make" toolchain to build an interpreter binary

To compile an interpreter binary for your own platform manually, please follow the steps.

* Retrieve at least `core` and `shell` folders for minimum build
* Setup your compile toolchain configuration
* Use your compiler to compile `core/my_basic.c` and `shell/main.c`, including `core/my_basic.h` is required for both source files, then link up your own executable

The standalone interpreter supports three modes.

* Execute the binary directly without arguments to interactive with MY-BASIC
* Pass a file path to the binary to load and run that script file
* Pass an argument `-e` and an expression to evaluate and print it, eg. `-e "2 * (3 + 4)"`

### Combine with exist projects

MY-BASIC is cleanly written in a single C source file and an associated header file. Just copy `my_basic.c` and `my_basic.h` to your project folder and add them to a build configuration.

You can definitely [link with MY-BASIC as a lib](https://github.com/paladin-t/my_basic/wiki/Link-with-MY_BASIC) as well.

For more details about using MY-BASIC when it's already integrated with exist projects, please see [MY-BASIC Quick Reference](MY-BASIC%20Quick%20Reference.pdf).

## [Wiki](https://github.com/paladin-t/my_basic/wiki)

* [Passes](https://github.com/paladin-t/my_basic/wiki/Passes)
* [Interpreter workflow diagram](https://github.com/paladin-t/my_basic/wiki/Interpreter-workflow-diagram)
* [Link with MY-BASIC](https://github.com/paladin-t/my_basic/wiki/Link-with-MY_BASIC)
* [Customize a memory allocator](https://github.com/paladin-t/my_basic/wiki/Customize-a-memory-allocator)
* [Redirect PRINT and INPUT](https://github.com/paladin-t/my_basic/wiki/Redirect-PRINT-and-INPUT)
* [Redefine int_t and real_t](https://github.com/paladin-t/my_basic/wiki/Redefine-int_t-and-real_t)
* [Use usertype values](https://github.com/paladin-t/my_basic/wiki/Use-usertype-values)
* [Manipulate an array](https://github.com/paladin-t/my_basic/wiki/Manipulate-an-array)
* [Customize macros](https://github.com/paladin-t/my_basic/wiki/Customize-macros)
* [Write a debugger](https://github.com/paladin-t/my_basic/wiki/Write-a-debugger)
* [More scripting APIs](https://github.com/paladin-t/my_basic/wiki/More-scripting-APIs)
 * [String matching module](https://github.com/paladin-t/my_basic/wiki/String-matching-module)
 * [File module](https://github.com/paladin-t/my_basic/wiki/File-module)
 * [Bit operation module](https://github.com/paladin-t/my_basic/wiki/Bit-operation-module)
* [FAQ](https://github.com/paladin-t/my_basic/wiki/FAQ)

## References

* [MY-BASIC Quick Reference](MY-BASIC%20Quick%20Reference.pdf)
* [BASIC - Wikipedia](http://en.wikipedia.org/wiki/BASIC)

-----

You can support MY-BASIC development with a donation:
<br>
[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=hellotony521%40gmail%2ecom&lc=US&item_name=my-basic&no_note=0&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHostedGuest)
