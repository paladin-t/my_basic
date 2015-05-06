/*
** This source file is part of MY-BASIC
**
** For the latest info, see https://github.com/paladin-t/my_basic/
**
** Copyright (C) 2011 - 2015 W. Renxin
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of
** this software and associated documentation files (the "Software"), to deal in
** the Software without restriction, including without limitation the rights to
** use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
** the Software, and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
** FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
** IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __MY_BASIC_H__
#define __MY_BASIC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef MBAPI
#	define MBAPI
#endif /* MBAPI */

#ifndef MB_SIMPLE_ARRAY
#	define MB_SIMPLE_ARRAY
#endif /* MB_SIMPLE_ARRAY */

#ifndef MB_MAX_DIMENSION_COUNT
#	define MB_MAX_DIMENSION_COUNT 4
#endif /* MB_MAX_DIMENSION_COUNT */

#ifndef MB_ENABLE_ALLOC_STAT
#	define MB_ENABLE_ALLOC_STAT
#endif /* MB_ENABLE_ALLOC_STAT */

#ifndef MB_ENABLE_SOURCE_TRACE
#   define MB_ENABLE_SOURCE_TRACE
#endif /* MB_ENABLE_SOURCE_TRACE */

#ifndef MB_COMPACT_MODE
#	define MB_COMPACT_MODE
#endif /* MB_COMPACT_MODE */

#ifdef MB_COMPACT_MODE
#	pragma pack(1)
#endif /* MB_COMPACT_MODE */

#ifndef true
#	define true (!0)
#endif
#ifndef false
#	define false (0)
#endif

#ifndef bool_t
#	define bool_t int
#endif
#ifndef int_t
#	define int_t int
#endif
#ifndef real_t
#	define real_t float
#endif

#ifndef mb_strtol
#	define mb_strtol(__s, __e, __r) strtol((__s), (__e), (__r))
#endif
#ifndef mb_strtod
#	define mb_strtod(__s, __e) strtod((__s), (__e))
#endif

#ifndef MB_INT_FMT
#	define MB_INT_FMT "%d"
#endif
#ifndef MB_REAL_FMT
#	define MB_REAL_FMT "%g"
#endif

#ifndef MB_FNAN
#	define MB_FNAN 0xffc00000
#endif
#ifndef MB_FINF
#	define MB_FINF 0x7f800000
#endif

#ifndef MB_EOS
#	define MB_EOS '\n'
#endif
#ifndef MB_NULL_STRING
#	define MB_NULL_STRING "(empty)"
#endif

#ifndef _MSC_VER
#	ifndef _strcmpi
#		ifdef __BORLANDC__
#			define _strcmpi stricmp
#		elif defined __POCC__
#			define _strcmpi _stricmp
#		else
#			define _strcmpi strcasecmp
#		endif
#	endif /* _strcmpi */
#endif /* _MSC_VER */

#ifndef mb_assert
#	define mb_assert(__a) do { ((void)(__a)); assert(__a); } while(0)
#endif /* mb_assert */

#ifndef mb_static_assert
#	define _static_assert_impl(cond, msg) typedef char static_assertion_##msg[(!!(cond)) * 2 - 1]
#	define _compile_time_assert3(x, l) _static_assert_impl(x, static_assertion_at_line_##l)
#	define _compile_time_assert2(x, l) _compile_time_assert3(x, l)
#	define mb_static_assert(x) _compile_time_assert2(x, __LINE__)
#endif /* mb_static_assert */

#ifndef mb_unrefvar
#	define mb_unrefvar(__v) ((void)(__v))
#endif /* mb_unrefvar */

#ifndef MB_CODES
#	define MB_CODES
#	define MB_FUNC_OK 0
#	define MB_FUNC_BYE 1001
#	define MB_FUNC_WARNING 1002
#	define MB_FUNC_ERR 1003
#	define MB_FUNC_END 1004
#	define MB_FUNC_SUSPEND 1005
#	define MB_PARSING_ERR 3001
#	define MB_LOOP_BREAK 5001
#	define MB_LOOP_CONTINUE 5002
#	define MB_SUB_RETURN 5101
#	define MB_DEBUG_ID_NOT_FOUND 7001
#	define MB_EXTENDED_ABORT 9001
#endif /* MB_CODES */

