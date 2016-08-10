	 __   __  __   __         _______  _______  _______  ___   _______ 
	|  |_|  ||  | |  |       |  _    ||   _   ||       ||   | |       |
	|       ||  |_|  | ____  | |_|   ||  |_|  ||  _____||   | |       |
	|       ||       ||____| |       ||       || |_____ |   | |       |
	|       ||_     _|       |  _   | |       ||_____  ||   | |      _|
	| ||_|| |  |   |         | |_|   ||   _   | _____| ||   | |     |_ 
	|_|   |_|  |___|         |_______||__| |__||_______||___| |_______|

**Copyright (C) 2011 - 2016 [Wang Renxin](https://cn.linkedin.com/pub/wang-renxin/43/494/20). All rights reserved.**

[简体中文](https://github.com/paladin-t/my_basic/wiki/%E7%94%B1%E7%BA%AF-C-%E8%AF%AD%E8%A8%80%E7%BC%96%E5%86%99%E7%9A%84-BASIC-%E8%84%9A%E6%9C%AC%E8%A7%A3%E9%87%8A%E5%99%A8)

[日本語](https://github.com/paladin-t/my_basic/wiki/BASIC-%E3%82%B9%E3%82%AF%E3%83%AA%E3%83%97%E3%83%88%E3%82%A4%E3%83%B3%E3%82%BF%E3%83%97%E3%83%AA%E3%82%BF)

Why were other script interpreters so complex? Why was it so difficult to integrate with them and use the API? Why not try MY-BASIC today! Script had never been so simple and enjoyable.

[![Build status](https://travis-ci.org/paladin-t/my_basic.svg?branch=master)](https://travis-ci.org/paladin-t/my_basic)
[![MIT license](http://img.shields.io/badge/license-MIT-brightgreen.svg)](http://opensource.org/licenses/MIT)

## Contents

* [Introduction](#introduction)
* [Compatibility](#compatibility)
* [Main features](#main-features)
* [Script at a glance](#script-at-a-glance)
* [Interpreter workflow diagram](#interpreter-workflow-diagram)
* [Installation](#installation)
* [Wiki](#wiki)
* [References](#references)
* [Support the MY-BASIC project](#support-the-my-basic-project)

## Introduction

MY-BASIC is a lightweight cross-platform easy extendable BASIC interpreter written in pure C with less than twenty thousand lines of source code. MY-BASIC is a dynamic typed programming language. It supports structured grammar, and implements a style of OOP called [prototype-based programming](https://en.wikipedia.org/wiki/Prototype-based_programming) paradigm, furthermore it offers a functional programming ability with [lambda abstraction](https://en.wikipedia.org/wiki/Anonymous_function). It is aimed to be either an embeddable scripting language or a standalone interpreter. The core is pretty light; all in a C source file and an associated header file; simpleness of source file layout and tightness dependency make it feels extraordinarily tough. Anyone even C programming newbies could learn how to add new scripting interfaces in five minutes. It's able to easily combine MY-BASIC with an existing project in C, C++, Java, Objective-C, Swift, C# and many other languages. Script driven can make your projects more powerful, elegant and neat. It's also able to learn how to build an interpreter from scratch with MY-BASIC, or build your own dialect easily based on it.

## Compatibility

It fits well on a large scale of Workstation, PC, Tablet, Pad, Mobile Phone, PDA, Video Game Console, Raspberry Pi, Intel Edison, Arduino and even MCU; totally portable to Windows, MacOS, Unix, Linux, iOS, Android, RTOS, etc.

## Main features

MY-BASIC is a dynamic typed programming language with BASIC syntax and has a very dynamic nature; it makes it easy to use and flexible. MY-BASIC offers a wide range of features including:

* It is totally **free** to use MY-BASIC for individual or commercial purpose under the MIT license
* Written in clean **ANSI C**, source code is portable for a dozen of platforms
* **Lightweight** (within memory usage less than 128KB), fast, and cuttable
* With most both retro and modern BASIC syntax
* Case-insensitive tokenization, and many other indelible BASIC feelings
* [Unicode support](https://github.com/paladin-t/my_basic/wiki/Support-for-Unicode)
* **[Prototype-based programming](https://en.wikipedia.org/wiki/Prototype-based_programming)** (OOP) paradigm, with reflection support
* **[Lambda abstraction](https://en.wikipedia.org/wiki/Anonymous_function)** enhanced functional programming
* **Dynamic typed** integer, float point, string, boolean, user defined data types, etc. with array support
* Standard numeric functions, and standard string functions
* Referenced usertype support
* Collection implementation and manipulation functions for **`LIST`** and **`DICT`**
* Automatic releasing of referenced objects (list, dictionary, referenced usertype, prototype, lambda, etc.) benefited from **Reference Counting** and **Garbage Collection**
* Multiple source file support by `IMPORT` statement
* Structured user customizable **sub routine** definition by **`DEF-ENDDEF`** support, including tail recursion optimization
* Structured `IF-THEN-ELSEIF-ELSE-ENDIF` support
* Structured `FOR-TO-STEP-NEXT/FOR-IN/WHILE-WEND/DO-UNTIL` support
* Reserved retro `GOTO/GOSUB-RETURN` support
* Debug APIs
* Customizable memory pool
* High expansibility, easy to use APIs, easy to write customized scripting interfaces
* Powerful interactive ability to manipulate script facilities at native side; or to use native functionalities in script, and vice versa
* More features under development

You may wondering if it's possible to introduce another feature to MY-BASIC, well, you may would like to take a look at [this page](https://github.com/paladin-t/my_basic/wiki/Is-it-possible-to-introduce-another-feature%3F).

## Script at a glance

Come along with a traditional "hello world" script in MY-BASIC:

~~~~~~~~~~bas
print "What is your name: "
input n$

def foo(a, b)
	return a + " " + b + " by " + n$ + "."
enddef

print foo("Hello", "world");
~~~~~~~~~~

Read the [MY-BASIC Quick Reference](MY-BASIC%20Quick%20Reference.pdf) (especially the "**Programming with BASIC**" section) to get more details about how to program in MY-BASIC.

## [Interpreter workflow diagram](https://github.com/paladin-t/my_basic/wiki/Interpreter-workflow-diagram)

It's necessary to know some principle of MY-BASIC before doing deep customization; nothing is better than a workflow diagram to get a first impression.

![](https://github.com/paladin-t/my_basic/blob/master/interpreter%20workflow%20diagram.png)

More detail are issued in the [Wiki](https://github.com/paladin-t/my_basic/wiki) pages.

## Installation

### Use standalone interpreter binary

This repository contains precompiled binaries for [Windows](output/my_basic.exe) and [MacOS](output/my_basic_mac), it's efficient to download one of them and have a first impressive playground. Or you could make a build as follow:

* Open the Visual Studio solution `my_basic.sln` on Windows to build an executable
* Open the Xcode solution `my_basic_mac.xcodeproj` on MacOS to build an MacOS executable
* If you were using a *nix OS, then use the `makefile` with a "make" toolchain to build an interpreter binary according to your specific platform

To compile an interpreter binary for your own platform manually, please follow the steps:

1. Retrieve at least `core` and `shell` folders for a minimum build
2. Setup your compile toolchain configuration
3. Compile `core/my_basic.c` and `shell/main.c`, they both require `core/my_basic.h`; then link up your own executable

The standalone interpreter supports three running modes:

* Execute the binary without arguments to enter MY-BASIC's interactive mode
* Pass a file path to the binary to load and run that script file directly
* Pass an argument `-e` followed by an expression to evaluate it and print the result, eg. `-e "2 * (3 + 4)"`

Type "HELP" and hint Enter under interactive mode to view full detail usage of the interpreter.

### Combine with exist projects

MY-BASIC is cleanly written in a single C source file and an associated header file. Just copy `my_basic.c` and `my_basic.h` to your project folder and add them to a build configuration.

You can definitely [link with MY-BASIC as a lib](https://github.com/paladin-t/my_basic/wiki/Link-with-MY_BASIC) as well.

For more details about using MY-BASIC when it has be integrated with a project, please see [MY-BASIC Quick Reference](MY-BASIC%20Quick%20Reference.pdf) or read the following Wiki pages.

## [Wiki](https://github.com/paladin-t/my_basic/wiki)

The [MY-BASIC Quick Reference](MY-BASIC%20Quick%20Reference.pdf) includes most of the fundamental topics, however, it hasn't covered everything, such as the design principle, machinism behind MY-BASIC, effective practice, etc; all of them are issued in the [Wiki](https://github.com/paladin-t/my_basic/wiki):

* Principles
 * [Language design](https://github.com/paladin-t/my_basic/wiki/Language-design)
 * [Passes](https://github.com/paladin-t/my_basic/wiki/Passes)
 * [Interpreter workflow diagram](https://github.com/paladin-t/my_basic/wiki/Interpreter-workflow-diagram)
* Code with MY-BASIC
 * [Support for Unicode](https://github.com/paladin-t/my_basic/wiki/Support-for-Unicode)
 * [Import another file](https://github.com/paladin-t/my_basic/wiki/Import-another-file)
 * [Module (namespace)](https://github.com/paladin-t/my_basic/wiki/Module-(namespace))
 * [Sub routine](https://github.com/paladin-t/my_basic/wiki/Sub-routine)
 * [Lambda abstraction](https://github.com/paladin-t/my_basic/wiki/Lambda-abstraction)
* Understanding data type system in MY-BASIC
 * [Collection manipulation](https://github.com/paladin-t/my_basic/wiki/Collection-manipulation)
 * [Manipulate an array](https://github.com/paladin-t/my_basic/wiki/Manipulate-an-array)
 * [Automatic memory management](https://github.com/paladin-t/my_basic/wiki/Automatic-memory-management)
 * [Use usertype values](https://github.com/paladin-t/my_basic/wiki/Use-usertype-values)
 * [Use prototype-based class](https://github.com/paladin-t/my_basic/wiki/Use-prototype-based-class)
 * [Define a class in C](https://github.com/paladin-t/my_basic/wiki/Define-a-class-in-C)
 * [Meta methods](https://github.com/paladin-t/my_basic/wiki/Meta-methods)
 * [Override operators](https://github.com/paladin-t/my_basic/wiki/Override-operators)
 * [Override functions](https://github.com/paladin-t/my_basic/wiki/Override-functions)
* Standalone shell
 * [Extra functions](https://github.com/paladin-t/my_basic/wiki/Extra-functions)
* Integration
 * [Link with MY-BASIC](https://github.com/paladin-t/my_basic/wiki/Link-with-MY_BASIC)
 * [Write a debugger](https://github.com/paladin-t/my_basic/wiki/Write-a-debugger)
 * [Callback](https://github.com/paladin-t/my_basic/wiki/Callback)
 * [Interop with C#](https://github.com/paladin-t/my_basic/wiki/Interop-with-C%23)
* Customization
 * [Customize macros](https://github.com/paladin-t/my_basic/wiki/Customize-macros)
 * [Customize a memory allocator](https://github.com/paladin-t/my_basic/wiki/Customize-a-memory-allocator)
 * [Redirect PRINT and INPUT](https://github.com/paladin-t/my_basic/wiki/Redirect-PRINT-and-INPUT)
 * [Redefine int_t and real_t](https://github.com/paladin-t/my_basic/wiki/Redefine-int_t-and-real_t)
 * [Customize an importer](https://github.com/paladin-t/my_basic/wiki/Customize-an-importer)
* [More scripting APIs](https://github.com/paladin-t/my_basic/wiki/More-scripting-APIs)
 * [String matching module](https://github.com/paladin-t/my_basic/wiki/String-matching-module)
 * [String manipulation module](https://github.com/paladin-t/my_basic/wiki/String-manipulation-module)
 * [File module](https://github.com/paladin-t/my_basic/wiki/File-module)
 * [Bit operation module](https://github.com/paladin-t/my_basic/wiki/Bit-operation-module)
 * [Miscellaneous module](https://github.com/paladin-t/my_basic/wiki/Miscellaneous-module)
 * [Stack module](https://github.com/paladin-t/my_basic/wiki/Stack-module)
* [FAQ](https://github.com/paladin-t/my_basic/wiki/FAQ)
 * [Is it possible to introduce another feature](https://github.com/paladin-t/my_basic/wiki/Is-it-possible-to-introduce-another-feature)

## References

* [BASIC - Wikipedia](http://en.wikipedia.org/wiki/BASIC)
* [Prototype-based programming - Wikipedia](https://en.wikipedia.org/wiki/Prototype-based_programming)
* [Lambda abstraction - Wikipedia](https://en.wikipedia.org/wiki/Anonymous_function)

## Support the MY-BASIC project

You can support MY-BASIC development with a donation:
<br>
[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=hellotony521%40gmail%2ecom&lc=US&item_name=my-basic&no_note=0&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHostedGuest)
