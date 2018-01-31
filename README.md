~~~~~~~~~~
 _____ __ __     _____ _____ _____ _____ _____ 
|     |  |  |___| __  |  _  |   __|     |     |
| | | |_   _|___| __ -|     |__   |-   -|   --|
|_|_|_| |_|     |_____|__|__|_____|_____|_____|
~~~~~~~~~~

**Copyright (C) 2011 - 2018 Wang Renxin. All rights reserved.**

[![Build status](https://travis-ci.org/paladin-t/my_basic.svg?branch=master)](https://travis-ci.org/paladin-t/my_basic)
[![MIT license](http://img.shields.io/badge/license-MIT-brightgreen.svg)](http://opensource.org/licenses/MIT)

[开发日志](http://blog.sina.com.cn/s/articlelist_1584387113_12_1.html)

## Contents

* [Introduction](#introduction)
* [Main features](#main-features)
* [BASIC at a glance](#basic-at-a-glance)
* [Awesome](#awesome)
* [Installation](#installation)
* [Interpreter workflow diagram](#interpreter-workflow-diagram)
* [Wiki](#wiki)
* [References](#references)
* [Donate/List of donors](#donatelist-of-donors)

## Introduction

MY-BASIC is a lightweight BASIC interpreter written in standard C, with only dual files. It is a dynamic typed programming language, and aimed to be embeddable, extendable and portable. MY-BASIC supports structured syntax; implements a style of OOP called [prototype-based programming](https://en.wikipedia.org/wiki/Prototype-based_programming) paradigm; and it offers a functional programming ability with [lambda abstraction](https://en.wikipedia.org/wiki/Anonymous_function). The kernel is written with a C source file and an associated header file. It's easy to either embed it or use it as a standalone interpreter. You can get how to use it and how to add new scripting interfaces in five minutes. It's possible to combine MY-BASIC with existing projects in C, C++, Java, Objective-C, Swift, C# and many other languages. Script driven can make your projects configurable, scalable and elegant. It's also possible to learn how to build an interpreter from scratch with MY-BASIC, or build your own dialect based on it.

The prefix "MY-" in the name means either literally "My" or "Make Your".

## Main features

MY-BASIC is a dynamic typed programming language with BASIC syntax and has a very dynamic nature; it makes it flexible and easy to use. MY-BASIC offers a wide range of features including:

* It is totally **free** to use MY-BASIC for commercial or noncommercial purposes under the MIT license
* Written in clean **standard C**, source code is portable to a dozen of platforms
* **Lightweight** (within less than 128KB footprint), fast, and configurable
* With both retro and modern BASIC syntax
* Case-insensitive tokenization, and many other indelible BASIC feelings
* [Unicode support](https://github.com/paladin-t/my_basic/wiki/Using-Unicode)
* **[Prototype-based programming](https://en.wikipedia.org/wiki/Prototype-based_programming)** (OOP) paradigm, with reflection support
* **[Lambda abstraction](https://en.wikipedia.org/wiki/Anonymous_function)** enhanced functional programming
* Customizable referenced usertype
* Collection construction and manipulation functions for **`LIST`** and **`DICT`**
* Automatic releasing of referenced objects (prototype, lambda, referenced usertype, list, dictionary, etc.) benefited from **Reference Counting** and **Garbage Collection**
* Dynamic typed integer, real, string, boolean, usertype, and other advanced types with array support
* Standard numeric functions, and standard string functions
* Importing multiple source files with the `IMPORT` statement
* Structured **sub routine** definition with the **`DEF/ENDDEF`** statements
* Structured `IF/THEN/ELSEIF/ELSE/ENDIF`
* Structured `FOR/TO/STEP/NEXT`, `FOR/IN/NEXT`, `WHILE/WEND`, `DO/UNTIL`
* Reserved retro `GOTO`, `GOSUB/RETURN`
* Debug API
* Customizable memory pool
* High expansibility, easy to use API, easy to extend new BASIC functions
* Powerful interactive ability to manipulate BASIC facilities at native side; or to use native functionalities in BASIC
* More features under development

I write some of my plans in the [language design](https://github.com/paladin-t/my_basic/wiki/Language-design) page.

[![Homepage](https://github.com/paladin-t/my_basic/wiki/img/basic8_banner.png)](https://paladin-t.github.io/b8/)

Get [BASIC8](https://paladin-t.github.io/b8/) on [Steam](http://store.steampowered.com/app/767240/); a **fantasy computer** for game and other program development powered by MY-BASIC!

## BASIC at a glance

Come along with a "Hello World" convention in MY-BASIC:

~~~~~~~~~~bas
input "What is your name: ", n$

def greeting(a, b)
	return a + " " + b + " by " + n$ + "."
enddef

print greeting("Hello", "world");
~~~~~~~~~~

Read the [MY-BASIC Quick Reference](https://paladin-t.github.io/my_basic/MY-BASIC%20Quick%20Reference.pdf) (especially the "**Programming with BASIC**" section) to get more details about how to program in MY-BASIC.

## Awesome

See the list for some user creations.

* [Awesome projects powered by MY-BASIC](https://my-basic.github.io/awesome/)

Don't forget to share with us about your creativity!

## Installation

### Using standalone interpreter binary

This repository contains precompiled binaries for [Windows](output/my_basic.exe) and [macOS](output/my_basic_mac), the easiest way is to download one of them to get a playground. Or you could make a build as follow:

* Open the Visual Studio solution `my_basic.sln` on Windows to build an executable
* Open the Xcode solution `my_basic_mac.xcodeproj` on macOS to build a macOS executable
* If you are using other *nix OS, use the `makefile` with a "make" toolchain to build an interpreter binary according to your specific platform

To compile an interpreter binary for your own platform manually, please follow these steps:

1. Retrieve everything under the [`core`](core) and [`shell`](shell) folders for a minimum build
2. Setup your compiling toolchain configuration
3. Compile [`core/my_basic.c`](core/my_basic.c) and [`shell/main.c`](shell/main.c), they both require [`core/my_basic.h`](core/my_basic.h); then link up an executable

The standalone interpreter supports three running modes:

* Execute the binary without arguments to enter the interactive mode
	* Type "HELP" and hint Enter to get detail usages of the interpreter
* Pass a file path to the binary to load and run that BASIC file directly
* Pass an argument `-e` followed by an expression to evaluate and print instantly, as a simple calculator, eg. `-e "2 * (3 + 4)"`

### Combining with existing projects

MY-BASIC is cleanly written in a single C source file and an associated header file. Just copy [`core/my_basic.c`](core/my_basic.c) and [`core/my_basic.h`](core/my_basic.h) to the folder of your project and add them to a build configuration.

You can definitely [link with MY-BASIC as a lib](https://github.com/paladin-t/my_basic/wiki/Linking-with-MY_BASIC) as well.

For more details about using MY-BASIC when it has been integrated with a project, see [MY-BASIC Quick Reference](https://paladin-t.github.io/my_basic/MY-BASIC%20Quick%20Reference.pdf) or read the [Wiki](#wiki) pages.

## [Interpreter workflow diagram](https://github.com/paladin-t/my_basic/wiki/Interpreter-workflow-diagram)

It's necessary to know some principle of MY-BASIC before doing deep customization; nothing's better than a workflow diagram to get a first image.

![](https://github.com/paladin-t/my_basic/wiki/img/workflow.png)

A simplest setup as follow:

~~~~~~~~~~c
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

The [MY-BASIC Quick Reference](https://paladin-t.github.io/my_basic/MY-BASIC%20Quick%20Reference.pdf) includes most of the fundamental topics, however, it hasn't covered everything, such as the design principle, machinism behind MY-BASIC, efficient practice, etc; some of these topics are mentioned in the [Wiki](https://github.com/paladin-t/my_basic/wiki):

* Principles
	* [Language design](https://github.com/paladin-t/my_basic/wiki/Language-design)
	* [How lambda works](https://github.com/paladin-t/my_basic/wiki/How-lambda-works)
	* [Passes](https://github.com/paladin-t/my_basic/wiki/Passes)
	* [Interpreter workflow diagram](https://github.com/paladin-t/my_basic/wiki/Interpreter-workflow-diagram)
* Code with MY-BASIC
	* [Using Unicode](https://github.com/paladin-t/my_basic/wiki/Using-Unicode)
	* [Importing another file](https://github.com/paladin-t/my_basic/wiki/Importing-another-file)
	* [Module (namespace)](https://github.com/paladin-t/my_basic/wiki/Module-(namespace))
	* [Sub routine](https://github.com/paladin-t/my_basic/wiki/Sub-routine)
	* [Lambda abstraction](https://github.com/paladin-t/my_basic/wiki/Lambda-abstraction)
	* [Structured exception handling](https://github.com/paladin-t/my_basic/wiki/Structured-exception-handling)
	* [Multiple condition](https://github.com/paladin-t/my_basic/wiki/Multiple-condition)
* Understanding data type system in MY-BASIC
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
* [More scripting API](https://github.com/paladin-t/my_basic/wiki/More-scripting-API)
	* [String matching module](https://github.com/paladin-t/my_basic/wiki/String-matching-module)
	* [String manipulation module](https://github.com/paladin-t/my_basic/wiki/String-manipulation-module)
	* [File module](https://github.com/paladin-t/my_basic/wiki/File-module)
	* [Inline data sequence module](https://github.com/paladin-t/my_basic/wiki/Inline-data-sequence-module)
	* [Bit operation module](https://github.com/paladin-t/my_basic/wiki/Bit-operation-module)
	* [Miscellaneous module](https://github.com/paladin-t/my_basic/wiki/Miscellaneous-module)
	* [Stack module](https://github.com/paladin-t/my_basic/wiki/Stack-module)
* [FAQ](https://github.com/paladin-t/my_basic/wiki/FAQ)
	* [Is it possible to introduce another feature](https://github.com/paladin-t/my_basic/wiki/Is-it-possible-to-introduce-another-feature)

## References

* [BASIC - Wikipedia](http://en.wikipedia.org/wiki/BASIC)
* [Prototype-based programming - Wikipedia](https://en.wikipedia.org/wiki/Prototype-based_programming)
* [Lambda abstraction - Wikipedia](https://en.wikipedia.org/wiki/Anonymous_function)

## Donate/List of donors

I need your supports to keep this project alive. Consider supporting MY-BASIC development with a donation.

One-off [donation](http://paladin-t.github.io/my_basic/donate.html) via PayPal.

<a href="http://paladin-t.github.io/my_basic/donate.html">
<img src="https://github.com/paladin-t/my_basic/wiki/img/button_donate.png" width="92">
</a>

[List of donors](http://paladin-t.github.io/my_basic/donate.html).