#ifndef mb_check
#	define mb_check(__r) { int __hr = __r; if(__hr != MB_FUNC_OK) { return __hr; } }
#endif /* mb_check */

#ifndef mb_reg_fun
#	define mb_reg_fun(__s, __f) mb_register_func(__s, #__f, __f)
#endif /* mb_reg_fun */
#ifndef mb_rem_fun
#	define mb_rem_fun(__s, __f) mb_remove_func(__s, #__f)
#endif /* mb_rem_fun */
#ifndef mb_rem_res_fun
#	define mb_rem_res_fun(__s, __f) mb_remove_reserved_func(__s, #__f)
#endif /* mb_rem_res_fun */

struct mb_interpreter_t;

typedef enum mb_error_e {
	SE_NO_ERR = 0,
	/** Common */
	SE_CM_MB_OPEN_FAILED,
	SE_CM_FUNC_EXISTS,
	SE_CM_FUNC_NOT_EXISTS,
	/** Parsing */
	SE_PS_FILE_OPEN_FAILED,
	SE_PS_SYMBOL_TOO_LONG,
	SE_PS_INVALID_CHAR,
	/** Running */
	SE_RN_NOT_SUPPORTED,
	SE_RN_EMPTY_PROGRAM,
	SE_RN_SYNTAX,
	SE_RN_INVALID_DATA_TYPE,
	SE_RN_TYPE_NOT_MATCH,
	SE_RN_ILLEGAL_BOUND,
	SE_RN_DIMENSION_TOO_MUCH,
	SE_RN_OPERATION_FAILED,
	SE_RN_DIMENSION_OUT_OF_BOUND,
	SE_RN_ARRAY_OUT_OF_BOUND,
	SE_RN_LABEL_NOT_EXISTS,
	SE_RN_NO_RETURN_POINT,
	SE_RN_COLON_EXPECTED,
	SE_RN_COMMA_OR_SEMICOLON_EXPECTED,
	SE_RN_ARRAY_IDENTIFIER_EXPECTED,
	SE_RN_OPEN_BRACKET_EXPECTED,
	SE_RN_CLOSE_BRACKET_EXPECTED,
	SE_RN_ARRAY_SUBSCRIPT_EXPECTED,
	SE_RN_STRUCTURE_NOT_COMPLETED,
	SE_RN_FUNCTION_EXPECTED,
	SE_RN_VAR_OR_ARRAY_EXPECTED,
	SE_RN_ASSIGN_OPERATOR_EXPECTED,
	SE_RN_STRING_EXPECTED,
	SE_RN_NUMBER_EXPECTED,
	SE_RN_INTEGER_EXPECTED,
	SE_RN_ELSE_EXPECTED,
	SE_RN_TO_EXPECTED,
	SE_RN_NEXT_EXPECTED,
	SE_RN_UNTIL_EXPECTED,
	SE_RN_LOOP_VAR_EXPECTED,
	SE_RN_JUMP_LABEL_EXPECTED,
	SE_RN_VARIABLE_EXPECTED,
	SE_RN_INVALID_ID_USAGE,
	SE_RN_OPERATOR_EXPECTED,
	SE_RN_CALCULATION_ERROR,
	SE_RN_DIVIDE_BY_ZERO,
	SE_RN_MOD_BY_ZERO,
	SE_RN_INVALID_EXPRESSION,
	SE_RN_OUT_OF_MEMORY,
	/** Extended abort */
	SE_EA_EXTENDED_ABORT,
	/** Extra */
	SE_COUNT
} mb_error_e;

typedef enum mb_data_e {
	MB_DT_NIL = -1,
	MB_DT_INT = 0,
	MB_DT_REAL,
	MB_DT_STRING,
	MB_DT_USERTYPE
} mb_data_e;

typedef union mb_value_u {
	int_t integer;
	real_t float_point;
	char* string;
	void* usertype;
} mb_value_u;

