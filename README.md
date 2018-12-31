~~~~~~~~~~
 _____ __ __     _____ _____ _____ _____ _____ 
|     |  |  |___| __  |  _  |   __|     |     |
| | | |_   _|___| __ -|     |__   |-   -|   --|
|_|_|_| |_|     |_____|__|__|_____|_____|_____|
~~~~~~~~~~

**Copyright (C) 2011 - 2019 Wang Renxin**

[![Build status](https://travis-ci.org/paladin-t/my_basic.svg?branch=master)](https://travis-ci.org/paladin-t/my_basic)
[![MIT license](http://img.shields.io/badge/license-MIT-brightgreen.svg)](http://opensource.org/licenses/MIT)

[Web playground](https://my-basic.github.io/playground/output/index.html)

## Contents

* [Introduction](#introduction)
* [Main features](#main-features)
* [BASIC at a glance](#basic-at-a-glance)
* [Installation](#installation)
* [Interpreter workflow diagram](#interpreter-workflow-diagram)
* [Wiki](#wiki)
* [Donate](#donate)

## Introduction

MY-BASIC is a lightweight BASIC interpreter written in standard C in dual files. It's aimed to be embeddable, extendable and portable. It is a dynamic typed programming language, reserves structured syntax, supports a style of [prototype-based programming](https://en.wikipedia.org/wiki/Prototype-based_programming) (OOP), also implements a functional paradigm by [lambda abstraction](https://en.wikipedia.org/wiki/Anonymous_function). The core is written in a C source file and an associated header file. It's easy to either use it as a standalone interpreter or embed it with existing projects in C, C++, Java, Objective-C, Swift, C#, etc. and totally customizable by adding your own scripting interfaces.

## Main features

MY-BASIC offers a wide range of features including:

* Written in standard C, source code is portable to a dozen of platforms
* Lightweight (within less than 128KB footprint), fast, and configurable
* With both retro and modern BASIC syntax
* Case-insensitive tokenization, and many other indelible BASIC flavour
* [Unicode support](https://github.com/paladin-t/my_basic/wiki/Using-Unicode)
* [Prototype-based programming](https://en.wikipedia.org/wiki/Prototype-based_programming), with reflection support
* [Lambda abstraction](https://en.wikipedia.org/wiki/Anonymous_function) enhanced functional programming
* Customizable referenced/non-referenced usertype
* Collection construction and manipulation functions for `LIST` and `DICT`
* Automatic releasing for referenced values (prototype, lambda, referenced usertype, list, dictionary, etc.) benefited from reference counting and garbage collection
* Common numeric and string functions
* Structured sub routine definition with the `DEF/ENDDEF` statements
* Structured `IF/THEN/ELSEIF/ELSE/ENDIF`
* Structured `FOR/TO/STEP/NEXT`, `FOR/IN/NEXT`, `WHILE/WEND`, `DO/UNTIL`
* Reserved retro `GOTO`, `GOSUB/RETURN`
* Importing multiple source files with the `IMPORT` statement
* Debug API
* Customizable memory pool
* Easy API, for extending new BASIC functions
* Easy interacting BASIC facilities at native side, and vice versa
* More features under development

[![BASIC8](https://github.com/paladin-t/my_basic/wiki/img/basic8_banner.png)](https://paladin-t.github.io/b8/)

Get [BASIC8](https://paladin-t.github.io/b8/) - the **fantasy computer** powered by MY-BASIC - on [Steam](http://store.steampowered.com/app/767240/) for game and other program development in an integrated environment.

See awesome [user creations](https://my-basic.github.io/awesome/).

## BASIC at a glance

A "Hello World" convention in MY-BASIC:

~~~~~~~~~~bas
input "What is your name: ", n$

def greeting(a, b)
	return a + " " + b + " by " + n$ + "."
enddef

print greeting("Hello", "world");
~~~~~~~~~~

Read the [MY-BASIC Quick Reference](https://paladin-t.github.io/my_basic/MY-BASIC%20Quick%20Reference.pdf) to get details about how to program in MY-BASIC.

## Installation

### Using standalone interpreter binary

This repository contains precompiled binaries for [Windows](output/my_basic.exe) and [macOS](output/my_basic_mac), the easiest way is to download to get a direct playground. Or you can make a build by:

* Using the Visual Studio workspace `my_basic.sln` on Windows to build an executable
* Using the Xcode workspace `my_basic_mac.xcodeproj` on macOS to build an executable
* Using the `makefile` on *nix OS to build an executable

Follow these steps to compile an interpreter binary manually for any platform:

1. Retrieve everything under the [`core`](core) and [`shell`](shell) folders for a minimum build
2. Setup your toolchain for compiling and linking
3. Compile [`core/my_basic.c`](core/my_basic.c) and [`shell/main.c`](shell/main.c), with both including [`core/my_basic.h`](core/my_basic.h); then link up an executable

The standalone interpreter supports three running modes:

* Execute the binary without arguments to use the interactive mode
	* Type "HELP" and hint Enter to get usages of it
* Pass a file to the binary to load and run that BASIC source code
* Pass an argument `-e` followed with an expression to evaluate and print instantly as a simple calculator, eg. `-e "2 * (3 + 4)"`

### Combining with existing projects

Just copy [`core/my_basic.c`](core/my_basic.c) and [`core/my_basic.h`](core/my_basic.h) to your project and add them to the build configuration. You can [link with MY-BASIC as a lib](https://github.com/paladin-t/my_basic/wiki/Linking-with-MY_BASIC) as well.

For details about using MY-BASIC after integration, see [MY-BASIC Quick Reference](https://paladin-t.github.io/my_basic/MY-BASIC%20Quick%20Reference.pdf) and read the [Wiki](#wiki) pages.

## [Interpreter workflow diagram](https://github.com/paladin-t/my_basic/wiki/Interpreter-workflow-diagram)

It's recommended to know the basic principle of MY-BASIC before customizing; nothing's better than a workflow diagram to get a first image.

![](https://github.com/paladin-t/my_basic/wiki/img/workflow.png)

A simple setup as follow:

~~~~~~~~~~c
#include "my_basic.h"

int main() {
	struct mb_interpreter_t* bas = NULL;

	mb_init();
	mb_open(&bas);
	mb_load_string(bas, "print 22 / 7;", true);
	mb_run(bas, true);
	mb_close(&bas);
	mb_dispose();

	return 0;
}
~~~~~~~~~~

## [Wiki](https://github.com/paladin-t/my_basic/wiki)

The manual explains most of the fundamental topics, however it doesn't cover everything; read the [Wiki](https://github.com/paladin-t/my_basic/wiki) as supplement instructions, such as machinism behind MY-BASIC, efficient practice, etc:

* Principles
	* [Passes](https://github.com/paladin-t/my_basic/wiki/Passes)
	* [Interpreter workflow diagram](https://github.com/paladin-t/my_basic/wiki/Interpreter-workflow-diagram)
	* [How lambda works](https://github.com/paladin-t/my_basic/wiki/How-lambda-works)
* Coding
	* [Using Unicode](https://github.com/paladin-t/my_basic/wiki/Using-Unicode)
	* [Importing another file](https://github.com/paladin-t/my_basic/wiki/Importing-another-file)
	* [Module (namespace)](https://github.com/paladin-t/my_basic/wiki/Module-(namespace))
	* [Sub routine](https://github.com/paladin-t/my_basic/wiki/Sub-routine)
	* [Lambda abstraction](https://github.com/paladin-t/my_basic/wiki/Lambda-abstraction)
	* [Structured exception handling](https://github.com/paladin-t/my_basic/wiki/Structured-exception-handling)
	* [Multiple condition](https://github.com/paladin-t/my_basic/wiki/Multiple-condition)
* Data types
	* [Collection manipulation](https://github.com/paladin-t/my_basic/wiki/Collection-manipulation)
	* [Manipulating an array](https://github.com/paladin-t/my_basic/wiki/Manipulating-an-array)
	* [Automatic memory management](https://github.com/paladin-t/my_basic/wiki/Automatic-memory-management)
	* [Using usertype values](https://github.com/paladin-t/my_basic/wiki/Using-usertype-values)
	* [Using prototype-based class](https://github.com/paladin-t/my_basic/wiki/Using-prototype-based-class)
	* [Defining a class in C](https://github.com/paladin-t/my_basic/wiki/Defining-a-class-in-C)
	* [Meta methods](https://github.com/paladin-t/my_basic/wiki/Meta-methods)
	* [Overriding operators](https://github.com/paladin-t/my_basic/wiki/Overriding-operators)
	* [Overriding functions](https://github.com/paladin-t/my_basic/wiki/Overriding-functions)
	* [Using iterators](https://github.com/paladin-t/my_basic/wiki/Using-iterators)
* Standalone shell
	* [Extra functions](https://github.com/paladin-t/my_basic/wiki/Extra-functions)
* Integration
	* [Linking with MY-BASIC](https://github.com/paladin-t/my_basic/wiki/Linking-with-MY_BASIC)
	* [Writing a debugger](https://github.com/paladin-t/my_basic/wiki/Writing-a-debugger)
	* [Callback](https://github.com/paladin-t/my_basic/wiki/Callback)
	* [Interop with C#](https://github.com/paladin-t/my_basic/wiki/Interop-with-C%23)
* Customization
	* [Customizing macros](https://github.com/paladin-t/my_basic/wiki/Customizing-macros)
	* [Customizing a memory allocator](https://github.com/paladin-t/my_basic/wiki/Customizing-a-memory-allocator)
	* [Redirecting PRINT and INPUT](https://github.com/paladin-t/my_basic/wiki/Redirecting-PRINT-and-INPUT)
	* [Redefining int_t and real_t](https://github.com/paladin-t/my_basic/wiki/Redefining-int_t-and-real_t)
	* [Converting between string and real](https://github.com/paladin-t/my_basic/wiki/Converting-between-string-and-real)
	* [Customizing an importer](https://github.com/paladin-t/my_basic/wiki/Customizing-an-importer)
* More scripting API
	* [String matching module](https://github.com/paladin-t/my_basic/wiki/String-matching-module)
	* [String manipulation module](https://github.com/paladin-t/my_basic/wiki/String-manipulation-module)
	* [File module](https://github.com/paladin-t/my_basic/wiki/File-module)
	* [Inline data sequence module](https://github.com/paladin-t/my_basic/wiki/Inline-data-sequence-module)
	* [Bit operation module](https://github.com/paladin-t/my_basic/wiki/Bit-operation-module)
	* [Miscellaneous module](https://github.com/paladin-t/my_basic/wiki/Miscellaneous-module)
	* [Stack module](https://github.com/paladin-t/my_basic/wiki/Stack-module)
* [FAQ](https://github.com/paladin-t/my_basic/wiki/FAQ)
	* [Is it possible to introduce another feature](https://github.com/paladin-t/my_basic/wiki/Is-it-possible-to-introduce-another-feature)

## Donate

[List of donors](http://paladin-t.github.io/my_basic/donate.html).

Consider supporting MY-BASIC development with a donation if you like this project.

One-off [donation](http://paladin-t.github.io/my_basic/donate.html) via PayPal.

<a href="http://paladin-t.github.io/my_basic/donate.html">
<img src="https://github.com/paladin-t/my_basic/wiki/img/button_donate.png" width="92">
</a>