typedef struct mb_value_t {
	mb_data_e type;
	mb_value_u value;
} mb_value_t;

typedef int (* mb_func_t)(struct mb_interpreter_t*, void**);
typedef void (* mb_debug_stepped_handler_t)(struct mb_interpreter_t*, int, unsigned short, unsigned short);
typedef void (* mb_error_handler_t)(struct mb_interpreter_t*, enum mb_error_e, char*, int, unsigned short, unsigned short, int);
typedef int (* mb_print_func_t)(const char*, ...);
typedef int (* mb_input_func_t)(char*, int);

MBAPI unsigned int mb_ver(void);
MBAPI const char* mb_ver_string(void);

MBAPI int mb_init(void);
MBAPI int mb_dispose(void);
MBAPI int mb_open(struct mb_interpreter_t** s);
MBAPI int mb_close(struct mb_interpreter_t** s);
MBAPI int mb_reset(struct mb_interpreter_t** s, bool_t clrf);

MBAPI int mb_register_func(struct mb_interpreter_t* s, const char* n, mb_func_t f);
MBAPI int mb_remove_func(struct mb_interpreter_t* s, const char* n);
MBAPI int mb_remove_reserved_func(struct mb_interpreter_t* s, const char* n);

MBAPI int mb_attempt_func_begin(struct mb_interpreter_t* s, void** l);
MBAPI int mb_attempt_func_end(struct mb_interpreter_t* s, void** l);
MBAPI int mb_attempt_open_bracket(struct mb_interpreter_t* s, void** l);
MBAPI int mb_attempt_close_bracket(struct mb_interpreter_t* s, void** l);
MBAPI int mb_has_arg(struct mb_interpreter_t* s, void** l);
MBAPI int mb_pop_int(struct mb_interpreter_t* s, void** l, int_t* val);
MBAPI int mb_pop_real(struct mb_interpreter_t* s, void** l, real_t* val);
MBAPI int mb_pop_string(struct mb_interpreter_t* s, void** l, char** val);
MBAPI int mb_pop_usertype(struct mb_interpreter_t* s, void** l, void** val);
MBAPI int mb_pop_value(struct mb_interpreter_t* s, void** l, mb_value_t* val);
MBAPI int mb_push_int(struct mb_interpreter_t* s, void** l, int_t val);
MBAPI int mb_push_real(struct mb_interpreter_t* s, void** l, real_t val);
MBAPI int mb_push_string(struct mb_interpreter_t* s, void** l, char* val);
MBAPI int mb_push_usertype(struct mb_interpreter_t* s, void** l, void* val);
MBAPI int mb_push_value(struct mb_interpreter_t* s, void** l, mb_value_t val);
MBAPI int mb_dispose_value(struct mb_interpreter_t* s, mb_value_t val);

MBAPI int mb_load_string(struct mb_interpreter_t* s, const char* l);
MBAPI int mb_load_file(struct mb_interpreter_t* s, const char* f);
MBAPI int mb_run(struct mb_interpreter_t* s);
MBAPI int mb_suspend(struct mb_interpreter_t* s, void** l);

MBAPI int mb_debug_get(struct mb_interpreter_t* s, const char* n, mb_value_t* val);
MBAPI int mb_debug_set(struct mb_interpreter_t* s, const char* n, mb_value_t val);
MBAPI int mb_debug_set_stepped_handler(struct mb_interpreter_t* s, mb_debug_stepped_handler_t h);

MBAPI mb_error_e mb_get_last_error(struct mb_interpreter_t* s);
MBAPI const char* mb_get_error_desc(mb_error_e err);
MBAPI int mb_set_error_handler(struct mb_interpreter_t* s, mb_error_handler_t h);
MBAPI int mb_set_printer(struct mb_interpreter_t* s, mb_print_func_t p);
MBAPI int mb_set_inputer(struct mb_interpreter_t* s, mb_input_func_t p);

MBAPI int mb_gets(char* buf, int s);

MBAPI char* mb_memdup(char* val, unsigned size);

#ifdef MB_COMPACT_MODE
#	pragma pack()
#endif /* MB_COMPACT_MODE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MY_BASIC_H__ */
