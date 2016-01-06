/*
** This source file is part of MY-BASIC
**
** For the latest info, see https://github.com/paladin-t/my_basic/
**
** Copyright (C) 2011 - 2016 Wang Renxin
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

#ifdef _MSC_VER
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS
#	endif /* _CRT_SECURE_NO_WARNINGS */
#endif /* _MSC_VER */

#include "my_basic.h"
#ifdef _MSC_VER
#	include <conio.h>
#	include <malloc.h>
#else /* _MSC_VER */
#	include <stdint.h>
#endif /* _MSC_VER */
#include <limits.h>
#ifndef ARDUINO
#	include <memory.h>
#endif /* ARDUINO */
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4127)
#	pragma warning(disable : 4305)
#	pragma warning(disable : 4309)
#	pragma warning(disable : 4805)
#	pragma warning(disable : 4996)
#endif /* _MSC_VER */

#ifdef __APPLE__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wunused-function"
#	pragma clang diagnostic ignored "-Wunused-variable"
#endif /* __APPLE__ */

#ifdef __BORLANDC__
#	pragma warn -8004
#	pragma warn -8008
#	pragma warn -8012
#endif /* __BORLANDC__ */

#ifdef MB_COMPACT_MODE
#	pragma pack(1)
#endif /* MB_COMPACT_MODE */

/*
** {========================================================
** Data type declarations
*/

/** Macros */
#define _VER_MAJOR 1
#define _VER_MINOR 2
#define _VER_REVISION 0
#define _VER_SUFFIX
#define _MB_VERSION ((_VER_MAJOR * 0x01000000) + (_VER_MINOR * 0x00010000) + (_VER_REVISION))
#define _STRINGIZE(A) _MAKE_STRINGIZE(A)
#define _MAKE_STRINGIZE(A) #A
#if _VER_REVISION == 0
#	define _MB_VERSION_STRING _STRINGIZE(_VER_MAJOR._VER_MINOR _VER_SUFFIX)
#else /* _VER_REVISION == 0 */
#	define _MB_VERSION_STRING _STRINGIZE(_VER_MAJOR._VER_MINOR._VER_REVISION _VER_SUFFIX)
#endif /* _VER_REVISION == 0 */

/* Define as 1 to treat warning as error, 0 just leave it */
#define _WARING_AS_ERROR 0

/* Define as 1 to create class instance without variable, 0 to put class instance into a variable */
#define _CLASS_IN_VAR 1

/* Define as 1 to use a comma to PRINT a new line, 0 to use a semicolon */
#define _COMMA_AS_NEWLINE 0

#define _NO_EAT_COMMA 2

/* Helper */
#ifndef sgn
#	define sgn(__v) ((__v) ? ((__v) > 0 ? 1 : -1) : (0))
#endif /* sgn */

#ifndef _countof
#	define _countof(__a) (sizeof(__a) / sizeof(*(__a)))
#endif /* _countof */

#ifndef islower
#	define islower(__c) ((__c) >= 'a' && (__c) <= 'z')
#endif /* islower */

#ifndef toupper
#	define toupper(__c) ((islower(__c)) ? ((__c) - 'a' + 'A') : (__c))
#endif /* toupper */

#define _copy_bytes(__l, __r) do { memcpy((__l), (__r), sizeof(mb_val_bytes_t)); } while(0)
#define _cmp_bytes(__l, __r) (memcmp((__l), (__r), sizeof(mb_val_bytes_t)))

#define _mb_check(__expr, __exit) do { if((__expr) != MB_FUNC_OK) goto __exit; } while(0)
#define _mb_check_mark(__expr, __err, __exit) do { __err |= (__expr) != MB_FUNC_OK; if(__err) goto __exit; } while(0)

#define DON(__o) ((__o) ? ((_object_t*)((__o)->data)) : 0)
#define TON(__t) (((__t) && *(__t)) ? ((_object_t*)(((_tuple3_t*)(*(__t)))->e1)) : 0)

#define _IS_EOS(__o) (__o && ((_object_t*)(__o))->type == _DT_EOS)
#define _IS_SEP(__o, __c) (((_object_t*)(__o))->type == _DT_SEP && ((_object_t*)(__o))->data.separator == __c)
#define _IS_FUNC(__o, __f) (((_object_t*)(__o))->type == _DT_FUNC && ((_object_t*)(__o))->data.func->pointer == __f)
#define _IS_UNARY_FUNC(__o) (((_object_t*)(__o))->type == _DT_FUNC && _is_unary(((_object_t*)(__o))->data.func->pointer))
#define _IS_VAR(__o) ((__o) && ((_object_t*)(__o))->type == _DT_VAR)
#ifdef MB_ENABLE_COLLECTION_LIB
#	define _IS_LIST(__o) ((__o) && ((_object_t*)(__o))->type == _DT_LIST)
#	define _IS_DICT(__o) ((__o) && ((_object_t*)(__o))->type == _DT_DICT)
#	define _IS_COLL(__o) (_IS_LIST(__o) || _IS_DICT(__o))
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
#	define _IS_CLASS(__o) ((__o) && ((_object_t*)(__o))->type == _DT_CLASS)
#	define _GET_CLASS(__o) ((!__o) ? 0 : (_IS_CLASS(__o) ? (__o) : (_IS_VAR(__o) && _IS_CLASS((__o)->data.variable->data) ? (__o)->data.variable->data : 0)))
#endif /* MB_ENABLE_CLASS */
#define _IS_ROUTINE(__o) ((__o) && ((_object_t*)(__o))->type == _DT_ROUTINE)

/* Hash table size */
#define _HT_ARRAY_SIZE_TINY 1
#define _HT_ARRAY_SIZE_SMALL 193
#define _HT_ARRAY_SIZE_MID 1543
#define _HT_ARRAY_SIZE_BIG 12289
#define _HT_ARRAY_SIZE_DEFAULT _HT_ARRAY_SIZE_SMALL

/* Max length of a single symbol */
#define _SINGLE_SYMBOL_MAX_LENGTH 128

#define _META_LIST_MAX_DEPTH UINT_MAX

typedef int (* _common_compare)(void*, void*);

/* Container operation */
#define _OP_RESULT_NORMAL 0
#define _OP_RESULT_DEL_NODE -1
typedef int (* _common_operation)(void*, void*);

/** List */
typedef _common_compare _ls_compare;
typedef _common_operation _ls_operation;

typedef struct _ls_node_t {
	void* data;
	struct _ls_node_t* prev;
	struct _ls_node_t* next;
	void* extra;
} _ls_node_t;

/** Dictionary */
typedef unsigned int (* _ht_hash)(void*, void*);
typedef _common_compare _ht_compare;
typedef _common_operation _ht_operation;

typedef struct _ht_node_t {
	_ls_operation free_extra;
	_ht_compare compare;
	_ht_hash hash;
	unsigned int array_size;
	unsigned int count;
	_ls_node_t** array;
} _ht_node_t;

/** Normal enum/struct/union/const, etc. */
/* Error description text */
static const char* _ERR_DESC[] = {
	"No error",
	/** Common */
	"Open MY-BASIC failed",
	"A function with the same name already exists",
	"A function with the name does not exists",
	/** Parsing */
	"Open file failed",
	"Symbol too long",
	"Invalid character",
	/** Running */
	"Not supported",
	"Empty program",
	"Syntax error",
	"Invalid data type",
	"Type does not match",
	"Invalid string",
	"Index out of bound",
	"Cannot find with given index",
	"Illegal bound",
	"Too much dimensions",
	"Operation failed",
	"Dimension count out of bound",
	"Out of bound",
	"Label does not exist",
	"No return point",
	"Colon expected",
	"Comma or semicolon expected",
	"Array identifier expected",
	"Open bracket expected",
	"Close bracket expected",
	"Array subscript expected",
	"Nested too deep",
	"Structure not completed",
	"Function expected",
	"Variable or array identifier expected",
	"Assign operator expected",
	"String expected",
	"Number expected",
	"Integer expected",
	"ELSE statement expected",
	"TO statement expected",
	"NEXT statement expected",
	"UNTIL statement expected",
	"Loop variable expected",
	"Jump label expected",
	"Variable expected",
	"Invalid identifier usage",
	"Duplicate identifier",
	"Operator expected",
	"Calculation error",
	"Divide by zero",
	"MOD by zero",
	"Invalid expression",
	"Out of memory",
	"Wrong function reached",
	"Don't suspend in a routine",
	"Don't mix instructional and structured sub routines",
	"Invalid routine",
	"Routine expected",
	"Duplicate routine",
	"Invalid class",
	"Class expected",
	"Duplicate class",
	"Wrong meta class",
	"Collection expected",
	"Iterator expected",
	"Collection or iterator expected",
	"Invalid iterator",
	"Empty collection",
	"Referenced type expected",
	"Stack trace disabled",
	/** Extended abort */
	"Extended abort"
};

mb_static_assert(_countof(_ERR_DESC) == SE_COUNT);

/* Data type */
typedef enum _data_e {
	_DT_INVALID = -1,
	_DT_NIL = 0,
	_DT_UNKNOWN,
	_DT_TYPE,
	_DT_INT,
	_DT_REAL,
	_DT_STRING,
	_DT_USERTYPE,
	_DT_USERTYPE_REF,
	_DT_FUNC,
	_DT_VAR,
	_DT_ARRAY,
#ifdef MB_ENABLE_COLLECTION_LIB
	_DT_LIST,
	_DT_LIST_IT,
	_DT_DICT,
	_DT_DICT_IT,
#endif /* MB_ENABLE_COLLECTION_LIB */
	_DT_LABEL, /* Label type, used for GOTO, GOSUB statement */
#ifdef MB_ENABLE_CLASS
	_DT_CLASS, /* Object instance */
#endif /* MB_ENABLE_CLASS */
	_DT_ROUTINE, /* User defined sub routine in script */
	_DT_SEP, /* Separator */
	_DT_EOS /* End of statement */
} _data_e;

typedef struct _func_t {
	char* name;
	mb_func_t pointer;
} _func_t;

typedef struct _var_t {
	char* name;
	struct _object_t* data;
#ifdef MB_ENABLE_CLASS
	int pathing;
#endif /* MB_ENABLE_CLASS */
} _var_t;

struct _ref_t;

typedef void (* _unref_func_t)(struct _ref_t*, void*);

typedef unsigned int _ref_count_t;

typedef struct _ref_t {
	_ref_count_t* count;
	_unref_func_t on_unref;
	_data_e type;
	struct mb_interpreter_t* s;
} _ref_t;

typedef short _lock_t;

#ifdef MB_ENABLE_GC
typedef struct _gc_t {
	_ht_node_t* table;
	_ht_node_t* recursive_table;
	_ht_node_t* collected_table;
	int_t collecting;
} _gc_t;
#endif /* MB_ENABLE_GC */

typedef struct _usertype_ref_t {
	_ref_t ref;
	void* usertype;
	mb_dtor_func_t dtor;
	mb_clone_func_t clone;
	mb_hash_func_t hash;
	mb_cmp_func_t cmp;
	mb_fmt_func_t fmt;
} _usertype_ref_t;

typedef struct _array_t {
	_ref_t ref;
	char* name;
	_data_e type;
#ifndef MB_SIMPLE_ARRAY
	_data_e* types;
#endif /* MB_SIMPLE_ARRAY */
	void* raw;
	unsigned int count;
	int dimension_count;
	int dimensions[MB_MAX_DIMENSION_COUNT];
} _array_t;

#ifdef MB_ENABLE_COLLECTION_LIB
typedef struct _list_t {
	_ref_t ref;
	_ls_node_t* list;
	_lock_t lock;
	_ls_node_t* cached_node;
	int cached_index;
	unsigned int count;
} _list_t;

typedef struct _list_it_t {
	_list_t* list;
	bool_t locking;
	_ls_node_t* curr;
} _list_it_t;

typedef struct _dict_t {
	_ref_t ref;
	_ht_node_t* dict;
	_lock_t lock;
} _dict_t;

typedef struct _dict_it_t {
	_dict_t* dict;
	bool_t locking;
	unsigned int curr_bucket;
	_ls_node_t* curr_node;
} _dict_it_t;
#endif /* MB_ENABLE_COLLECTION_LIB */

typedef struct _label_t {
	char* name;
	_ls_node_t* node;
} _label_t;

#ifdef MB_ENABLE_CLASS
typedef struct _class_t {
	_ref_t ref;
	char* name;
	_ls_node_t* meta_list;
	struct _running_context_t* scope;
	void* userdata;
} _class_t;
#endif /* MB_ENABLE_CLASS */

typedef enum _invokable_e {
	_IT_BASIC,
#ifdef MB_ENABLE_LAMBDA
	_IT_LAMBDA,
#endif /* MB_ENABLE_LAMBDA */
	_IT_NATIVE
} _invokable_e;

#ifdef MB_ENABLE_LAMBDA
typedef struct _running_context_ref_t {
	_ref_t ref;
	struct _running_context_t* running;
} _running_context_ref_t;
#endif /* MB_ENABLE_LAMBDA */

typedef struct _routine_t {
	char* name;
#ifdef MB_ENABLE_CLASS
	_class_t* instance;
#endif /* MB_ENABLE_CLASS */
	bool_t is_cloned;
	_invokable_e type;
	union {
		struct {
			struct _running_context_t* scope;
			_ls_node_t* entry;
			_ls_node_t* parameters;
		} basic;
#ifdef MB_ENABLE_LAMBDA
		struct {
			_ref_t ref;
			struct _running_context_ref_t* scope;
			_ls_node_t* entry;
			_ls_node_t* parameters;
			_ls_node_t* upvalues;
		} lambda;
#endif /* MB_ENABLE_LAMBDA */
		struct {
			mb_routine_func_t entry;
		} native;
	} func;
} _routine_t;

typedef union _raw_u { char c; int_t i; real_t r; void* p; mb_val_bytes_t b; } _raw_u;

typedef unsigned char _raw_t[sizeof(_raw_u)];

typedef struct _object_t {
	_data_e type;
	union {
		mb_data_e type;
		int_t integer;
		real_t float_point;
		char* string;
		void* usertype;
		_usertype_ref_t* usertype_ref;
		_func_t* func;
		_var_t* variable;
		_array_t* array;
#ifdef MB_ENABLE_COLLECTION_LIB
		_list_t* list;
		_list_it_t* list_it;
		_dict_t* dict;
		_dict_it_t* dict_it;
#endif /* MB_ENABLE_COLLECTION_LIB */
		_label_t* label;
#ifdef MB_ENABLE_CLASS
		_class_t* instance;
#endif /* MB_ENABLE_CLASS */
		_routine_t* routine;
		char separator;
		mb_val_bytes_t bytes;
		_raw_t raw;
	} data;
	bool_t ref;
#ifdef MB_ENABLE_SOURCE_TRACE
	int source_pos;
	unsigned short source_row;
	unsigned short source_col;
#else /* MB_ENABLE_SOURCE_TRACE */
	char source_pos;
#endif /* MB_ENABLE_SOURCE_TRACE */
} _object_t;

#ifdef MB_ENABLE_MODULE
typedef struct _module_func_t {
	char* module;
	mb_func_t func;
} _module_func_t;
#endif /* MB_ENABLE_MODULE */

#define _MB_MEM_TAG_SIZE (sizeof(mb_mem_tag_t))

MBAPI const size_t MB_SIZEOF_4BYTES = 4;
MBAPI const size_t MB_SIZEOF_8BYTES = 8;
MBAPI const size_t MB_SIZEOF_32BYTES = 32;
MBAPI const size_t MB_SIZEOF_64BYTES = 64;
MBAPI const size_t MB_SIZEOF_128BYTES = 128;
MBAPI const size_t MB_SIZEOF_256BYTES = 256;
MBAPI const size_t MB_SIZEOF_512BYTES = 512;
#ifdef MB_ENABLE_ALLOC_STAT
MBAPI const size_t MB_SIZEOF_INT = _MB_MEM_TAG_SIZE + sizeof(int);
MBAPI const size_t MB_SIZEOF_PTR = _MB_MEM_TAG_SIZE + sizeof(intptr_t);
MBAPI const size_t MB_SIZEOF_LSN = _MB_MEM_TAG_SIZE + sizeof(_ls_node_t);
MBAPI const size_t MB_SIZEOF_HTN = _MB_MEM_TAG_SIZE + sizeof(_ht_node_t);
MBAPI const size_t MB_SIZEOF_HTA = _MB_MEM_TAG_SIZE + sizeof(_ht_node_t*) * _HT_ARRAY_SIZE_DEFAULT;
MBAPI const size_t MB_SIZEOF_OBJ = _MB_MEM_TAG_SIZE + sizeof(_object_t);
MBAPI const size_t MB_SIZEOF_FUN = _MB_MEM_TAG_SIZE + sizeof(_func_t);
MBAPI const size_t MB_SIZEOF_ARR = _MB_MEM_TAG_SIZE + sizeof(_array_t);
MBAPI const size_t MB_SIZEOF_VAR = _MB_MEM_TAG_SIZE + sizeof(_var_t);
MBAPI const size_t MB_SIZEOF_LBL = _MB_MEM_TAG_SIZE + sizeof(_label_t);
MBAPI const size_t MB_SIZEOF_RTN = _MB_MEM_TAG_SIZE + sizeof(_routine_t);
#ifdef MB_ENABLE_CLASS
MBAPI const size_t MB_SIZEOF_CLS = _MB_MEM_TAG_SIZE + sizeof(_class_t);
#endif /* MB_ENABLE_CLASS */
#else /* MB_ENABLE_ALLOC_STAT */
MBAPI const size_t MB_SIZEOF_INT = sizeof(int);
MBAPI const size_t MB_SIZEOF_PTR = sizeof(intptr_t);
MBAPI const size_t MB_SIZEOF_LSN = sizeof(_ls_node_t);
MBAPI const size_t MB_SIZEOF_HTN = sizeof(_ht_node_t);
MBAPI const size_t MB_SIZEOF_HTA = sizeof(_ht_node_t*) * _HT_ARRAY_SIZE_DEFAULT;
MBAPI const size_t MB_SIZEOF_OBJ = sizeof(_object_t);
MBAPI const size_t MB_SIZEOF_FUN = sizeof(_func_t);
MBAPI const size_t MB_SIZEOF_ARR = sizeof(_array_t);
MBAPI const size_t MB_SIZEOF_VAR = sizeof(_var_t);
MBAPI const size_t MB_SIZEOF_LBL = sizeof(_label_t);
MBAPI const size_t MB_SIZEOF_RTN = sizeof(_routine_t);
#ifdef MB_ENABLE_CLASS
MBAPI const size_t MB_SIZEOF_CLS = sizeof(_class_t);
#endif /* MB_ENABLE_CLASS */
#endif /* MB_ENABLE_ALLOC_STAT */

#ifdef MB_ENABLE_SOURCE_TRACE
static const _object_t _OBJ_INT_UNIT = { _DT_INT, (mb_data_e)1, false, 0, 0, 0 };
static const _object_t _OBJ_INT_ZERO = { _DT_INT, (mb_data_e)0, false, 0, 0, 0 };
#else /* MB_ENABLE_SOURCE_TRACE */
static const _object_t _OBJ_INT_UNIT = { _DT_INT, (mb_data_e)1, false, 0 };
static const _object_t _OBJ_INT_ZERO = { _DT_INT, (mb_data_e)0, false, 0 };
#endif /* MB_ENABLE_SOURCE_TRACE */
#define _MAKE_NIL(__o) do { memset((__o), 0, sizeof(_object_t)); (__o)->type = _DT_NIL; } while(0)

static _object_t* _OBJ_BOOL_TRUE = 0;
static _object_t* _OBJ_BOOL_FALSE = 0;

/* Parsing context */
#define _CLASS_STATE_NONE 0
#define _CLASS_STATE_PROC 1

typedef enum _parsing_state_e {
	_PS_NORMAL = 0,
	_PS_STRING,
	_PS_COMMENT
} _parsing_state_e;

typedef enum _symbol_state_e {
	_SS_IDENTIFIER = 0,
	_SS_OPERATOR
} _symbol_state_e;

typedef struct _parsing_context_t {
	_ls_node_t* imported;
	char current_char;
	char current_symbol[_SINGLE_SYMBOL_MAX_LENGTH + 1];
	int current_symbol_nonius;
	int current_symbol_contains_accessor;
	_object_t* last_symbol;
	_parsing_state_e parsing_state;
	_symbol_state_e symbol_state;
#ifdef MB_ENABLE_CLASS
	unsigned short class_state;
#endif /* MB_ENABLE_CLASS */
	unsigned short routine_state;
	unsigned short routine_params_state;
	int parsing_pos;
	unsigned short parsing_row;
	unsigned short parsing_col;
} _parsing_context_t;

/* Running context */
#define _SCOPE_META_ROOT (1 << 0)
#define _SCOPE_META_REF (1 << 1)

#define _INFINITY_CALC_DEPTH -1

typedef struct _running_context_t {
	struct _running_context_t* prev;
	unsigned meta;
	_ht_node_t* var_dict;
	struct _running_context_t* ref;
	_var_t* next_loop_var;
	mb_value_t intermediate_value;
	int calc_depth;
} _running_context_t;

/* Expression processing utilities */
typedef struct _tuple3_t {
	void* e1;
	void* e2;
	void* e3;
} _tuple3_t;

/* Interpreter tag */
#define _JMP_NIL 0x00
#define _JMP_INS 0x01
#define _JMP_STR 0x02

typedef struct mb_interpreter_t {
	_ht_node_t* local_func_dict;
	_ht_node_t* global_func_dict;
#ifdef MB_ENABLE_MODULE
	_ht_node_t* module_func_dict;
	char* with_module;
	_ls_node_t* using_modules;
#endif /* MB_ENABLE_MODULE */
	_ls_node_t* ast;
	_parsing_context_t* parsing_context;
	_running_context_t* running_context;
	unsigned char jump_set;
#ifdef MB_ENABLE_CLASS
	_class_t* last_instance;
#endif /* MB_ENABLE_CLASS */
	_routine_t* last_routine;
	_ls_node_t* sub_stack;
	_ls_node_t* suspent_point;
	int schedule_suspend_tag;
	_ls_node_t* temp_values;
	_ls_node_t* lazy_destroy_objects;
#ifdef MB_ENABLE_GC
	_gc_t gc;
#endif /* MB_ENABLE_GC */
	int_t no_eat_comma_mark;
	_ls_node_t* skip_to_eoi;
	_ls_node_t* in_neg_expr;
	bool_t handled_error;
	mb_error_e last_error;
	char* last_error_func;
	int last_error_pos;
	unsigned short last_error_row;
	unsigned short last_error_col;
#ifdef MB_ENABLE_STACK_TRACE
	_ls_node_t* stack_frames;
#endif /* MB_ENABLE_STACK_TRACE */
	mb_debug_stepped_handler_t debug_stepped_handler;
	mb_error_handler_t error_handler;
	mb_print_func_t printer;
	mb_input_func_t inputer;
	mb_import_handler_t import_handler;
	void* userdata;
} mb_interpreter_t;

/* Operations */
static const char _PRECEDE_TABLE[20][20] = { /* Operator priority table */
	/* +    -    *    /   MOD   ^    (    )    =    >    <    >=   <=   ==   <>  AND   OR  NOT  NEG  IS */
	{ '>', '>', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* + */
	{ '>', '>', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* - */
	{ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* * */
	{ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* / */
	{ '>', '>', '<', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* MOD */
	{ '>', '>', '>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* ^ */
	{ '<', '<', '<', '<', '<', '<', '<', '=', ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<' }, /* ( */
	{ '>', '>', '>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* ) */
	{ '<', '<', '<', '<', '<', '<', '<', ' ', '=', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<' }, /* = */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* > */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* < */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* >= */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* <= */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* == */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* <> */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '>' }, /* AND */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '>' }, /* OR */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>' }, /* NOT */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<' }, /* NEG */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '>' } /* IS */
};

static _object_t* _exp_assign = 0;

#define _set_real_with_hex(__r, __i) \
	do { \
		if(sizeof(__r) == sizeof(unsigned char)) { \
			unsigned char __b = __i; \
			memcpy(&__r, &__b, sizeof(__r)); \
		} else if(sizeof(__r) == sizeof(unsigned short)) { \
			unsigned short __b = __i; \
			memcpy(&__r, &__b, sizeof(__r)); \
		} else if(sizeof(__r) == sizeof(unsigned)) { \
			unsigned __b = __i; \
			memcpy(&__r, &__b, sizeof(__r)); \
		} else if(sizeof(__r) == sizeof(unsigned long)) { \
			unsigned long __b = __i; \
			memcpy(&__r, &__b, sizeof(__r)); \
		} else if(sizeof(__r) == sizeof(unsigned long long)) { \
			unsigned long long __b = __i; \
			memcpy(&__r, &__b, sizeof(__r)); \
		} else { \
			mb_assert(0 && "Invalid real number precision."); \
		} \
	} while(0)

#if MB_CONVERT_TO_INT_LEVEL == MB_CONVERT_TO_INT_LEVEL_NONE
#	define _convert_to_int_if_posible(__o) ((void)(__o))
#else /* MB_CONVERT_TO_INT_LEVEL == MB_CONVERT_TO_INT_LEVEL_NONE */
#	define _convert_to_int_if_posible(__o) \
		do { \
			if((__o)->type == _DT_REAL && (real_t)(int_t)(__o)->data.float_point == (__o)->data.float_point) { \
				(__o)->type = _DT_INT; \
				(__o)->data.integer = (int_t)(__o)->data.float_point; \
			} \
		} while(0)
#endif /* MB_CONVERT_TO_INT_LEVEL == MB_CONVERT_TO_INT_LEVEL_NONE */

#define _instruct_head(__tuple) \
	_object_t opndv1; \
	_object_t opndv2; \
	_tuple3_t* tpptr = (_tuple3_t*)(*__tuple); \
	_object_t* opnd1 = (_object_t*)(tpptr->e1); \
	_object_t* opnd2 = (_object_t*)(tpptr->e2); \
	_object_t* val = (_object_t*)(tpptr->e3);
#define _instruct_common(__tuple) \
	_instruct_head(__tuple) \
	opndv1.type = (opnd1->type == _DT_INT || (opnd1->type == _DT_VAR && opnd1->data.variable->data->type == _DT_INT)) ? \
		_DT_INT : _DT_REAL; \
	opndv1.data = opnd1->type == _DT_VAR ? opnd1->data.variable->data->data : opnd1->data; \
	opndv2.type = (opnd2->type == _DT_INT || (opnd2->type == _DT_VAR && opnd2->data.variable->data->type == _DT_INT)) ? \
		_DT_INT : _DT_REAL; \
	opndv2.data = opnd2->type == _DT_VAR ? opnd2->data.variable->data->data : opnd2->data;
#define _instruct_fun_num_num(__optr, __tuple) \
	do { \
		_instruct_common(__tuple) \
		if(opndv1.type == _DT_INT && opndv2.type == _DT_INT) { \
			val->type = _DT_REAL; \
			val->data.float_point = (real_t)__optr((real_t)opndv1.data.integer, (real_t)opndv2.data.integer); \
		} else { \
			val->type = _DT_REAL; \
			val->data.float_point = (real_t)__optr( \
				opndv1.type == _DT_INT ? opndv1.data.integer : opndv1.data.float_point, \
				opndv2.type == _DT_INT ? opndv2.data.integer : opndv2.data.float_point); \
		} \
		_convert_to_int_if_posible(val); \
	} while(0)
#define _instruct_bool_op_bool(__optr, __tuple) \
	do { \
		_instruct_common(__tuple) \
		if(opndv1.type == _DT_NIL) { opndv1.type = _DT_INT; opndv1.data.integer = 0; } \
		else if(opndv1.type != _DT_INT && opndv1.type != _DT_REAL) { opndv1.type = _DT_INT; opndv1.data.integer = 1; } \
		if(opndv2.type == _DT_NIL) { opndv2.type = _DT_INT; opndv2.data.integer = 0; } \
		else if(opndv2.type != _DT_INT && opndv2.type != _DT_REAL) { opndv2.type = _DT_INT; opndv2.data.integer = 1; } \
		if(opndv1.type == _DT_INT && opndv2.type == _DT_INT) { \
			if((real_t)(opndv1.data.integer __optr opndv2.data.integer) == (real_t)opndv1.data.integer __optr (real_t)opndv2.data.integer) { \
				val->type = _DT_INT; \
				val->data.integer = opndv1.data.integer __optr opndv2.data.integer; \
			} else { \
				val->type = _DT_REAL; \
				val->data.float_point = (real_t)((real_t)opndv1.data.integer __optr (real_t)opndv2.data.integer); \
			} \
		} else { \
			val->type = _DT_REAL; \
			val->data.float_point = (real_t) \
				((opndv1.type == _DT_INT ? opndv1.data.integer : opndv1.data.float_point) __optr \
				(opndv2.type == _DT_INT ? opndv2.data.integer : opndv2.data.float_point)); \
		} \
		_convert_to_int_if_posible(val); \
	} while(0)
#define _instruct_int_op_int(__optr, __tuple) \
	do { \
		_instruct_common(__tuple) \
		if(opndv1.type == _DT_INT && opndv2.type == _DT_INT) { \
			val->type = _DT_INT; \
			val->data.integer = opndv1.data.integer __optr opndv2.data.integer; \
		} else { \
			val->type = _DT_INT; \
			val->data.integer = \
				((opndv1.type == _DT_INT ? opndv1.data.integer : (int_t)(opndv1.data.float_point)) __optr \
				(opndv2.type == _DT_INT ? opndv2.data.integer : (int_t)(opndv2.data.float_point))); \
		} \
	} while(0)
#define _instruct_num_op_num(__optr, __tuple) \
	do { \
		_instruct_common(__tuple) \
		if(opndv1.type == _DT_INT && opndv2.type == _DT_INT) { \
			if((real_t)(opndv1.data.integer __optr opndv2.data.integer) == (real_t)opndv1.data.integer __optr (real_t)opndv2.data.integer) { \
				val->type = _DT_INT; \
				val->data.integer = opndv1.data.integer __optr opndv2.data.integer; \
			} else { \
				val->type = _DT_REAL; \
				val->data.float_point = (real_t)((real_t)opndv1.data.integer __optr (real_t)opndv2.data.integer); \
			} \
		} else { \
			val->type = _DT_REAL; \
			val->data.float_point = (real_t) \
				((opndv1.type == _DT_INT ? opndv1.data.integer : opndv1.data.float_point) __optr \
				(opndv2.type == _DT_INT ? opndv2.data.integer : opndv2.data.float_point)); \
		} \
		_convert_to_int_if_posible(val); \
	} while(0)
#define _instruct_obj_op_obj(__optr, __tuple) \
	do { \
		_instruct_head(__tuple); \
		opndv1.type = opnd1->type == _DT_VAR ? opnd1->data.variable->data->type : opnd1->type; \
		opndv1.data = opnd1->type == _DT_VAR ? opnd1->data.variable->data->data : opnd1->data; \
		opndv2.type = opnd2->type == _DT_VAR ? opnd2->data.variable->data->type : opnd2->type; \
		opndv2.data = opnd2->type == _DT_VAR ? opnd2->data.variable->data->data : opnd2->data; \
		val->type = _DT_INT; \
		if(opndv1.type == opndv2.type) { \
			val->data.integer = (int_t)(mb_memcmp(&opndv1.data, &opndv2.data, sizeof(_raw_t)) __optr 0); \
		} else { \
			val->data.integer = (int_t)(opndv1.type __optr opndv2.type); \
		} \
	} while(0)
#define _instruct_connect_strings(__tuple) \
	do { \
		char* _str1 = 0; \
		char* _str2 = 0; \
		_tuple3_t* tpptr = (_tuple3_t*)(*__tuple); \
		_object_t* opnd1 = (_object_t*)(tpptr->e1); \
		_object_t* opnd2 = (_object_t*)(tpptr->e2); \
		_object_t* val = (_object_t*)(tpptr->e3); \
		val->type = _DT_STRING; \
		if(val->data.string) { \
			safe_free(val->data.string); \
		} \
		_str1 = _extract_string(opnd1); \
		_str2 = _extract_string(opnd2); \
		val->data.string = (char*)mb_malloc(strlen(_str1) + strlen(_str2) + 1); \
		memset(val->data.string, 0, strlen(_str1) + strlen(_str2) + 1); \
		strcat(val->data.string, _str1); \
		strcat(val->data.string, _str2); \
	} while(0)
#define _instruct_compare_strings(__optr, __tuple) \
	do { \
		char* _str1 = 0; \
		char* _str2 = 0; \
		_tuple3_t* tpptr = (_tuple3_t*)(*__tuple); \
		_object_t* opnd1 = (_object_t*)(tpptr->e1); \
		_object_t* opnd2 = (_object_t*)(tpptr->e2); \
		_object_t* val = (_object_t*)(tpptr->e3); \
		val->type = _DT_INT; \
		_str1 = _extract_string(opnd1); \
		_str2 = _extract_string(opnd2); \
		val->data.integer = strcmp(_str1, _str2) __optr 0; \
	} while(0)
#define _proc_div_by_zero(__s, __tuple, __exit, __result, __kind) \
	do { \
		_instruct_common(__tuple) \
		if((opndv2.type == _DT_INT && opndv2.data.integer == 0) || (opndv2.type == _DT_REAL && opndv2.data.float_point == 0.0f)) { \
			if((opndv1.type == _DT_INT && opndv1.data.integer == 0) || (opndv1.type == _DT_REAL && opndv1.data.float_point == 0.0f)) { \
				val->type = _DT_REAL; \
				_set_real_with_hex(val->data.float_point, MB_FNAN); \
			} else { \
				val->type = _DT_REAL; \
				_set_real_with_hex(val->data.float_point, MB_FINF); \
			} \
			_handle_error_on_obj((__s), __kind, 0, ((__tuple) && *(__tuple)) ? ((_object_t*)(((_tuple3_t*)(*(__tuple)))->e1)) : 0, MB_FUNC_WARNING, __exit, __result); \
		} \
	} while(0)

#define _set_tuple3_result(__l, __r) \
	do { \
		_object_t* val = (_object_t*)(((_tuple3_t*)(*(__l)))->e3); \
		val->type = _DT_INT; \
		val->data.integer = __r; \
	} while(0)

#define _math_calculate_fun_real(__s, __l, __a, __f, __exit, __result) \
	switch((__a).type) { \
	case MB_DT_INT: \
		(__a).value.float_point = (real_t)__f((real_t)(__a).value.integer); \
		(__a).type = MB_DT_REAL; \
		break; \
	case MB_DT_REAL: \
		(__a).value.float_point = (real_t)__f((__a).value.float_point); \
		break; \
	default: \
		_handle_error_on_obj(__s, SE_RN_NUMBER_EXPECTED, 0, ((__l) && *(__l)) ? ((_object_t*)(((_tuple3_t*)(*(__l)))->e1)) : 0, MB_FUNC_WARNING, __exit, __result); \
		break; \
	} \
	mb_convert_to_int_if_posible(__a);

#define _using_jump_set_of_instructional(__s, __obj, __exit, __result) \
	do { \
		if((__s)->jump_set & (~_JMP_INS)) { \
			_handle_error_on_obj(__s, SE_RN_DONT_MIX_INSTRUCTIONAL_AND_STRUCTURED, 0, DON(__obj), MB_FUNC_ERR, __exit, __result); \
		} else { \
			(__s)->jump_set |= _JMP_INS; \
		} \
	} while(0)

#define _using_jump_set_of_structured(__s, __obj, __exit, __result) \
	do { \
		if((__s)->jump_set & (~_JMP_STR)) { \
			_handle_error_on_obj(__s, SE_RN_DONT_MIX_INSTRUCTIONAL_AND_STRUCTURED, 0, DON(__obj), MB_FUNC_ERR, __exit, __result); \
		} else { \
			(__s)->jump_set |= _JMP_STR; \
		} \
	} while(0)

/* ========================================================} */

/*
** {========================================================
** Private function declarations
*/

/** List operations */
static int _ls_cmp_data(void* node, void* info);
static int _ls_cmp_extra(void* node, void* info);
static int _ls_cmp_extra_object(void* node, void* info);
static int _ls_cmp_extra_string(void* node, void* info);

static _ls_node_t* _ls_create_node(void* data);
static _ls_node_t* _ls_create(void);
static _ls_node_t* _ls_find(_ls_node_t* list, void* data, _ls_compare cmp);
static _ls_node_t* _ls_back(_ls_node_t* node);
static _ls_node_t* _ls_pushback(_ls_node_t* list, void* data);
static void* _ls_popback(_ls_node_t* list);
static _ls_node_t* _ls_insert_at(_ls_node_t* list, int index, void* data);
static unsigned int _ls_remove(_ls_node_t* list, _ls_node_t* node, _ls_operation op);
static unsigned int _ls_try_remove(_ls_node_t* list, void* info, _ls_compare cmp, _ls_operation op);
static unsigned int _ls_foreach(_ls_node_t* list, _ls_operation op);
static void _ls_sort(_ls_node_t* list, _ls_compare cmp);
static bool_t _ls_empty(_ls_node_t* list);
static void _ls_clear(_ls_node_t* list);
static void _ls_destroy(_ls_node_t* list);
static int _ls_free_extra(void* data, void* extra);
#define _LS_FOREACH(L, O, P, E) \
	do { \
		_ls_node_t* __lst = L; \
		int __opresult = _OP_RESULT_NORMAL; \
		_ls_node_t* __tmp = 0; \
		mb_assert(L); \
		__lst = __lst->next; \
		while(__lst) { \
			if(P != 0) { \
				P(__lst->data, __lst->extra, E); \
			} \
			if(O != 0) { \
				__opresult = O(__lst->data, __lst->extra); \
			} \
			__tmp = __lst; \
			__lst = __lst->next; \
			if(_OP_RESULT_DEL_NODE == __opresult) { \
				__tmp->prev->next = __lst; \
				if(__lst) { \
					__lst->prev = __tmp->prev; \
				} \
				safe_free(__tmp); \
			} \
		} \
	} while(0)

/** Dictionary operations */
static unsigned int _ht_hash_object(void* ht, void* d);
static unsigned int _ht_hash_string(void* ht, void* d);
static unsigned int _ht_hash_int(void* ht, void* d);
#ifdef MB_ENABLE_GC
static unsigned int _ht_hash_ref(void* ht, void* d);
#endif /* MB_ENABLE_GC */

static int _ht_cmp_object(void* d1, void* d2);
static int _ht_cmp_string(void* d1, void* d2);
static int _ht_cmp_int(void* d1, void* d2);
#ifdef MB_ENABLE_GC
static int _ht_cmp_ref(void* d1, void* d2);
#endif /* MB_ENABLE_GC */
#ifdef MB_ENABLE_MODULE
static int _ht_cmp_module_func(void* d1, void* d2);
#endif /* MB_ENABLE_MODULE */

static _ht_node_t* _ht_create(unsigned int size, _ht_compare cmp, _ht_hash hs, _ls_operation freeextra);
static _ls_node_t* _ht_find(_ht_node_t* ht, void* key);
static unsigned int _ht_set_or_insert(_ht_node_t* ht, void* key, void* value);
static unsigned int _ht_remove(_ht_node_t* ht, void* key, _ls_compare cmp);
static unsigned int _ht_foreach(_ht_node_t* ht, _ht_operation op);
static unsigned int _ht_count(_ht_node_t* ht);
static void _ht_clear(_ht_node_t* ht);
static void _ht_destroy(_ht_node_t* ht);
static int _ht_remove_exist(void* data, void* extra, _ht_node_t* ht);
#define _HT_FOREACH(H, O, P, E) \
	do { \
		_ls_node_t* __bucket = 0; \
		unsigned int __ul = 0; \
		for(__ul = 0; __ul < (H)->array_size; ++__ul) { \
			__bucket = (H)->array[__ul]; \
			if(__bucket) \
				_LS_FOREACH(__bucket, O, P, E); \
		} \
	} while(0)

/** Memory manipulations */
#define _MB_CHECK_MEM_TAG_SIZE(y, s) do { mb_mem_tag_t _tmp = (mb_mem_tag_t)s; if((y)_tmp != s) { mb_assert(0 && "\"mb_mem_tag_t\" is too small."); printf("The type \"mb_mem_tag_t\" is not precise enough to hold the given data, please redefine it!"); } } while(0)
#define _MB_WRITE_MEM_TAG_SIZE(t, s) (*((mb_mem_tag_t*)((char*)(t) - _MB_MEM_TAG_SIZE)) = (mb_mem_tag_t)s)
#define _MB_READ_MEM_TAG_SIZE(t) (*((mb_mem_tag_t*)((char*)(t) - _MB_MEM_TAG_SIZE)))

#ifdef MB_ENABLE_ALLOC_STAT
static volatile size_t _mb_allocated = 0;
#else /* MB_ENABLE_ALLOC_STAT */
static const size_t _mb_allocated = (size_t)(~0);
#endif /* MB_ENABLE_ALLOC_STAT */

static mb_memory_allocate_func_t _mb_allocate_func = 0;
static mb_memory_free_func_t _mb_free_func = 0;

static void* mb_malloc(size_t s);
static void mb_free(void* p);

static int mb_memcmp(void* l, void* r, size_t s);
static size_t mb_memtest(void* p, size_t s);

static char* mb_strdup(const char* p, size_t s);
static char* mb_strupr(char* s);

#define safe_free(__p) do { if(__p) { mb_free(__p); __p = 0; } else { mb_assert(0 && "Memory already released."); } } while(0)

/** Unicode handling */
#ifdef MB_ENABLE_UNICODE
static int mb_uu_ischar(char* ch);
static int mb_uu_strlen(char* ch);
static int mb_uu_substr(char* ch, int begin, int count, char** o);
#endif /* MB_ENABLE_UNICODE */

/** Expression processing */
static bool_t _is_operator(mb_func_t op);
static bool_t _is_flow(mb_func_t op);
static bool_t _is_unary(mb_func_t op);
static char _get_priority(mb_func_t op1, mb_func_t op2);
static int _get_priority_index(mb_func_t op);
static _object_t* _operate_operand(mb_interpreter_t* s, _object_t* optr, _object_t* opnd1, _object_t* opnd2, int* status);
static bool_t _is_expression_terminal(mb_interpreter_t* s, _object_t* obj);
static int _calc_expression(mb_interpreter_t* s, _ls_node_t** l, _object_t** val);
static int _proc_args(mb_interpreter_t* s, _ls_node_t** l, _running_context_t* running, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg, bool_t proc_ref);
static int _eval_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg);
static int _eval_script_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg);
static int _eval_native_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg);
static int _has_routine_lex_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r);
static int _pop_routine_lex_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r, mb_value_t* val);
static int _has_routine_fun_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r);
static int _pop_routine_fun_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r, mb_value_t* val);
static bool_t _is_print_terminal(mb_interpreter_t* s, _object_t* obj);

/** Handlers */
#define _handle_error_now(__s, __err, __func, __result) \
	do { \
		_set_current_error((__s), (__err), (__func)); \
		if((__s)->error_handler) { \
			if((__s)->handled_error) break; \
			(__s)->handled_error = true; \
			((__s)->error_handler)((__s), (__s)->last_error, (char*)mb_get_error_desc((__s)->last_error), \
				(__s)->last_error_func, \
				(__s)->parsing_context ? (__s)->parsing_context->parsing_pos : (__s)->last_error_pos, \
				(__s)->parsing_context ? (__s)->parsing_context->parsing_row : (__s)->last_error_row, \
				(__s)->parsing_context ? (__s)->parsing_context->parsing_col : (__s)->last_error_col, \
				(__result)); \
		} \
	} while(0)
#if _WARING_AS_ERROR
#	define _handle_error(__s, __err, __func, __pos, __row, __col, __ret, __exit, __result) \
		do { \
			_set_current_error((__s), (__err), (__func)); \
			_set_error_pos((__s), (__pos), (__row), (__col)); \
			__result = (__ret); \
			goto __exit; \
		} while(0)
#else /* _WARING_AS_ERROR */
#	define _handle_error(__s, __err, __func, __pos, __row, __col, __ret, __exit, __result) \
		do { \
			_set_current_error((__s), (__err), (__func)); \
			_set_error_pos((__s), (__pos), (__row), (__col)); \
			if((__ret) != MB_FUNC_WARNING) { \
				__result = (__ret); \
			} \
			goto __exit; \
		} while(0)
#endif /* _WARING_AS_ERROR */
#ifdef MB_ENABLE_SOURCE_TRACE
#	define _HANDLE_ERROR(__s, __err, __func, __obj, __ret, __exit, __result) _handle_error((__s), (__err), (__func), (__obj)->source_pos, (__obj)->source_row, (__obj)->source_col, (__ret), __exit, (__result))
#else /* MB_ENABLE_SOURCE_TRACE */
#	define _HANDLE_ERROR(__s, __err, __func, __obj, __ret, __exit, __result) _handle_error((__s), (__err), (__func), 0, 0, 0, (__ret), __exit, (__result))
#endif /* MB_ENABLE_SOURCE_TRACE */
#define _handle_error_on_obj(__s, __err, __func, __obj, __ret, __exit, __result) \
	do { \
		if(__obj) { \
			_HANDLE_ERROR((__s), (__err), (__func), (__obj), (__ret), __exit, (__result)); \
		} else { \
			_handle_error((__s), (__err), (__func), 0, 0, 0, (__ret), __exit, (__result)); \
		} \
	} while(0)

#define _OUTTER_SCOPE(__s) ((__s)->prev ? (__s)->prev : (__s))

static void _set_current_error(mb_interpreter_t* s, mb_error_e err, char* func);

static mb_print_func_t _get_printer(mb_interpreter_t* s);
static mb_input_func_t _get_inputer(mb_interpreter_t* s);

/** Parsing helpers */
static char* _load_file(mb_interpreter_t* s, const char* f, const char* prefix);

static bool_t _is_blank(char c);
static bool_t _is_newline(char c);
static bool_t _is_separator(char c);
static bool_t _is_bracket(char c);
static bool_t _is_quotation_mark(char c);
static bool_t _is_comment(char c);
static bool_t _is_accessor(char c);
static bool_t _is_numeric_char(char c);
static bool_t _is_identifier_char(char c);
static bool_t _is_operator_char(char c);
static bool_t _is_using_char(char c);
static bool_t _is_exponent_prefix(char* s, int begin, int end);

static int _append_char_to_symbol(mb_interpreter_t* s, char c);
static int _cut_symbol(mb_interpreter_t* s, int pos, unsigned short row, unsigned short col);
static int _append_symbol(mb_interpreter_t* s, char* sym, bool_t* delsym, int pos, unsigned short row, unsigned short col);
static int _create_symbol(mb_interpreter_t* s, _ls_node_t* l, char* sym, _object_t** obj, _ls_node_t*** asgn, bool_t* delsym);
static _data_e _get_symbol_type(mb_interpreter_t* s, char* sym, _raw_t* value);
static int _parse_char(mb_interpreter_t* s, char c, int pos, unsigned short row, unsigned short col);
static void _set_error_pos(mb_interpreter_t* s, int pos, unsigned short row, unsigned short col);

/** Object processors */
static int_t _get_size_of(_data_e type);
static bool_t _try_get_value(_object_t* obj, mb_value_u* val, _data_e expected);

static bool_t _is_number(void* obj);
static bool_t _is_string(void* obj);
static char* _extract_string(_object_t* obj);

#define _REF_USERTYPE_REF(__o) \
	case _DT_USERTYPE_REF: \
		_ref(&(__o)->data.usertype_ref->ref, (__o)->data.usertype_ref); \
		break;
#define _UNREF_USERTYPE_REF(__o) \
	case _DT_USERTYPE_REF: \
		_unref(&(__o)->data.usertype_ref->ref, (__o)->data.usertype_ref); \
		break;
#define _ADDGC_USERTYPE_REF(__o, __g) \
	case _DT_USERTYPE_REF: \
		_gc_add(&(__o)->data.usertype_ref->ref, (__o)->data.usertype_ref, (__g)); \
		break;
#define _REF_ARRAY(__o) \
	case _DT_ARRAY: \
		if(!(__o)->ref) \
			_ref(&(__o)->data.array->ref, (__o)->data.array); \
		break;
#define _UNREF_ARRAY(__o) \
	case _DT_ARRAY: \
		if(!(__o)->ref) \
			_unref(&(__o)->data.array->ref, (__o)->data.array); \
		break;
#define _ADDGC_ARRAY(__o, __g) \
	case _DT_ARRAY: \
		if(!(__o)->ref) \
			_gc_add(&(__o)->data.array->ref, (__o)->data.array, (__g)); \
		break;
#ifdef MB_ENABLE_COLLECTION_LIB
#	define _REF_COLL(__o) \
		case _DT_LIST: \
			_ref(&(__o)->data.list->ref, (__o)->data.list); \
			break; \
		case _DT_DICT: \
			_ref(&(__o)->data.dict->ref, (__o)->data.dict); \
			break;
#	define _UNREF_COLL(__o) \
		case _DT_LIST: \
			_unref(&(__o)->data.list->ref, (__o)->data.list); \
			break; \
		case _DT_DICT: \
			_unref(&(__o)->data.dict->ref, (__o)->data.dict); \
			break;
#	define _ADDGC_COLL(__o, __g) \
		case _DT_LIST: \
			_gc_add(&(__o)->data.list->ref, (__o)->data.list, (__g)); \
			break; \
		case _DT_DICT: \
			_gc_add(&(__o)->data.dict->ref, (__o)->data.dict, (__g)); \
			break;
#	define _UNREF_COLL_IT(__o) \
		case _DT_LIST_IT: \
			_destroy_list_it(obj->data.list_it); \
			break; \
		case _DT_DICT_IT: \
			_destroy_dict_it(obj->data.dict_it); \
			break;
#else /* MB_ENABLE_COLLECTION_LIB */
#	define _REF_COLL(__o) ((void)(__o));
#	define _UNREF_COLL(__o) ((void)(__o));
#	define _ADDGC_COLL(__o, __g) ((void)(__o)); ((void)(__g));
#	define _UNREF_COLL_IT(__o) ((void)(__o));
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
#	define _REF_CLASS(__o) \
		case _DT_CLASS: \
			_ref(&(__o)->data.instance->ref, (__o)->data.instance); \
			break;
#	define _UNREF_CLASS(__o) \
		case _DT_CLASS: \
			if(!(__o)->ref) \
				_unref(&(__o)->data.instance->ref, (__o)->data.instance); \
			break;
#	define _ADDGC_CLASS(__o, __g) \
		case _DT_CLASS: \
			if(!(__o)->ref) \
				_gc_add(&(__o)->data.instance->ref, (__o)->data.instance, (__g)); \
			break;
#else /* MB_ENABLE_CLASS */
#	define _REF_CLASS(__o) ((void)(__o));
#	define _UNREF_CLASS(__o) ((void)(__o));
#	define _ADDGC_CLASS(__o, __g) ((void)(__o)); ((void)(__g));
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_LAMBDA
#	define _REF_ROUTINE(__o) \
		case _DT_ROUTINE: \
			_ref(&(__o)->data.routine->func.lambda.ref, (__o)->data.routine); \
			break;
#	define _UNREF_ROUTINE(__o) \
		case _DT_ROUTINE: \
			if(!(__o)->ref && (__o)->data.routine->type == _IT_LAMBDA) \
				_unref(&(__o)->data.routine->func.lambda.ref, (__o)->data.routine); \
			else if(!(__o)->ref && (__o)->data.routine->type != _IT_LAMBDA)\
				_destroy_routine((__o)->data.routine); \
			break;
#	define _ADDGC_ROUTINE(__o, __g) \
		case _DT_ROUTINE: \
			if(!(__o)->ref && (__o)->data.routine->type == _IT_LAMBDA) \
				_gc_add(&(__o)->data.routine->func.lambda.ref, (__o)->data.routine, (__g)); \
			else if(!(__o)->ref && (__o)->data.routine->type != _IT_LAMBDA)\
				_dispose_object(__o); \
			break;
#else /* MB_ENABLE_LAMBDA */
#	define _REF_ROUTINE(__o) ((void)(__o));
#	define _UNREF_ROUTINE(__o) ((void)(__o));
#	define _ADDGC_ROUTINE(__o, __g) \
		case _DT_ROUTINE: \
			((void)(__g)); \
			_dispose_object(__o); \
			break;
#endif /* MB_ENABLE_LAMBDA */
#define _ADDGC_STRING(__o) \
	case _DT_STRING: \
		_dispose_object(__o); \
		break;
#define _REF(__o) \
	switch((__o)->type) { \
	_REF_USERTYPE_REF(__o) \
	_REF_ARRAY(__o) \
	_REF_COLL(__o) \
	_REF_CLASS(__o) \
	_REF_ROUTINE(__o) \
	default: break; \
	}
#define _UNREF(__o) \
	switch((__o)->type) { \
	_UNREF_USERTYPE_REF(__o) \
	_UNREF_ARRAY(__o) \
	_UNREF_COLL(__o) \
	_UNREF_CLASS(__o) \
	_UNREF_ROUTINE(__o) \
	default: break; \
	}
#define _ADDGC(__o, __g) \
	switch((__o)->type) { \
	_ADDGC_USERTYPE_REF(__o, __g) \
	_ADDGC_ARRAY(__o, __g) \
	_ADDGC_COLL(__o, __g) \
	_ADDGC_CLASS(__o, __g) \
	_ADDGC_ROUTINE(__o, __g) \
	_ADDGC_STRING(__o) \
	default: break; \
	}

static bool_t _lock_ref_object(_lock_t* lk, _ref_t* ref, void* obj);
static bool_t _unlock_ref_object(_lock_t* lk, _ref_t* ref, void* obj);
static bool_t _write_on_ref_object(_lock_t* lk, _ref_t* ref, void* obj);

static unsigned _ref(_ref_t* ref, void* data);
static unsigned _unref(_ref_t* ref, void* data);
static void _create_ref(_ref_t* ref, _unref_func_t dtor, _data_e t, mb_interpreter_t* s);
static void _destroy_ref(_ref_t* ref);

#ifdef MB_ENABLE_GC
static void _gc_add(_ref_t* ref, void* data, _gc_t* gc);
static void _gc_remove(_ref_t* ref, void* data);
static int _gc_add_reachable(void* data, void* extra, void* ht);
static void _gc_get_reachable(mb_interpreter_t* s, _ht_node_t* ht);
static int _gc_destroy_garbage_in_list(void* data, void* extra, void* gc);
static int _gc_destroy_garbage_in_dict(void* data, void* extra, void* gc);
#ifdef MB_ENABLE_CLASS
static int _gc_destroy_garbage_in_class(void* data, void* extra, void* gc);
#endif /* MB_ENABLE_CLASS */
static int _gc_destroy_garbage(void* data, void* extra);
static void _gc_swap_tables(mb_interpreter_t* s);
static void _gc_try_trigger(mb_interpreter_t* s);
static void _gc_collect_garbage(mb_interpreter_t* s, int depth);
#endif /* MB_ENABLE_GC */

static _usertype_ref_t* _create_usertype_ref(mb_interpreter_t* s, void* val, mb_dtor_func_t un, mb_clone_func_t cl, mb_hash_func_t hs, mb_cmp_func_t cp, mb_fmt_func_t ft);
static void _destroy_usertype_ref(_usertype_ref_t* c);
static void _unref_usertype_ref(_ref_t* ref, void* data);

static _array_t* _create_array(const char* n, _data_e t, mb_interpreter_t* s);
static void _destroy_array(_array_t* arr);
static void _init_array(_array_t* arr);
static int _get_array_pos(struct mb_interpreter_t* s, _array_t* arr, int* d, int c);
static int _get_array_index(mb_interpreter_t* s, _ls_node_t** l, _object_t* c, unsigned int* index, bool_t* literally);
static bool_t _get_array_elem(mb_interpreter_t* s, _array_t* arr, unsigned int index, mb_value_u* val, _data_e* type);
static int _set_array_elem(mb_interpreter_t* s, _ls_node_t* ast, _array_t* arr, unsigned int index, mb_value_u* val, _data_e* type);
static void _clear_array(_array_t* arr);
static bool_t _is_array(void* obj);
static void _unref_array(_ref_t* ref, void* data);

#ifdef MB_ENABLE_COLLECTION_LIB
static _list_t* _create_list(mb_interpreter_t* s);
static void _destroy_list(_list_t* c);
static _dict_t* _create_dict(mb_interpreter_t* s);
static void _destroy_dict(_dict_t* c);
static _list_it_t* _create_list_it(_list_t* coll, bool_t lock);
static bool_t _destroy_list_it(_list_it_t* it);
static _list_it_t* _move_list_it_next(_list_it_t* it);
static _dict_it_t* _create_dict_it(_dict_t* coll, bool_t lock);
static bool_t _destroy_dict_it(_dict_it_t* it);
static _dict_it_t* _move_dict_it_next(_dict_it_t* it);
static void _unref_list(_ref_t* ref, void* data);
static void _unref_dict(_ref_t* ref, void* data);
static void _push_list(_list_t* coll, mb_value_t* val, _object_t* oarg);
static bool_t _pop_list(_list_t* coll, mb_value_t* val, mb_interpreter_t* s);
static bool_t _insert_list(_list_t* coll, int_t idx, mb_value_t* val, _object_t** oval);
static bool_t _set_list(_list_t* coll, int_t idx, mb_value_t* val, _object_t** oval);
static bool_t _remove_at_list(_list_t* coll, int_t idx);
static _ls_node_t* _node_at_list(_list_t* coll, int index);
static bool_t _at_list(_list_t* coll, int_t idx, mb_value_t* oval);
static bool_t _find_list(_list_t* coll, mb_value_t* val);
static void _clear_list(_list_t* coll);
static void _sort_list(_list_t* coll);
static void _invalidate_list_cache(_list_t* coll);
static void _set_dict(_dict_t* coll, mb_value_t* key, mb_value_t* val, _object_t* okey, _object_t* oval);
static bool_t _remove_dict(_dict_t* coll, mb_value_t* key);
static bool_t _find_dict(_dict_t* coll, mb_value_t* val, mb_value_t* oval);
static void _clear_dict(_dict_t* coll);
static bool_t _invalid_list_it(_list_it_t* it);
static bool_t _invalid_dict_it(_dict_it_t* it);
static bool_t _assign_with_it(_object_t* tgt, _object_t* src);
static int _clone_to_list(void* data, void* extra, _list_t* coll);
static int _clone_to_dict(void* data, void* extra, _dict_t* coll);
#endif /* MB_ENABLE_COLLECTION_LIB */

#ifdef MB_ENABLE_CLASS
typedef int (* _class_scope_walker)(void*, void*, void*);
typedef bool_t (* _class_meta_walker)(_class_t*, void*, void*);
static void _init_class(mb_interpreter_t* s, _class_t* instance, char* n);
static void _begin_class(mb_interpreter_t* s);
static bool_t _end_class(mb_interpreter_t* s);
static void _unref_class(_ref_t* ref, void* data);
static void _destroy_class(_class_t* c);
static bool_t _traverse_class(_class_t* c, _class_scope_walker scope_walker, _class_meta_walker meta_walker, unsigned meta_depth, bool_t meta_walk_on_self, void* extra_data, void* ret);
static bool_t _link_meta_class(mb_interpreter_t* s, _class_t* derived, _class_t* base);
static void _unlink_meta_class(mb_interpreter_t* s, _class_t* derived);
static int _unlink_meta_instance(void* data, void* extra, _class_t* derived);
static int _clone_clsss_field(void* data, void* extra, void* n);
static bool_t _clone_class_meta_link(_class_t* meta, void* n, void* ret);
static bool_t _is_class(_class_t* instance, void* m, void* ret);
static bool_t _add_class_meta_reachable(_class_t* meta, void* ht, void* ret);
#endif /* MB_ENABLE_CLASS */
static void _init_routine(mb_interpreter_t* s, _routine_t* routine, char* n, mb_routine_func_t f);
static void _begin_routine(mb_interpreter_t* s);
static bool_t _end_routine(mb_interpreter_t* s);
static void _begin_routine_parameter_list(mb_interpreter_t* s);
static void _end_routine_parameter_list(mb_interpreter_t* s);
static _object_t* _duplicate_parameter(void* data, void* extra, _running_context_t* running);
#ifdef MB_ENABLE_LAMBDA
static void _unref_routine(_ref_t* ref, void* data);
static void _destroy_routine(_routine_t* r);
#endif /* MB_ENABLE_LAMBDA */
#ifdef MB_ENABLE_CLASS
static _running_context_t* _reference_scope_by_class(mb_interpreter_t* s, _running_context_t* p, _class_t* c);
static _running_context_t* _push_scope_by_class(mb_interpreter_t* s, _running_context_t* p);
static _ls_node_t* _search_identifier_in_class(mb_interpreter_t* s, _class_t* instance, const char* n, _ht_node_t** ht);
static _ls_node_t* _search_identifier_accessor(mb_interpreter_t* s, _running_context_t* scope, const char* n, _ht_node_t** ht);
#endif /* MB_ENABLE_CLASS */
static _running_context_t* _reference_scope_by_routine(mb_interpreter_t* s, _running_context_t* p, _routine_t* r);
static _running_context_t* _push_weak_scope_by_routine(mb_interpreter_t* s, _running_context_t* p, _routine_t* r);
static _running_context_t* _push_scope_by_routine(mb_interpreter_t* s, _running_context_t* p);
static void _unreference_scope(mb_interpreter_t* s, _running_context_t* p);
static _running_context_t* _pop_weak_scope(mb_interpreter_t* s, _running_context_t* p);
static _running_context_t* _pop_scope(mb_interpreter_t* s);
static _running_context_t* _find_scope(mb_interpreter_t* s, _running_context_t* p);
static _running_context_t* _get_scope_to_add_routine(mb_interpreter_t* s);
static _ls_node_t* _search_identifier_in_scope_chain(mb_interpreter_t* s, _running_context_t* scope, const char* n, int pathing, _ht_node_t** ht);
static _array_t* _search_array_in_scope_chain(mb_interpreter_t* s, _array_t* i, _object_t** o);
static _var_t* _search_var_in_scope_chain(mb_interpreter_t* s, _var_t* i);

static _var_t* _create_var(_object_t** oobj, const char* n, bool_t dup_name);
static _object_t* _create_object(void);
static int _clone_object(mb_interpreter_t* s, _object_t* obj, _object_t* tgt);
static int _dispose_object(_object_t* obj);
static int _destroy_object(void* data, void* extra);
static int _destroy_object_with_extra(void* data, void* extra);
static int _destroy_object_not_compile_time(void* data, void* extra);
static int _destroy_object_capsule_only(void* data, void* extra);
static int _destroy_object_capsule_only_with_extra(void* data, void* extra);
static int _do_nothing_on_object(void* data, void* extra);
static int _remove_source_object(void* data, void* extra);
static int _destroy_memory(void* data, void* extra);
static int _compare_numbers(const _object_t* first, const _object_t* second);
static bool_t _is_internal_object(_object_t* obj);
static _data_e _public_type_to_internal_type(mb_data_e t);
static mb_data_e _internal_type_to_public_type(_data_e t);
static int _public_value_to_internal_object(mb_value_t* pbl, _object_t* itn);
static int _internal_object_to_public_value(_object_t* itn, mb_value_t* pbl);
static int _create_internal_object_from_public_value(mb_value_t* pbl, _object_t** itn);
static int _compare_public_value_and_internal_object(mb_value_t* pbl, _object_t* itn);
static void _try_clear_intermediate_value(void* data, void* extra, mb_interpreter_t* s);
static void _mark_lazy_destroy_string(mb_interpreter_t* s, char* ch);
static void _assign_public_value(mb_value_t* tgt, mb_value_t* src);
static void _swap_public_value(mb_value_t* tgt, mb_value_t* src);
static int _clear_scope_chain(mb_interpreter_t* s);
static int _dispose_scope_chain(mb_interpreter_t* s);
static void _tidy_scope_chain(mb_interpreter_t* s);
static void _tidy_intermediate_value(_ref_t* ref, void* data);

static void _stepped(mb_interpreter_t* s, _ls_node_t* ast);
static int _execute_statement(mb_interpreter_t* s, _ls_node_t** l);
static int _skip_to(mb_interpreter_t* s, _ls_node_t** l, mb_func_t f, _data_e t);
static int _skip_if_chunk(mb_interpreter_t* s, _ls_node_t** l);
static int _skip_struct(mb_interpreter_t* s, _ls_node_t** l, mb_func_t open_func, mb_func_t close_func);

static _running_context_t* _create_running_context(void);
static _parsing_context_t* _reset_parsing_context(_parsing_context_t* context);
static void _destroy_parsing_context(_parsing_context_t** context);

#ifdef MB_ENABLE_MODULE
static _module_func_t* _create_module_func(mb_interpreter_t* s, mb_func_t f);
static int _ls_destroy_module_func(void* data, void* extra);
static int _ht_destroy_module_func_list(void* data, void* extra);
#endif /* MB_ENABLE_MODULE */
static char* _generate_func_name(mb_interpreter_t* s, char* n, bool_t with_mod);
static int _register_func(mb_interpreter_t* s, char* n, mb_func_t f, bool_t local);
static int _remove_func(mb_interpreter_t* s, char* n, bool_t local);
static _ls_node_t* _find_func(mb_interpreter_t* s, char* n, bool_t* mod);

static int _open_constant(mb_interpreter_t* s);
static int _close_constant(mb_interpreter_t* s);
static int _open_core_lib(mb_interpreter_t* s);
static int _close_core_lib(mb_interpreter_t* s);
static int _open_std_lib(mb_interpreter_t* s);
static int _close_std_lib(mb_interpreter_t* s);
#ifdef MB_ENABLE_COLLECTION_LIB
static int _open_coll_lib(mb_interpreter_t* s);
static int _close_coll_lib(mb_interpreter_t* s);
#endif /* MB_ENABLE_COLLECTION_LIB */

/* ========================================================} */

/*
** {========================================================
** Lib declarations
*/

/** Macro */
#ifdef _MSC_VER
#	if _MSC_VER < 1300
#		define MB_FUNC 0
#	else /* _MSC_VER < 1300 */
#		define MB_FUNC __FUNCTION__
#	endif /* _MSC_VER < 1300 */
#elif defined __BORLANDC__
#	define MB_FUNC __FUNC__
#elif defined __POCC__
#	define MB_FUNC __func__
#else /* _MSC_VER */
#	define MB_FUNC __FUNCTION__
#endif /* _MSC_VER */

#ifdef _MSC_VER
#	if _MSC_VER < 1300
#		define _do_nothing(__s, __l, __exit, __result) \
			do { \
				_ls_node_t* ast = 0; static int i = 0; ++i; \
				printf("Unaccessable function called %d times\n", i); \
				ast = (_ls_node_t*)(*(__l)); \
				_handle_error_on_obj((__s), SE_RN_WRONG_FUNCTION_REACHED, 0, DON(ast), MB_FUNC_ERR, __exit, __result); \
			} while(0)
#	endif /* _MSC_VER < 1300 */
#endif /* _MSC_VER */
#ifndef _do_nothing
#	define _do_nothing(__s, __l, __exit, __result) \
		do { \
			_ls_node_t* ast = (_ls_node_t*)(*(__l)); \
			_handle_error_on_obj((__s), SE_RN_WRONG_FUNCTION_REACHED, (char*)MB_FUNC, DON(ast), MB_FUNC_ERR, __exit, __result); \
		} while(0);
#endif /* _do_nothing */

/** Core lib */
static int _core_dummy_assign(mb_interpreter_t* s, void** l);
static int _core_add(mb_interpreter_t* s, void** l);
static int _core_min(mb_interpreter_t* s, void** l);
static int _core_mul(mb_interpreter_t* s, void** l);
static int _core_div(mb_interpreter_t* s, void** l);
static int _core_mod(mb_interpreter_t* s, void** l);
static int _core_pow(mb_interpreter_t* s, void** l);
static int _core_open_bracket(mb_interpreter_t* s, void** l);
static int _core_close_bracket(mb_interpreter_t* s, void** l);
static int _core_neg(mb_interpreter_t* s, void** l);
static int _core_equal(mb_interpreter_t* s, void** l);
static int _core_less(mb_interpreter_t* s, void** l);
static int _core_greater(mb_interpreter_t* s, void** l);
static int _core_less_equal(mb_interpreter_t* s, void** l);
static int _core_greater_equal(mb_interpreter_t* s, void** l);
static int _core_not_equal(mb_interpreter_t* s, void** l);
static int _core_and(mb_interpreter_t* s, void** l);
static int _core_or(mb_interpreter_t* s, void** l);
static int _core_not(mb_interpreter_t* s, void** l);
static int _core_is(mb_interpreter_t* s, void** l);
static int _core_let(mb_interpreter_t* s, void** l);
static int _core_dim(mb_interpreter_t* s, void** l);
static int _core_if(mb_interpreter_t* s, void** l);
static int _core_then(mb_interpreter_t* s, void** l);
static int _core_elseif(mb_interpreter_t* s, void** l);
static int _core_else(mb_interpreter_t* s, void** l);
static int _core_endif(mb_interpreter_t* s, void** l);
static int _core_for(mb_interpreter_t* s, void** l);
static int _core_to(mb_interpreter_t* s, void** l);
static int _core_step(mb_interpreter_t* s, void** l);
static int _core_next(mb_interpreter_t* s, void** l);
static int _core_while(mb_interpreter_t* s, void** l);
static int _core_wend(mb_interpreter_t* s, void** l);
static int _core_do(mb_interpreter_t* s, void** l);
static int _core_until(mb_interpreter_t* s, void** l);
static int _core_exit(mb_interpreter_t* s, void** l);
static int _core_goto(mb_interpreter_t* s, void** l);
static int _core_gosub(mb_interpreter_t* s, void** l);
static int _core_return(mb_interpreter_t* s, void** l);
static int _core_call(mb_interpreter_t* s, void** l);
static int _core_def(mb_interpreter_t* s, void** l);
static int _core_enddef(mb_interpreter_t* s, void** l);
#ifdef MB_ENABLE_CLASS
static int _core_class(mb_interpreter_t* s, void** l);
static int _core_endclass(mb_interpreter_t* s, void** l);
static int _core_new(mb_interpreter_t* s, void** l);
static int _core_var(mb_interpreter_t* s, void** l);
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_LAMBDA
static int _core_lambda(mb_interpreter_t* s, void** l);
#endif /* MB_ENABLE_LAMBDA */
#ifdef MB_ENABLE_ALLOC_STAT
static int _core_mem(mb_interpreter_t* s, void** l);
#endif /* MB_ENABLE_ALLOC_STAT */
static int _core_type(mb_interpreter_t* s, void** l);
static int _core_import(mb_interpreter_t* s, void** l);
static int _core_end(mb_interpreter_t* s, void** l);

/** Standard lib */
static int _std_abs(mb_interpreter_t* s, void** l);
static int _std_sgn(mb_interpreter_t* s, void** l);
static int _std_sqr(mb_interpreter_t* s, void** l);
static int _std_floor(mb_interpreter_t* s, void** l);
static int _std_ceil(mb_interpreter_t* s, void** l);
static int _std_fix(mb_interpreter_t* s, void** l);
static int _std_round(mb_interpreter_t* s, void** l);
static int _std_srnd(mb_interpreter_t* s, void** l);
static int _std_rnd(mb_interpreter_t* s, void** l);
static int _std_sin(mb_interpreter_t* s, void** l);
static int _std_cos(mb_interpreter_t* s, void** l);
static int _std_tan(mb_interpreter_t* s, void** l);
static int _std_asin(mb_interpreter_t* s, void** l);
static int _std_acos(mb_interpreter_t* s, void** l);
static int _std_atan(mb_interpreter_t* s, void** l);
static int _std_exp(mb_interpreter_t* s, void** l);
static int _std_log(mb_interpreter_t* s, void** l);
static int _std_asc(mb_interpreter_t* s, void** l);
static int _std_chr(mb_interpreter_t* s, void** l);
static int _std_left(mb_interpreter_t* s, void** l);
static int _std_mid(mb_interpreter_t* s, void** l);
static int _std_right(mb_interpreter_t* s, void** l);
static int _std_str(mb_interpreter_t* s, void** l);
static int _std_val(mb_interpreter_t* s, void** l);
static int _std_len(mb_interpreter_t* s, void** l);
static int _std_print(mb_interpreter_t* s, void** l);
static int _std_input(mb_interpreter_t* s, void** l);

/** Collection lib */
#ifdef MB_ENABLE_COLLECTION_LIB
static int _coll_list(mb_interpreter_t* s, void** l);
static int _coll_dict(mb_interpreter_t* s, void** l);
static int _coll_push(mb_interpreter_t* s, void** l);
static int _coll_pop(mb_interpreter_t* s, void** l);
static int _coll_peek(mb_interpreter_t* s, void** l);
static int _coll_insert(mb_interpreter_t* s, void** l);
static int _coll_sort(mb_interpreter_t* s, void** l);
static int _coll_exist(mb_interpreter_t* s, void** l);
static int _coll_get(mb_interpreter_t* s, void** l);
static int _coll_set(mb_interpreter_t* s, void** l);
static int _coll_remove(mb_interpreter_t* s, void** l);
static int _coll_clear(mb_interpreter_t* s, void** l);
static int _coll_clone(mb_interpreter_t* s, void** l);
static int _coll_iterator(mb_interpreter_t* s, void** l);
static int _coll_move_next(mb_interpreter_t* s, void** l);
#endif /* MB_ENABLE_COLLECTION_LIB */

/** Lib information */
static const _func_t _core_libs[] = {
	{ "#", _core_dummy_assign },
	{ "+", _core_add },
	{ "-", _core_min },
	{ "*", _core_mul },
	{ "/", _core_div },
	{ "MOD", _core_mod },
	{ "^", _core_pow },
	{ "(", _core_open_bracket },
	{ ")", _core_close_bracket },
	{ 0, _core_neg },

	{ "=", _core_equal },
	{ "<", _core_less },
	{ ">", _core_greater },
	{ "<=", _core_less_equal },
	{ ">=", _core_greater_equal },
	{ "<>", _core_not_equal },

	{ "AND", _core_and },
	{ "OR", _core_or },
	{ "NOT", _core_not },

	{ "IS", _core_is },

	{ "LET", _core_let },
	{ "DIM", _core_dim },

	{ "IF", _core_if },
	{ "THEN", _core_then },
	{ "ELSEIF", _core_elseif },
	{ "ELSE", _core_else },
	{ "ENDIF", _core_endif },

	{ "FOR", _core_for },
	{ "TO", _core_to },
	{ "STEP", _core_step },
	{ "NEXT", _core_next },
	{ "WHILE", _core_while },
	{ "WEND", _core_wend },
	{ "DO", _core_do },
	{ "UNTIL", _core_until },

	{ "EXIT", _core_exit },
	{ "GOTO", _core_goto },
	{ "GOSUB", _core_gosub },
	{ "RETURN", _core_return },

	{ "CALL", _core_call },
	{ "DEF", _core_def },
	{ "ENDDEF", _core_enddef },
#ifdef MB_ENABLE_CLASS
	{ "CLASS", _core_class },
	{ "ENDCLASS", _core_endclass },
	{ "NEW", _core_new },
	{ "VAR", _core_var },
#endif /* MB_ENABLE_CLASS */

#ifdef MB_ENABLE_LAMBDA
	{ "LAMBDA", _core_lambda },
#endif /* MB_ENABLE_LAMBDA */

#ifdef MB_ENABLE_ALLOC_STAT
	{ "MEM", _core_mem },
#endif /* MB_ENABLE_ALLOC_STAT */

	{ "TYPE", _core_type },
	{ "IMPORT", _core_import },
	{ "END", _core_end }
};

static const _func_t _std_libs[] = {
	{ "ABS", _std_abs },
	{ "SGN", _std_sgn },
	{ "SQR", _std_sqr },
	{ "FLOOR", _std_floor },
	{ "CEIL", _std_ceil },
	{ "FIX", _std_fix },
	{ "ROUND", _std_round },
	{ "SRND", _std_srnd },
	{ "RND", _std_rnd },
	{ "SIN", _std_sin },
	{ "COS", _std_cos },
	{ "TAN", _std_tan },
	{ "ASIN", _std_asin },
	{ "ACOS", _std_acos },
	{ "ATAN", _std_atan },
	{ "EXP", _std_exp },
	{ "LOG", _std_log },

	{ "ASC", _std_asc },
	{ "CHR", _std_chr },
	{ "LEFT", _std_left },
	{ "MID", _std_mid },
	{ "RIGHT", _std_right },
	{ "STR", _std_str },
	{ "VAL", _std_val },

	{ "LEN", _std_len },

	{ "PRINT", _std_print },
	{ "INPUT", _std_input }
};

#ifdef MB_ENABLE_COLLECTION_LIB
static const _func_t _coll_libs[] = {
	{ "LIST", _coll_list },
	{ "DICT", _coll_dict },
	{ "PUSH", _coll_push },
	{ "POP", _coll_pop },
	{ "PEEK", _coll_peek },
	{ "INSERT", _coll_insert },
	{ "SORT", _coll_sort },
	{ "EXIST", _coll_exist },
	{ "GET", _coll_get },
	{ "SET", _coll_set },
	{ "REMOVE", _coll_remove },
	{ "CLEAR", _coll_clear },
	{ "CLONE", _coll_clone },
	{ "ITERATOR", _coll_iterator },
	{ "MOVE_NEXT", _coll_move_next }
};
#endif /* MB_ENABLE_COLLECTION_LIB */

/* ========================================================} */

/*
** {========================================================
** Private function definitions
*/

/** List operations */
int _ls_cmp_data(void* node, void* info) {
	_ls_node_t* n = (_ls_node_t*)node;

	return (n->data == info) ? 0 : 1;
}

int _ls_cmp_extra(void* node, void* info) {
	_ls_node_t* n = (_ls_node_t*)node;

	return (n->extra == info) ? 0 : 1;
}

int _ls_cmp_extra_object(void* node, void* info) {
	_ls_node_t* n = (_ls_node_t*)node;

	return _ht_cmp_object(n->extra, info);
}

int _ls_cmp_extra_string(void* node, void* info) {
	_ls_node_t* n = (_ls_node_t*)node;
	char* s1 = (char*)n->extra;
	char* s2 = (char*)info;

	return strcmp(s1, s2);
}

_ls_node_t* _ls_create_node(void* data) {
	_ls_node_t* result = 0;

	result = (_ls_node_t*)mb_malloc(sizeof(_ls_node_t));
	mb_assert(result);
	memset(result, 0, sizeof(_ls_node_t));
	result->data = data;

	return result;
}

_ls_node_t* _ls_create(void) {
	_ls_node_t* result = 0;

	result = _ls_create_node(0);

	return result;
}

_ls_node_t* _ls_find(_ls_node_t* list, void* data, _ls_compare cmp) {
	_ls_node_t* result = 0;

	mb_assert(list && data && cmp);

	list = list->next;
	while(list) {
		if(!cmp(list->data, data)) {
			result = list;

			break;
		}
		list = list->next;
	}

	return result;
}

_ls_node_t* _ls_back(_ls_node_t* node) {
	_ls_node_t* result = node;

	result = result->prev;

	return result;
}

_ls_node_t* _ls_pushback(_ls_node_t* list, void* data) {
	_ls_node_t* result = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list);

	result = _ls_create_node(data);

	tmp = _ls_back(list);
	if(!tmp)
		tmp = list;
	tmp->next = result;
	result->prev = tmp;
	list->prev = result;

	return result;
}

void* _ls_popback(_ls_node_t* list) {
	void* result = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list);

	tmp = _ls_back(list);
	if(tmp) {
		result = tmp->data;
		if(list != tmp->prev)
			list->prev = tmp->prev;
		else
			list->prev = 0;
		tmp->prev->next = 0;
		safe_free(tmp);
	}

	return result;
}

_ls_node_t* _ls_insert_at(_ls_node_t* list, int index, void* data) {
	_ls_node_t* result = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list);

	tmp = list->next;
	while(tmp && index) {
		tmp = tmp->next;
		--index;
	}
	if(!tmp) {
		if(index == 0)
			result = _ls_pushback(list, data);
	} else {
		result = _ls_create_node(data);
		tmp->prev->next = result;
		result->prev = tmp->prev;
		result->next = tmp;
		tmp->prev = result;
	}

	return result;
}

unsigned int _ls_remove(_ls_node_t* list, _ls_node_t* node, _ls_operation op) {
	unsigned int result = 0;

	mb_assert(list && node);

	if(node->prev)
		node->prev->next = node->next;
	if(node->next)
		node->next->prev = node->prev;
	if(list->prev == node)
		list->prev = node->prev;
	if(list->prev == list)
		list->prev = 0;
	if(op)
		op(node->data, node->extra);
	safe_free(node);
	++result;

	return result;
}

unsigned int _ls_try_remove(_ls_node_t* list, void* info, _ls_compare cmp, _ls_operation op) {
	unsigned int result = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list && cmp);

	tmp = list->next;
	while(tmp) {
		if(cmp(tmp, info) == 0) {
			if(tmp->prev)
				tmp->prev->next = tmp->next;
			if(tmp->next)
				tmp->next->prev = tmp->prev;
			if(list->prev == tmp)
				list->prev = tmp->prev;
			if(list->prev == list)
				list->prev = 0;
			if(op)
				op(tmp->data, tmp->extra);
			safe_free(tmp);
			++result;

			break;
		}
		tmp = tmp->next;
	}

	return result;
}

unsigned int _ls_foreach(_ls_node_t* list, _ls_operation op) {
	unsigned int idx = 0;
	int opresult = _OP_RESULT_NORMAL;
	_ls_node_t* tmp = 0;

	mb_assert(list && op);

	list = list->next;
	while(list) {
		opresult = op(list->data, list->extra);
		++idx;
		tmp = list;
		list = list->next;

		if(_OP_RESULT_NORMAL == opresult) {
			/* Do nothing */
		} else if(_OP_RESULT_DEL_NODE == opresult) {
			tmp->prev->next = list;
			if(list)
				list->prev = tmp->prev;
			safe_free(tmp);
		} else {
			/* Do nothing */
		}
	}

	return idx;
}

void _ls_sort(_ls_node_t* list, _ls_compare cmp) {
	_ls_node_t* ptr = 0;
	void* tmp = 0;

	mb_assert(list && cmp);

	list = list->next;
	for( ; list; list = list->next) {
		for(ptr = list; ptr; ptr = ptr->next) {
			if(cmp(list->data, ptr->data) > 0) {
				tmp = ptr->data;
				ptr->data = list->data;
				list->data = tmp;
			}
		}
	}
}

bool_t _ls_empty(_ls_node_t* list) {
	bool_t result = false;

	mb_assert(list);

	result = 0 == list->next;

	return result;
}

void _ls_clear(_ls_node_t* list) {
	_ls_node_t* tmp = 0;

	mb_assert(list);

	tmp = list;
	list = list->next;
	tmp->next = 0;
	tmp->prev = 0;

	while(list) {
		tmp = list;
		list = list->next;
		safe_free(tmp);
	}
}

void _ls_destroy(_ls_node_t* list) {
	_ls_clear(list);
	safe_free(list);
}

int _ls_free_extra(void* data, void* extra) {
	int result = _OP_RESULT_NORMAL;
	mb_unrefvar(data);

	mb_assert(extra);

	safe_free(extra);

	result = _OP_RESULT_DEL_NODE;

	return result;
}

/** Dictionary operations */
unsigned int _ht_hash_object(void* ht, void* d) {
	unsigned int result = 0;
	_ht_node_t* self = (_ht_node_t*)ht;
	_object_t* o = (_object_t*)d;
	size_t i = 0;
	unsigned int h = 0;

	mb_assert(ht);

	h = o->type;
	switch(o->type) {
	case _DT_STRING:
		result = 5 * h + _ht_hash_string(ht, o->data.string);

		break;
	case _DT_USERTYPE_REF:
		if(o->data.usertype_ref->hash) {
			h = 5 * h + o->data.usertype_ref->hash(o->data.usertype_ref->ref.s, o->data.usertype_ref->usertype);

			break;
		}
		/* Fall through */
	default:
		for(i = 0; i < sizeof(_raw_t); ++i)
			h = 5 * h + o->data.raw[i];
		result = h % self->array_size;

		break;
	}

	return result;
}

unsigned int _ht_hash_string(void* ht, void* d) {
	unsigned int result = 0;
	_ht_node_t* self = (_ht_node_t*)ht;
	char* s = (char*)d;
	unsigned int h = 0;

	mb_assert(ht);

	for( ; *s; ++s)
		h = 5 * h + *s;
	result = h % self->array_size;

	return result;
}

unsigned int _ht_hash_int(void* ht, void* d) {
	unsigned int result = 0;
	_ht_node_t* self = (_ht_node_t*)ht;
	int_t i = *(int_t*)d;

	mb_assert(ht);

	result = (unsigned int)i;
	result %= self->array_size;

	return result;
}

#ifdef MB_ENABLE_GC
unsigned int _ht_hash_ref(void* ht, void* d) {
	unsigned int result = 0;
	_ht_node_t* self = (_ht_node_t*)ht;
	_ref_t* ref = *(_ref_t**)d;

	mb_assert(ht);

	result = (unsigned int)(intptr_t)ref;
	result %= self->array_size;

	return result;
}
#endif /* MB_ENABLE_GC */

int _ht_cmp_object(void* d1, void* d2) {
	_object_t* o1 = (_object_t*)d1;
	_object_t* o2 = (_object_t*)d2;
	size_t i = 0;

	if(o1->type < o2->type)
		return -1;
	else if(o1->type > o2->type)
		return 1;

	switch(o1->type) {
	case _DT_STRING:
		return _ht_cmp_string(o1->data.string, o2->data.string);
	case _DT_USERTYPE_REF:
		if(o1->data.usertype_ref->cmp) {
			o1->data.usertype_ref->cmp(o1->data.usertype_ref->ref.s, o1->data.usertype_ref->usertype, o2->data.usertype_ref->usertype);

			break;
		} else if(o2->data.usertype_ref->cmp) {
			o2->data.usertype_ref->cmp(o1->data.usertype_ref->ref.s, o1->data.usertype_ref->usertype, o2->data.usertype_ref->usertype);

			break;
		}
		/* Fall through */
	default:
		for(i = 0; i < sizeof(_raw_t); ++i) {
			if(o1->data.raw[i] < o2->data.raw[i])
				return -1;
			else if(o1->data.raw[i] > o2->data.raw[i])
				return 1;
		}
		break;
	}

	return 0;
}

int _ht_cmp_string(void* d1, void* d2) {
	char* s1 = (char*)d1;
	char* s2 = (char*)d2;

	return strcmp(s1, s2);
}

int _ht_cmp_int(void* d1, void* d2) {
	int_t i1 = *(int_t*)d1;
	int_t i2 = *(int_t*)d2;
	int_t i = i1 - i2;
	int result = 0;

	if(i < 0)
		result = -1;
	else if(i > 0)
		result = 1;

	return result;
}

#ifdef MB_ENABLE_GC
int _ht_cmp_ref(void* d1, void* d2) {
	_ref_t* r1 = *(_ref_t**)d1;
	_ref_t* r2 = *(_ref_t**)d2;
	intptr_t i = (intptr_t)r1 - (intptr_t)r2;
	int result = 0;

	if(i < 0)
		result = -1;
	else if(i > 0)
		result = 1;

	return result;
}
#endif /* MB_ENABLE_GC */

#ifdef MB_ENABLE_MODULE
int _ht_cmp_module_func(void* d1, void* d2) {
	_module_func_t* m = (_module_func_t*)d1;
	mb_interpreter_t* s = (mb_interpreter_t*)d2;

	return strcmp(m->module, s->with_module);
}
#endif /* MB_ENABLE_MODULE */

_ht_node_t* _ht_create(unsigned int size, _ht_compare cmp, _ht_hash hs, _ls_operation freeextra) {
	const unsigned int array_size = size ? size : _HT_ARRAY_SIZE_DEFAULT;
	_ht_node_t* result = 0;
	unsigned int ul = 0;

	if(!cmp)
		cmp = _ht_cmp_int;
	if(!hs)
		hs = _ht_hash_int;

	result = (_ht_node_t*)mb_malloc(sizeof(_ht_node_t));
	result->free_extra = freeextra;
	result->compare = cmp;
	result->hash = hs;
	result->array_size = array_size;
	result->count = 0;
	result->array = (_ls_node_t**)mb_malloc(sizeof(_ls_node_t*) * result->array_size);
	for(ul = 0; ul < result->array_size; ++ul)
		result->array[ul] = _ls_create();

	return result;
}

_ls_node_t* _ht_find(_ht_node_t* ht, void* key) {
	_ls_node_t* result = 0;
	_ls_node_t* bucket = 0;
	unsigned int hash_code = 0;

	mb_assert(ht && key);

	hash_code = ht->hash(ht, key);
	bucket = ht->array[hash_code];
	bucket = bucket->next;
	while(bucket) {
		if(ht->compare(bucket->extra, key) == 0) {
			result = bucket;

			break;
		}
		bucket = bucket->next;
	}

	return result;
}

unsigned int _ht_set_or_insert(_ht_node_t* ht, void* key, void* value) {
	unsigned int result = 0;
	_ls_node_t* bucket = 0;
	unsigned int hash_code = 0;

	mb_assert(ht && key);

	bucket = _ht_find(ht, key);
	if(bucket) { /* Update */
		bucket->data = value;
		++result;
	} else { /* Insert */
		hash_code = ht->hash(ht, key);
		bucket = ht->array[hash_code];
		bucket = _ls_pushback(bucket, value);
		mb_assert(bucket);
		bucket->extra = key;
		++ht->count;
		++result;
	}

	return result;
}

unsigned int _ht_remove(_ht_node_t* ht, void* key, _ls_compare cmp) {
	unsigned int result = 0;
	unsigned int hash_code = 0;
	_ls_node_t* bucket = 0;

	mb_assert(ht && key);

	if(!cmp)
		cmp = _ls_cmp_extra;

	bucket = _ht_find(ht, key);
	hash_code = ht->hash(ht, key);
	bucket = ht->array[hash_code];
	result = _ls_try_remove(bucket, key, cmp, ht->free_extra);
	ht->count -= result;

	return result;
}

unsigned int _ht_foreach(_ht_node_t* ht, _ht_operation op) {
	unsigned int result = 0;
	_ls_node_t* bucket = 0;
	unsigned int ul = 0;

	for(ul = 0; ul < ht->array_size; ++ul) {
		bucket = ht->array[ul];
		if(bucket)
			result += _ls_foreach(bucket, op);
	}

	return result;
}

unsigned int _ht_count(_ht_node_t* ht) {
	mb_assert(ht);

	return ht->count;
}

void _ht_clear(_ht_node_t* ht) {
	unsigned int ul = 0;

	mb_assert(ht && ht->array);

	for(ul = 0; ul < ht->array_size; ++ul)
		_ls_clear(ht->array[ul]);
	ht->count = 0;
}

void _ht_destroy(_ht_node_t* ht) {
	unsigned int ul = 0;

	mb_assert(ht && ht->array);

	if(ht->free_extra)
		_ht_foreach(ht, ht->free_extra);

	for(ul = 0; ul < ht->array_size; ++ul)
		_ls_destroy(ht->array[ul]);

	safe_free(ht->array);
	safe_free(ht);
}

int _ht_remove_exist(void* data, void* extra, _ht_node_t* ht) {
	int result = _OP_RESULT_NORMAL;
	mb_unrefvar(data);

	if(_ht_find(ht, extra))
		_ht_remove(ht, extra, 0);

	return result;
}

/** Memory manipulations */
void* mb_malloc(size_t s) {
	/* Allocate a chunk of memory with a specific size */
	char* ret = NULL;
	size_t rs = s;
#ifdef MB_ENABLE_ALLOC_STAT
	_MB_CHECK_MEM_TAG_SIZE(size_t, s);
	rs += _MB_MEM_TAG_SIZE;
#endif /* MB_ENABLE_ALLOC_STAT */
	if(_mb_allocate_func)
		ret = _mb_allocate_func((unsigned)rs);
	else
		ret = (char*)malloc(rs);
	mb_assert(ret);
#ifdef MB_ENABLE_ALLOC_STAT
	_mb_allocated += s;
	ret += _MB_MEM_TAG_SIZE;
	_MB_WRITE_MEM_TAG_SIZE(ret, s);
#endif /* MB_ENABLE_ALLOC_STAT */

	return (void*)ret;
}

void mb_free(void* p) {
	/* Free a chunk of memory */
	mb_assert(p);

#ifdef MB_ENABLE_ALLOC_STAT
	do {
		size_t os = _MB_READ_MEM_TAG_SIZE(p);
		_mb_allocated -= os;
		p = (char*)p - _MB_MEM_TAG_SIZE;
	} while(0);
#endif /* MB_ENABLE_ALLOC_STAT */

	if(_mb_free_func)
		_mb_free_func((char*)p);
	else
		free(p);
}

int mb_memcmp(void* l, void* r, size_t s) {
	/* Compare two chunks of memory */
	char* lc = (char*)l;
	char* rc = (char*)r;
	size_t i = 0;

	for(i = 0; i < s; i++) {
		if(lc[i] < rc[i]) return -1;
		else if(lc[i] > rc[i]) return 1;
	}

	return 0;
}

size_t mb_memtest(void* p, size_t s) {
	/* Detect whether a chunk of memory contains non-zero byte */
	size_t result = 0;
	size_t i = 0;
	for(i = 0; i < s; i++)
		result += ((unsigned char*)p)[i];

	return result;
}

char* mb_strdup(const char* p, size_t s) {
	/* Duplicate a string */
#ifdef MB_ENABLE_ALLOC_STAT
	if(!s) {
		s = _MB_READ_MEM_TAG_SIZE(p);
	}

	return mb_memdup(p, (unsigned)s);
#else /* MB_ENABLE_ALLOC_STAT */
	if(s)
		return mb_memdup(p, (unsigned)s);

	return mb_memdup(p, (unsigned)(strlen(p) + 1));
#endif /* MB_ENABLE_ALLOC_STAT */
}

char* mb_strupr(char* s) {
	/* Change a string to upper case */
	char* t = s;

	while(*s) {
		*s = toupper(*s);
		++s;
	}

	return t;
}

/** Unicode handling */
#ifdef MB_ENABLE_UNICODE
int mb_uu_ischar(char* ch) {
	/* Determine whether a buffer is a UTF8 encoded character, and return _TAKEn bytes */
#define _TAKE(__ch, __c, __r) do { __c = *__ch++; __r++; } while(0)
#define _COPY(__ch, __c, __r, __cp) do { _TAKE(__ch, __c, __r); __cp = (__cp << 6) | ((unsigned char)__c & 0x3Fu); } while(0)
#define _TRANS(__m, __cp, __g) do { __cp &= ((__g[(unsigned char)c] & __m) != 0); } while(0)
#define _TAIL(__ch, __c, __r, __cp, __g) do { _COPY(__ch, __c, __r, __cp); _TRANS(0x70, __cp, __g); } while(0)
	static const unsigned char range[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
		8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 11, 6, 6, 6, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	};

	int result = 0;
	unsigned codepoint = 0;
	unsigned char type = 0;
	char c = 0;

	if(!ch)
		return 0;

	_TAKE(ch, c, result);
	if(!(c & 0x80)) {
		codepoint = (unsigned char)c;

		return 1;
	}

	type = range[(unsigned char)c];
	codepoint = (0xFF >> type) & (unsigned char)c;

	switch (type) {
	case 2: _TAIL(ch, c, result, codepoint, range); return result;
	case 3: _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 4: _COPY(ch, c, result, codepoint); _TRANS(0x50, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 5: _COPY(ch, c, result, codepoint); _TRANS(0x10, codepoint, range); _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 6: _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 10: _COPY(ch, c, result, codepoint); _TRANS(0x20, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 11: _COPY(ch, c, result, codepoint); _TRANS(0x60, codepoint, range); _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	default: return 0;
	}
#undef _TAKE
#undef _COPY
#undef _TRANS
#undef _TAIL
}

int mb_uu_strlen(char* ch) {
	/* Tell how many UTF8 character are there in a string */
	int result = 0;

	if(!ch)
		return 0;

	while(*ch) {
		int t = mb_uu_ischar(ch);
		if(t <= 0) return t;
		ch += t;
		result++;
	}

	return result;
}

int mb_uu_substr(char* ch, int begin, int count, char** o) {
	/* Retrieve a sub string of a UTF8 string */
	int cnt = 0;
	char* b = 0;
	char* e = 0;
	int l = 0;

	if(!ch || begin < 0 || count <= 0 || !o)
		return -1;

	while(*ch) {
		int t = mb_uu_ischar(ch);
		if(t <= 0) return t;
		if(cnt == begin) {
			b = ch;

			break;
		}
		ch += t;
		cnt++;
	}

	while(*ch) {
		int t = mb_uu_ischar(ch);
		if(t <= 0) return t;
		if(cnt == begin + count) {
			e = ch;

			break;
		}
		ch += t;
		e = ch;
		cnt++;
	}

	if(!(*ch) && (cnt != begin + count))
		return -1;

	l = (int)(e - b);
	*o = (char*)mb_malloc(l + 1);
	memcpy(*o, b, l);
	(*o)[l] = '\0';

	return l;
}
#endif /* MB_ENABLE_UNICODE */

/** Expression processing */
bool_t _is_operator(mb_func_t op) {
	/* Determine whether a function is an operator */
	bool_t result = false;

	result =
		(op == _core_dummy_assign) ||
		(op == _core_add) ||
		(op == _core_min) ||
		(op == _core_mul) ||
		(op == _core_div) ||
		(op == _core_mod) ||
		(op == _core_pow) ||
		(op == _core_open_bracket) ||
		(op == _core_close_bracket) ||
		(op == _core_equal) ||
		(op == _core_less) ||
		(op == _core_greater) ||
		(op == _core_less_equal) ||
		(op == _core_greater_equal) ||
		(op == _core_not_equal) ||
		(op == _core_and) ||
		(op == _core_or) ||
		(op == _core_is);

	return result;
}

bool_t _is_flow(mb_func_t op) {
	/* Determine whether a function is for flow control */
	bool_t result = false;

	result =
		(op == _core_if) ||
		(op == _core_then) ||
		(op == _core_elseif) ||
		(op == _core_else) ||
		(op == _core_endif) ||
		(op == _core_for) ||
		(op == _core_to) ||
		(op == _core_step) ||
		(op == _core_next) ||
		(op == _core_while) ||
		(op == _core_wend) ||
		(op == _core_do) ||
		(op == _core_until) ||
		(op == _core_exit) ||
		(op == _core_goto) ||
		(op == _core_gosub) ||
		(op == _core_return) ||
		(op == _core_end);

	return result;
}

bool_t _is_unary(mb_func_t op) {
	/* Determine whether a function is unary */
	return op == _core_neg || op == _core_not;
}

char _get_priority(mb_func_t op1, mb_func_t op2) {
	/* Get the priority of two operators */
	char result = '\0';
	int idx1 = 0;
	int idx2 = 0;

	mb_assert(op1 && op2);

	idx1 = _get_priority_index(op1);
	idx2 = _get_priority_index(op2);
	mb_assert(idx1 < _countof(_PRECEDE_TABLE) && idx2 < _countof(_PRECEDE_TABLE[0]));
	result = _PRECEDE_TABLE[idx1][idx2];

	return result;
}

int _get_priority_index(mb_func_t op) {
	/* Get the index of an operator in the priority table */
	int i = 0;

	mb_func_t funcs[] = {
		_core_add,
		_core_min,
		_core_mul,
		_core_div,
		_core_mod,
		_core_pow,
		_core_open_bracket,
		_core_close_bracket,
		_core_dummy_assign,
		_core_greater,
		_core_less,
		_core_greater_equal,
		_core_less_equal,
		_core_equal,
		_core_not_equal,
		_core_and,
		_core_or,
		_core_not,
		_core_neg,
		_core_is
	};

	mb_assert(op);

	for(i = 0; i < _countof(funcs); i++) {
		if(op == funcs[i])
			return i;
	}

	mb_assert(0 && "Unknown operator.");

	return -1;
}

_object_t* _operate_operand(mb_interpreter_t* s, _object_t* optr, _object_t* opnd1, _object_t* opnd2, int* status) {
	/* Operate two operands */
	_object_t* result = 0;
	_tuple3_t tp;
	_tuple3_t* tpptr = 0;
	int _status = 0;

	mb_assert(s && optr);
	mb_assert(optr->type == _DT_FUNC);

	if(!opnd1)
		return result;

	result = _create_object();

	memset(&tp, 0, sizeof(_tuple3_t));
	tp.e1 = opnd1;
	tp.e2 = opnd2;
	tp.e3 = result;
	tpptr = &tp;

	_status = (optr->data.func->pointer)(s, (void**)&tpptr);
	if(status)
		*status = _status;
	if(_status != MB_FUNC_OK) {
		if(_status != MB_FUNC_WARNING) {
			safe_free(result);
		}
		_set_current_error(s, SE_RN_OPERATION_FAILED, 0);
#ifdef MB_ENABLE_SOURCE_TRACE
		_set_error_pos(s, optr->source_pos, optr->source_row, optr->source_col);
#else /* MB_ENABLE_SOURCE_TRACE */
		_set_error_pos(s, 0, 0, 0);
#endif /* MB_ENABLE_SOURCE_TRACE */
	}

	return result;
}

bool_t _is_expression_terminal(mb_interpreter_t* s, _object_t* obj) {
	/* Determine whether an object is an expression termination */
	bool_t result = false;

	mb_assert(s && obj);

	result =
		(obj->type == _DT_EOS) ||
		(obj->type == _DT_SEP) ||
		(obj->type == _DT_FUNC &&
			(obj->data.func->pointer == _core_then ||
			obj->data.func->pointer == _core_elseif ||
			obj->data.func->pointer == _core_else ||
			obj->data.func->pointer == _core_endif ||
			obj->data.func->pointer == _core_to ||
			obj->data.func->pointer == _core_step));

	return result;
}

int _calc_expression(mb_interpreter_t* s, _ls_node_t** l, _object_t** val) {
	/* Calculate an expression */
	int result = 0;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_ls_node_t* garbage = 0;
	_ls_node_t* optr = 0;
	_ls_node_t* opnd = 0;
	_object_t* c = 0;
	_object_t* x = 0;
	_object_t* a = 0;
	_object_t* b = 0;
	_object_t* r = 0;
	_object_t* theta = 0;
	char pri = '\0';
	int* inep = 0;
	int f = 0;

	_object_t* guard_val = 0;
	int bracket_count = 0;
	bool_t hack = false;
	_ls_node_t* errn = 0;

	mb_assert(s && l);

	running = s->running_context;
	ast = *l;
	garbage = _ls_create();
	optr = _ls_create();
	opnd = _ls_create();

	inep = (int*)mb_malloc(sizeof(int));
	*inep = 0;
	_ls_pushback(s->in_neg_expr, inep);

	c = (_object_t*)ast->data;
	do {
		if(c->type == _DT_STRING) {
			if(ast->next) {
				_object_t* _fsn = (_object_t*)ast->next->data;
				if(_IS_FUNC(_fsn, _core_add) || _IS_FUNC(_fsn, _core_and) || _IS_FUNC(_fsn, _core_or))
					break;
			}

			(*val)->type = _DT_STRING;
			(*val)->data.string = c->data.string;
			(*val)->ref = true;
			ast = ast->next;

			goto _exit;
		}
	} while(0);
	guard_val = c;
	ast = ast->next;
	_ls_pushback(optr, _exp_assign);
	while(
		!(c->type == _DT_FUNC && strcmp(c->data.func->name, "#") == 0) ||
		!(((_object_t*)(_ls_back(optr)->data))->type == _DT_FUNC && strcmp(((_object_t*)(_ls_back(optr)->data))->data.func->name, "#") == 0)) {
		if(!hack) {
			if(_IS_FUNC(c, _core_open_bracket)) {
				++bracket_count;
			} else if(_IS_FUNC(c, _core_close_bracket)) {
				--bracket_count;
				if(bracket_count < 0) {
					c = _exp_assign;
					ast = ast->prev;

					continue;
				}
			}
		}
		hack = false;
		if(!(c->type == _DT_FUNC && _is_operator(c->data.func->pointer))) {
			if(_is_expression_terminal(s, c)) {
				c = _exp_assign;
				if(ast)
					ast = ast->prev;
				if(bracket_count) {
					_object_t _cb;
					_func_t _cbf;
					_MAKE_NIL(&_cb);
					_cb.type = _DT_FUNC;
					_cb.data.func = &_cbf;
					_cb.data.func->name = ")";
					_cb.data.func->pointer = _core_close_bracket;
					while(bracket_count) {
						_ls_pushback(optr, &_cb);
						bracket_count--;
						f = 0;
					}
					errn = ast;
				}
			} else {
				if(c->type == _DT_ARRAY) {
					unsigned int arr_idx = 0;
					mb_value_u arr_val;
					_data_e arr_type;
					_object_t* arr_elem = 0;

_array:
					if(ast && !_IS_FUNC(((_object_t*)ast->data), _core_open_bracket)) {
						_ls_pushback(opnd, c);
						f++;
					} else {
						ast = ast->prev;
						result = _get_array_index(s, &ast, c, &arr_idx, 0);
						if(result != MB_FUNC_OK) {
							_handle_error_on_obj(s, SE_RN_CALCULATION_ERROR, 0, DON(ast), MB_FUNC_ERR, _exit, result);
						}
						ast = ast->next;
						_get_array_elem(s, c->data.array, arr_idx, &arr_val, &arr_type);
						arr_elem = _create_object();
						_ls_pushback(garbage, arr_elem);
						arr_elem->type = arr_type;
						arr_elem->ref = true;
						_copy_bytes(arr_elem->data.bytes, arr_val.bytes);
						if(f) {
							_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
						}
						_ls_pushback(opnd, arr_elem);
						f++;
					}
				} else if(c->type == _DT_FUNC) {
					ast = ast->prev;
					if(_IS_UNARY_FUNC(c)) {
#ifdef MB_ENABLE_STACK_TRACE
						_ls_pushback(s->stack_frames, c->data.func->name);
#endif /* MB_ENABLE_STACK_TRACE */
						result = (c->data.func->pointer)(s, (void**)&ast);
#ifdef MB_ENABLE_STACK_TRACE
						_ls_popback(s->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */
					} else {
						int calc_depth = running->calc_depth;
						running->calc_depth = _INFINITY_CALC_DEPTH;
						result = (c->data.func->pointer)(s, (void**)&ast);
						running->calc_depth = calc_depth;
					}
					if(result != MB_FUNC_OK) {
						_handle_error_on_obj(s, SE_RN_CALCULATION_ERROR, 0, DON(ast), MB_FUNC_ERR, _exit, result);
					}
					c = _create_object();
					_ls_pushback(garbage, c);
					result = _public_value_to_internal_object(&running->intermediate_value, c);
					if(c->type == _DT_STRING)
						c->ref = true;
					if(result != MB_FUNC_OK)
						goto _exit;
					if(f) {
						_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
					}
					if(_is_array(c)) {
						goto _array;
					} else {
						if(ast && _IS_FUNC(ast->data, _core_open_bracket)) {
							_handle_error_on_obj(s, SE_RN_SYNTAX, 0, DON(ast), MB_FUNC_ERR, _exit, result);
						}
					}
					_ls_pushback(opnd, c);
					f++;
				} else if(c->type == _DT_ROUTINE) {
_routine:
					ast = ast->prev;
					result = _eval_routine(s, &ast, 0, 0, c->data.routine, _has_routine_lex_arg, _pop_routine_lex_arg);
					if(ast)
						ast = ast->prev;
					if(result != MB_FUNC_OK) {
						_handle_error_on_obj(s, SE_RN_CALCULATION_ERROR, 0, DON(ast), MB_FUNC_ERR, _exit, result);
					}
					c = _create_object();
					_ls_pushback(garbage, c);
					result = _public_value_to_internal_object(&running->intermediate_value, c);
					if(result != MB_FUNC_OK)
						goto _exit;
					if(f) {
						_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
					}
					_ls_pushback(opnd, c);
					f++;
				} else if(c->type == _DT_VAR && c->data.variable->data->type == _DT_ARRAY) {
					unsigned int arr_idx = 0;
					mb_value_u arr_val;
					_data_e arr_type;
					_object_t* arr_elem = 0;

					ast = ast->prev;
					result = _get_array_index(s, &ast, 0, &arr_idx, 0);
					if(result != MB_FUNC_OK) {
						_handle_error_on_obj(s, SE_RN_CALCULATION_ERROR, 0, DON(ast), MB_FUNC_ERR, _exit, result);
					}
					ast = ast->next;
					_get_array_elem(s, c->data.variable->data->data.array, arr_idx, &arr_val, &arr_type);
					arr_elem = _create_object();
					_ls_pushback(garbage, arr_elem);
					arr_elem->type = arr_type;
					arr_elem->ref = true;
					if(arr_type == _DT_INT) {
						arr_elem->data.integer = arr_val.integer;
					} else if(arr_type == _DT_REAL) {
						arr_elem->data.float_point = arr_val.float_point;
					} else if(arr_type == _DT_STRING) {
						arr_elem->data.string = arr_val.string;
					} else if(arr_type == _DT_USERTYPE) {
						arr_elem->data.usertype = arr_val.usertype;
					} else {
						mb_assert(0 && "Unsupported.");
					}
					if(f) {
						_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
					}
					_ls_pushback(opnd, arr_elem);
					f++;
				} else {
					if(c->type == _DT_VAR) {
						_ls_node_t* cs = _search_identifier_in_scope_chain(s, 0, c->data.variable->name,
#ifdef MB_ENABLE_CLASS
							c->data.variable->pathing,
#else /* MB_ENABLE_CLASS */
							0,
#endif /* MB_ENABLE_CLASS */
							0
						);
						if(cs) {
							c = (_object_t*)cs->data;
							if(c && c->type == _DT_ROUTINE)
								goto _routine;
						}
						if(ast) {
							_object_t* _err_or_bracket = (_object_t*)ast->data;
							do {
#ifdef MB_ENABLE_COLLECTION_LIB
								if(_IS_VAR(c) && _IS_COLL(c->data.variable->data)) {
									if(_IS_FUNC(_err_or_bracket, _core_open_bracket)) {
										int_t idx = 0;
										mb_value_t key;
										mb_value_t ret;
										_object_t* ocoll = c->data.variable->data;

										mb_make_nil(ret);

										mb_check(mb_attempt_open_bracket(s, (void**)l));

										switch(ocoll->type) {
										case _DT_LIST:
											mb_check(mb_pop_int(s, (void**)l, &idx));
											if(!_at_list(ocoll->data.list, idx, &ret)) {
												_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
											}

											break;
										case _DT_DICT:
											mb_make_nil(key);
											mb_check(mb_pop_value(s, (void**)l, &key));
											if(!_find_dict(ocoll->data.dict, &key, &ret)) {
												_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
											}

											break;
										default: /* Do nothing */
											break;
										}

										mb_check(mb_attempt_close_bracket(s, (void**)l));

										_dispose_object(c);
										_public_value_to_internal_object(&ret, c);

										mb_make_nil(ret);
										_internal_object_to_public_value(ocoll, &ret);
										_assign_public_value(&ret, 0);

										ast = (_ls_node_t*)*l;
									}

									break;
								}
#endif /* MB_ENABLE_COLLECTION_LIB */
								if(_IS_FUNC(_err_or_bracket, _core_open_bracket)) {
									_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, 0, DON(ast), MB_FUNC_ERR, _exit, result);
								}
							} while(0);
						}
					}
					if(f) {
						_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
					}
					_ls_pushback(opnd, c);
					f++;
				}
				if(running->calc_depth != _INFINITY_CALC_DEPTH)
					running->calc_depth--;
				if(ast && (running->calc_depth == _INFINITY_CALC_DEPTH || running->calc_depth)) {
					c = (_object_t*)ast->data;
					if(c->type == _DT_FUNC && !_is_operator(c->data.func->pointer) && !_is_flow(c->data.func->pointer)) {
						_ls_foreach(opnd, _remove_source_object);

						_handle_error_on_obj(s, SE_RN_COLON_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
					}
					ast = ast->next;
				} else {
					c = _exp_assign;
				}
			}
		} else {
			pri = _get_priority(((_object_t*)(_ls_back(optr)->data))->data.func->pointer, c->data.func->pointer);
			switch(pri) {
			case '<':
				_ls_pushback(optr, c);
				c = (_object_t*)ast->data;
				ast = ast->next;
				f = 0;

				break;
			case '=':
				x = (_object_t*)_ls_popback(optr);
				c = (_object_t*)ast->data;
				ast = ast->next;

				break;
			case '>':
				theta = (_object_t*)_ls_popback(optr);
				b = (_object_t*)_ls_popback(opnd);
				a = (_object_t*)_ls_popback(opnd);
				r = _operate_operand(s, theta, a, b, &result);
				if(!r) {
					_ls_clear(optr);
					_handle_error_on_obj(s, SE_RN_OPERATION_FAILED, 0, DON(errn), MB_FUNC_ERR, _exit, result);
				}
				_ls_pushback(opnd, r);
				_ls_pushback(garbage, r);
				if(_IS_FUNC(c, _core_close_bracket))
					hack = true;

				break;
			}
		}
	}

	if(errn) {
		_handle_error_on_obj(s, SE_RN_CLOSE_BRACKET_EXPECTED, 0, DON(errn), MB_FUNC_ERR, _exit, result);
	}

	c = (_object_t*)(_ls_popback(opnd));
	if(!c || !(c->type == _DT_TYPE || c->type == _DT_NIL ||
		c->type == _DT_INT || c->type == _DT_REAL || c->type == _DT_STRING ||
#ifdef MB_ENABLE_COLLECTION_LIB
		c->type == _DT_LIST || c->type == _DT_LIST_IT || c->type == _DT_DICT || c->type == _DT_DICT_IT ||
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		c->type == _DT_CLASS ||
#endif /* MB_ENABLE_CLASS */
		c->type == _DT_ROUTINE ||
		c->type == _DT_VAR || c->type == _DT_USERTYPE || c->type == _DT_USERTYPE_REF || c->type == _DT_ARRAY)
	) {
		_set_current_error(s, SE_RN_INVALID_DATA_TYPE, 0);
		result = MB_FUNC_ERR;

		goto _exit;
	}
	if(c->type == _DT_VAR) {
		(*val)->type = c->data.variable->data->type;
		(*val)->data = c->data.variable->data->data;
		if(_is_string(c))
			(*val)->ref = true;
	} else {
		(*val)->type = c->type;
		if(_is_string(c)) {
			char* _str = _extract_string(c);
			(*val)->data.string = mb_strdup(_str, strlen(_str) + 1);
			(*val)->ref = false;
		} else if(c->type == _DT_ARRAY) {
			(*val)->data = c->data;
			c->type = _DT_NIL;
		} else {
			(*val)->data = c->data;
		}
	}
	if(guard_val != c && _ls_try_remove(garbage, c, _ls_cmp_data, NULL)) {
		_try_clear_intermediate_value(c, 0, s);

		if(c->type == _DT_USERTYPE_REF ||
#ifdef MB_ENABLE_COLLECTION_LIB
			c->type == _DT_LIST || c->type == _DT_DICT || c->type == _DT_LIST_IT || c->type == _DT_DICT_IT ||
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
			c->type == _DT_CLASS ||
#endif /* MB_ENABLE_CLASS */
			c->type == _DT_ARRAY ||
			c->type == _DT_ROUTINE
		) {
			_destroy_object_capsule_only(c, 0);
		} else {
			_destroy_object(c, 0);
		}
	}

_exit:
	_LS_FOREACH(garbage, _destroy_object, _try_clear_intermediate_value, s);
	_ls_destroy(garbage);
	_ls_foreach(optr, _destroy_object_not_compile_time);
	_ls_foreach(opnd, _destroy_object_not_compile_time);
	_ls_destroy(optr);
	_ls_destroy(opnd);
	*l = ast;
	mb_free(_ls_popback(s->in_neg_expr));

	return result;
}

int _proc_args(mb_interpreter_t* s, _ls_node_t** l, _running_context_t* running, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg, bool_t proc_ref) {
	/* Process arguments of a routine */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	_ls_node_t* pars = 0;
	_var_t* var = 0;
	_ls_node_t* rnode = 0;
	unsigned ia = 0;

	if(r->func.basic.parameters) {
		mb_make_nil(arg);
		pars = r->func.basic.parameters;
		pars = pars->next;
		while(pars && (!has_arg || (has_arg && has_arg(s, (void**)l, va, ca, &ia, r)))) {
			_object_t* obj = 0;
			if(pop_arg) {
				mb_make_nil(arg);
				mb_check(pop_arg(s, (void**)l, va, ca, &ia, r, &arg));
			}

			var = (_var_t*)pars->data;
			pars = pars->next;
			if(running->meta == _SCOPE_META_REF) {
				obj = (_object_t*)(_ht_find(running->var_dict, var->name)->data);
				var = obj->data.variable;

				if(proc_ref)
					var->data->ref = false;
			} else {
				rnode = _search_identifier_in_scope_chain(s, running, var->name, 0, 0);
				if(rnode)
					var = ((_object_t*)rnode->data)->data.variable;

				if(proc_ref)
					var->data->ref = true;
			}

			result = _public_value_to_internal_object(&arg, var->data);
			if(result != MB_FUNC_OK)
				break;
		}
	}

	return result;
}

int _eval_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg) {
	/* Evaluate a routine */
	int result = MB_FUNC_OK;

#ifdef MB_ENABLE_STACK_TRACE
	_ls_pushback(s->stack_frames, r->name);
#endif /* MB_ENABLE_STACK_TRACE */

	if(r->type == _IT_BASIC && r->func.basic.entry) {
		result = _eval_script_routine(s, l, va, ca, r, has_arg, pop_arg);
	} else if(r->type == _IT_NATIVE && r->func.native.entry) {
		result = _eval_native_routine(s, l, va, ca, r, has_arg, pop_arg);
	} else {
		_handle_error_on_obj(s, SE_RN_INVALID_ROUTINE, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

_exit:
#ifdef MB_ENABLE_STACK_TRACE
	_ls_popback(s->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */

	return result;
}

int _eval_script_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg) {
	/* Evaluate a script routine */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_routine_t* lastr = 0;
	mb_value_t inte;

	mb_assert(s && l && r);

	if(!va && s->last_routine && !s->last_routine->func.basic.parameters && (s->last_routine->name == r->name || !strcmp(s->last_routine->name, r->name))) {
		ast = (_ls_node_t*)*l;
		_skip_to(s, &ast, 0, _DT_EOS);
		if(ast && ((_object_t*)ast->data)->type == _DT_EOS)
			ast = ast->next;
		if(_IS_FUNC((_object_t*)ast->data, _core_enddef)) { /* Tail recursion optimization */
			*l = r->func.basic.entry;
			if(*l)
				*l = (*l)->next;

			goto _tail;
		}
	}

	lastr = s->last_routine;
	s->last_routine = r;

	if(!va) {
		mb_check(mb_attempt_open_bracket(s, (void**)l));
	}

	running = _push_weak_scope_by_routine(s, r->func.basic.scope, r);
	result = _proc_args(s, l, running, va, ca, r, has_arg, pop_arg, true);
	if(result != MB_FUNC_OK) {
		if(running->meta == _SCOPE_META_REF)
			_unreference_scope(s, running);
		else
			_pop_weak_scope(s, running);

		goto _exit;
	}
	running = _pop_weak_scope(s, running);

	if(!va) {
		mb_check(mb_attempt_close_bracket(s, (void**)l));
	}

	ast = (_ls_node_t*)*l;
	_ls_pushback(s->sub_stack, ast);

	running = _push_scope_by_routine(s, running);

	*l = r->func.basic.entry;
	if(!(*l)) {
		_handle_error_on_obj(s, SE_RN_INVALID_ROUTINE, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	do {
		result = _execute_statement(s, l);
		ast = (_ls_node_t*)*l;
		if(result == MB_SUB_RETURN) {
			result = MB_FUNC_OK;

			break;
		}
		if(result == MB_FUNC_SUSPEND && s->error_handler) {
			_handle_error_now(s, SE_RN_DONT_SUSPEND_IN_A_ROUTINE, s->last_error_func, result);

			goto _exit;
		}
		if(result != MB_FUNC_OK && s->error_handler) {
			if(result >= MB_EXTENDED_ABORT)
				s->last_error = SE_EA_EXTENDED_ABORT;
			_handle_error_now(s, s->last_error, s->last_error_func, result);

			goto _exit;
		}
	} while(ast);

	result = _proc_args(s, l, running, 0, 0, r, 0, 0, false);
	if(result != MB_FUNC_OK)
		goto _exit;

	mb_make_nil(inte);

	_swap_public_value(&inte, &running->intermediate_value);

	_pop_scope(s);

	_assign_public_value(&s->running_context->intermediate_value, &inte);

_exit:
	if(result != MB_FUNC_OK)
		_pop_scope(s);

	s->last_routine = lastr;

_tail:
	return result;
}

int _eval_native_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg) {
	/* Evaluate a native routine */
	int result = MB_FUNC_OK;
	_routine_t* lastr = 0;
	mb_routine_func_t entry = 0;

	mb_assert(s && l && r);

	lastr = s->last_routine;
	s->last_routine = r;

	entry = r->func.native.entry;
	if(!entry) {
		_handle_error_on_obj(s, SE_RN_INVALID_ROUTINE, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

	result = entry(s, (void**)l, va, ca, r, has_arg, pop_arg);

_exit:
	s->last_routine = lastr;

	return result;
}

int _has_routine_lex_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r) {
	/* Detect if there is any more lexical argument */
	mb_unrefvar(va);
	mb_unrefvar(ca);
	mb_unrefvar(ia);
	mb_unrefvar(r);

	return mb_has_arg(s, l);
}

int _pop_routine_lex_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r, mb_value_t* val) {
	/* Pop a lexical argument */
	mb_unrefvar(va);
	mb_unrefvar(ca);
	mb_unrefvar(ia);
	mb_unrefvar(r);

	return mb_pop_value(s, l, val);
}

int _has_routine_fun_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r) {
	/* Detect if there is any more argument in the argument list */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(va);
	mb_unrefvar(r);

	return *ia < ca;
}

int _pop_routine_fun_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r, mb_value_t* val) {
	/* Pop an argument from the argument list */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(ca);
	mb_unrefvar(r);

	memcpy(val, &(va[*ia]), sizeof(mb_value_t));
	(*ia)++;

	return MB_FUNC_OK;
}

bool_t _is_print_terminal(mb_interpreter_t* s, _object_t* obj) {
	/* Determine whether an object is a PRINT termination */
	bool_t result = false;

	mb_assert(s && obj);

	result = _IS_EOS(obj) ||
		_IS_SEP(obj, ':') ||
		_IS_FUNC(obj, _core_elseif) ||
		_IS_FUNC(obj, _core_else) ||
		_IS_FUNC(obj, _core_endif);

	return result;
}

/** Others */
void _set_current_error(mb_interpreter_t* s, mb_error_e err, char* func) {
	/* Set current error information */
	mb_assert(s && err >= 0 && err < _countof(_ERR_DESC));

	if(s->last_error == SE_NO_ERR) {
		s->last_error = err;
		s->last_error_func = func;
	}
}

mb_print_func_t _get_printer(mb_interpreter_t* s) {
	/* Get a print functor according to an interpreter */
	mb_assert(s);

	if(s->printer)
		return s->printer;

	return printf;
}

mb_input_func_t _get_inputer(mb_interpreter_t* s) {
	/* Get an input functor according to an interpreter */
	mb_assert(s);

	if(s->inputer)
		return s->inputer;

	return mb_gets;
}

char* _load_file(mb_interpreter_t* s, const char* f, const char* prefix) {
	/* Read all content of a file into a buffer */
#ifndef ARDUINO
	FILE* fp = 0;
	char* buf = 0;
	long curpos = 0;
	long l = 0;
	long i = 0;
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = (_parsing_context_t*)s->parsing_context;

	if(_ls_find(context->imported, (void*)f, (_ls_compare)_ht_cmp_string)) {
		buf = (char*)f;
	} else {
		buf = mb_strdup(f, strlen(f) + 1);
		_ls_pushback(context->imported, buf);
		buf = 0;

		fp = fopen(f, "rb");
		if(fp) {
			curpos = ftell(fp);
			fseek(fp, 0L, SEEK_END);
			l = ftell(fp);
			fseek(fp, curpos, SEEK_SET);
			if(prefix) {
				i = (long)strlen(prefix);
				l += i;
			}
			buf = (char*)mb_malloc((size_t)(l + 1));
			mb_assert(buf);
			if(prefix) {
				memcpy(buf, prefix, i);
			}
			fread(buf + i, 1, l, fp);
			fclose(fp);
			buf[l] = '\0';
		}
	}

	return buf;
#else /* ARDUINO */
	return 0;
#endif /* ARDUINO */
}

bool_t _is_blank(char c) {
	/* Determine whether a character is a blank */
	return (' ' == c) || ('\t' == c) ||
		(-17 == c) || (-69 == c) || (-65 == c) ||
		(-2 == c) || (-1 == c);
}

bool_t _is_newline(char c) {
	/* Determine whether a character is a newline */
	return ('\r' == c) || ('\n' == c) || (EOF == c);
}

bool_t _is_separator(char c) {
	/* Determine whether a character is a separator */
	return (',' == c) || (';' == c) || (':' == c);
}

bool_t _is_bracket(char c) {
	/* Determine whether a character is a bracket */
	return ('(' == c) || (')' == c);
}

bool_t _is_quotation_mark(char c) {
	/* Determine whether a character is a quotation mark */
	return ('"' == c);
}

bool_t _is_comment(char c) {
	/* Determine whether a character is a comment mark */
	return ('\'' == c);
}

bool_t _is_accessor(char c) {
	/* Determine whether a character is an accessor char */
	return c == '.';
}

bool_t _is_numeric_char(char c) {
	/* Determine whether a character is a numeric char */
	return (c >= '0' && c <= '9') || _is_accessor(c);
}

bool_t _is_identifier_char(char c) {
	/* Determine whether a character is an identifier char */
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
		(c == '_') ||
		_is_numeric_char(c) ||
		(c == '$');
}

bool_t _is_operator_char(char c) {
	/* Determine whether a character is an operator char */
	return (c == '+') || (c == '-') || (c == '*') || (c == '/') ||
		(c == '^') ||
		(c == '(') || (c == ')') ||
		(c == '=') ||
		(c == '>') || (c == '<');
}

bool_t _is_using_char(char c) {
	/* Determine whether a character is a module using char */
	return c == '@';
}

bool_t _is_exponent_prefix(char* s, int begin, int end) {
	/* Determine whether current symbol is an exponent prefix */
	int i = 0;

	mb_assert(s);

	if(end < 0)
		return false;

	for(i = begin; i <= end; i++) {
		if(!_is_numeric_char(s[i]))
			return false;
	}

	return true;
}

int _append_char_to_symbol(mb_interpreter_t* s, char c) {
	/* Parse a character and append it to current parsing symbol */
	int result = MB_FUNC_OK;
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;

	if(_is_accessor(c))
		context->current_symbol_contains_accessor++;

	if(context->current_symbol_nonius + 1 >= _SINGLE_SYMBOL_MAX_LENGTH) {
		_set_current_error(s, SE_PS_SYMBOL_TOO_LONG, 0);

		++result;
	} else {
		context->current_symbol[context->current_symbol_nonius] = c;
		++context->current_symbol_nonius;
	}

	return result;
}

int _cut_symbol(mb_interpreter_t* s, int pos, unsigned short row, unsigned short col) {
	/* Current symbol parsing done and cut it */
	int result = MB_FUNC_OK;
	_parsing_context_t* context = 0;
	char* sym = 0;
	int status = 0;
	bool_t delsym = false;

	mb_assert(s);

	context = s->parsing_context;
	if(context->current_symbol_nonius && context->current_symbol[0] != '\0') {
		sym = (char*)mb_malloc(context->current_symbol_nonius + 1);
		memcpy(sym, context->current_symbol, context->current_symbol_nonius + 1);

		status = _append_symbol(s, sym, &delsym, pos, row, col);
		if(status || delsym) {
			safe_free(sym);
		}
		result = status;
	}
	memset(context->current_symbol, 0, sizeof(context->current_symbol));
	context->current_symbol_nonius = 0;
	context->current_symbol_contains_accessor = 0;

	return result;
}

int _append_symbol(mb_interpreter_t* s, char* sym, bool_t* delsym, int pos, unsigned short row, unsigned short col) {
	/* Append cut current symbol to AST list */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_ls_node_t** assign = 0;
	_ls_node_t* node = 0;
	_parsing_context_t* context = 0;

	mb_assert(s && sym);

	ast = s->ast;
	result = _create_symbol(s, ast, sym, &obj, &assign, delsym);
	if(obj) {
#ifdef MB_ENABLE_SOURCE_TRACE
		obj->source_pos = pos;
		obj->source_row = row;
		obj->source_col = col;
#else /* MB_ENABLE_SOURCE_TRACE */
		mb_unrefvar(row);
		mb_unrefvar(col);
		obj->source_pos = (char)pos;
#endif /* MB_ENABLE_SOURCE_TRACE */

		node = _ls_pushback(ast, obj);
		if(assign)
			*assign = node;

		context = s->parsing_context;
		context->last_symbol = obj;
	}

	return result;
}

int _create_symbol(mb_interpreter_t* s, _ls_node_t* l, char* sym, _object_t** obj, _ls_node_t*** asgn, bool_t* delsym) {
	/* Create a syntax symbol */
	int result = MB_FUNC_OK;
	_data_e type;
	union {
		_func_t* func; _array_t* array;
#ifdef MB_ENABLE_CLASS
		_class_t* instance;
#endif /* MB_ENABLE_CLASS */
		_routine_t* routine; _var_t* var; _label_t* label; real_t float_point; int_t integer; _raw_t any;
	} tmp;
	_raw_t value;
	unsigned int ul = 0;
	_parsing_context_t* context = 0;
	_running_context_t* running = 0;
	_ls_node_t* glbsyminscope = 0;
	bool_t is_field = false;
	mb_unrefvar(l);

	mb_assert(s && sym && obj);

	memset(value, 0, sizeof(_raw_t));

	context = s->parsing_context;
	running = s->running_context;

	*obj = _create_object();
#ifdef MB_ENABLE_SOURCE_TRACE
	(*obj)->source_pos = -1;
	(*obj)->source_row = (*obj)->source_col = 0xffff;
#else /* MB_ENABLE_SOURCE_TRACE */
	(*obj)->source_pos = -1;
#endif /* MB_ENABLE_SOURCE_TRACE */

	type = _get_symbol_type(s, sym, &value);
	if(s->last_error != SE_NO_ERR) {
		result = MB_FUNC_ERR;

		goto _exit;
	}
	(*obj)->type = type;
	switch(type) {
	case _DT_NIL:
		memcpy(tmp.any, value, sizeof(_raw_t));
		if(tmp.integer) { /* Nil type */
			(*obj)->type = _DT_NIL;
		} else { /* End of line character */
			safe_free(*obj);
		}
		safe_free(sym);

		break;
	case _DT_INT:
		memcpy(tmp.any, value, sizeof(_raw_t));
		(*obj)->data.integer = tmp.integer;
		safe_free(sym);

		break;
	case _DT_REAL:
		memcpy(tmp.any, value, sizeof(_raw_t));
		(*obj)->data.float_point = tmp.float_point;
		safe_free(sym);

		break;
	case _DT_STRING: {
			size_t _sl = strlen(sym);
			(*obj)->data.string = (char*)mb_malloc(_sl - 2 + 1);
			memcpy((*obj)->data.string, sym + sizeof(char), _sl - 2);
			(*obj)->data.string[_sl - 2] = '\0';
			*delsym = true;
		}

		break;
	case _DT_FUNC:
		tmp.func = (_func_t*)mb_malloc(sizeof(_func_t));
		memset(tmp.func, 0, sizeof(_func_t));
		tmp.func->name = sym;
		memcpy(&tmp.func->pointer, value, sizeof(tmp.func->pointer));
		(*obj)->data.func = tmp.func;

		break;
	case _DT_ARRAY:
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_ARRAY) {
			(*obj)->data.array = ((_object_t*)glbsyminscope->data)->data.array;
			(*obj)->ref = true;
			*delsym = true;
		} else {
			tmp.array = _create_array(sym, _DT_UNKNOWN, s);
			memcpy(&tmp.array->type, value, sizeof(tmp.array->type));
			(*obj)->data.array = tmp.array;

			ul = _ht_set_or_insert(running->var_dict, sym, *obj);
			mb_assert(ul);

			*obj = _create_object();
			(*obj)->type = type;
			(*obj)->data.array = tmp.array;
			(*obj)->ref = true;
		}

		break;
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_CLASS) {
			(*obj)->data.instance = ((_object_t*)glbsyminscope->data)->data.instance;
			(*obj)->ref = true;
			*delsym = true;
			if(running != (*obj)->data.instance->scope &&
				(context->class_state != _CLASS_STATE_NONE) &&
				_IS_FUNC(context->last_symbol, _core_class)) {
				_push_scope_by_class(s, (*obj)->data.instance->scope);
			}
		} else {
			_var_t* var = 0;

			tmp.instance = (_class_t*)mb_malloc(sizeof(_class_t));
			_init_class(s, tmp.instance, sym);
			_push_scope_by_class(s, tmp.instance->scope);
			s->last_instance = tmp.instance;

#if _CLASS_IN_VAR
			var = _create_var(obj, sym, true);

			ul = _ht_set_or_insert(running->var_dict, sym, *obj);
			mb_assert(ul);

			var->data->type = _DT_CLASS;
			var->data->data.instance = tmp.instance;

			*obj = _create_object();
			(*obj)->type = type;
			(*obj)->data.variable = tmp.var;
			(*obj)->ref = true;
#else /* _CLASS_IN_VAR */
			mb_unrefvar(var);

			(*obj)->data.instance = tmp.instance;

			ul = _ht_set_or_insert(running->var_dict, sym, *obj);
			mb_assert(ul);

			*obj = _create_object();
			(*obj)->type = type;
			(*obj)->data.instance = tmp.instance;
			(*obj)->ref = true;
#endif /* _CLASS_IN_VAR */
		}

		break;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_ROUTINE) {
			(*obj)->data.routine = ((_object_t*)glbsyminscope->data)->data.routine;
			(*obj)->ref = true;
			*delsym = true;
			if(running != (*obj)->data.routine->func.basic.scope &&
				context->routine_state &&
				_IS_FUNC(context->last_symbol, _core_def)) {
				_push_scope_by_routine(s, (*obj)->data.routine->func.basic.scope);
			}
		} else {
			_running_context_t* tba = 0;
			tmp.routine = (_routine_t*)mb_malloc(sizeof(_routine_t));
			_init_routine(s, tmp.routine, sym, 0);
			_push_scope_by_routine(s, tmp.routine->func.basic.scope);
			(*obj)->data.routine = tmp.routine;

			tba = _get_scope_to_add_routine(s);
			ul = _ht_set_or_insert(tba->var_dict, sym, *obj);
			mb_assert(ul);
			if(tba != _OUTTER_SCOPE(running) && tba != running)
				_pop_scope(s);

			*obj = _create_object();
			(*obj)->type = type;
			(*obj)->data.routine = tmp.routine;
			(*obj)->ref = true;

#ifdef MB_ENABLE_CLASS
			tmp.routine->instance = s->last_instance;
#endif /* MB_ENABLE_CLASS */
		}

		break;
	case _DT_VAR:
		if(context->routine_params_state)
			glbsyminscope = _ht_find(running->var_dict, sym);
		else
			glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, 0, 0);
#ifdef MB_ENABLE_CLASS
		is_field = context->last_symbol && _IS_FUNC(context->last_symbol, _core_var);
#endif /* MB_ENABLE_CLASS */
		if(!is_field && glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_VAR) {
			(*obj)->data.variable = ((_object_t*)glbsyminscope->data)->data.variable;
			(*obj)->ref = true;
			*delsym = true;
		} else {
			tmp.var = (_var_t*)mb_malloc(sizeof(_var_t));
			memset(tmp.var, 0, sizeof(_var_t));
			tmp.var->name = sym;
			tmp.var->data = _create_object();
			tmp.var->data->type = (sym[strlen(sym) - 1] == '$') ? _DT_STRING : _DT_INT;
			tmp.var->data->data.integer = 0;
#ifdef MB_ENABLE_CLASS
			if(!is_field)
				tmp.var->pathing = context->current_symbol_contains_accessor;
#endif /* MB_ENABLE_CLASS */
			(*obj)->data.variable = tmp.var;

			ul = _ht_set_or_insert(running->var_dict, sym, *obj);
			mb_assert(ul);

			*obj = _create_object();
			(*obj)->type = type;
			(*obj)->data.variable = tmp.var;
			(*obj)->ref = true;
		}

		break;
	case _DT_LABEL:
		if(context->current_char == ':') {
			if(mb_memtest(value, sizeof(_raw_t))) {
				memcpy(&((*obj)->data.label), value, sizeof((*obj)->data.label));
				(*obj)->ref = true;
				*delsym = true;
			} else {
				tmp.label = (_label_t*)mb_malloc(sizeof(_label_t));
				memset(tmp.label, 0, sizeof(_label_t));
				tmp.label->name = sym;
				*asgn = &(tmp.label->node);
				(*obj)->data.label = tmp.label;

				ul = _ht_set_or_insert(running->var_dict, sym, *obj);
				mb_assert(ul);

				*obj = _create_object();
				(*obj)->type = type;
				(*obj)->data.label = tmp.label;
				(*obj)->ref = true;
			}
		} else {
			(*obj)->data.label = (_label_t*)mb_malloc(sizeof(_label_t));
			memset((*obj)->data.label, 0, sizeof(_label_t));
			(*obj)->data.label->name = sym;
		}

		break;
	case _DT_SEP:
		(*obj)->data.separator = sym[0];
		safe_free(sym);

		break;
	case _DT_EOS:
		safe_free(sym);

		break;
	default:
		break;
	}

_exit:
	return result;
}

_data_e _get_symbol_type(mb_interpreter_t* s, char* sym, _raw_t* value) {
	/* Get the type of a syntax symbol */
	_data_e result = _DT_NIL;
	union { real_t float_point; int_t integer; _object_t* obj; _raw_t any; } tmp;
	char* conv_suc = 0;
	_parsing_context_t* context = 0;
	_running_context_t* running = 0;
	_ls_node_t* glbsyminscope = 0;
	size_t _sl = 0;
	_data_e en = _DT_UNKNOWN;
	intptr_t ptr = 0;
	bool_t mod = false;

	mb_assert(s && sym);
	_sl = strlen(sym);
	mb_assert(_sl > 0);

	context = s->parsing_context;
	running = s->running_context;

	/* int_t */
	tmp.integer = (int_t)mb_strtol(sym, &conv_suc, 0);
	if(*conv_suc == '\0') {
		memcpy(*value, tmp.any, sizeof(_raw_t));

		result = _DT_INT;

		goto _exit;
	}
	/* real_t */
	tmp.float_point = (real_t)mb_strtod(sym, &conv_suc);
	if(*conv_suc == '\0') {
		memcpy(*value, tmp.any, sizeof(_raw_t));

		result = _DT_REAL;

		goto _exit;
	}
	/* String */
	if(sym[0] == '"' && sym[_sl - 1] == '"' && _sl >= 2) {
		result = _DT_STRING;

		if(context->last_symbol && _IS_FUNC(context->last_symbol, _core_import)) {
			/* IMPORT statement */
			int n = context->current_symbol_nonius;
			char current_symbol[_SINGLE_SYMBOL_MAX_LENGTH + 1];
			char* buf = 0;
			memcpy(current_symbol, context->current_symbol, sizeof(current_symbol));
			memset(context->current_symbol, 0, sizeof(current_symbol));
			context->current_symbol_nonius = 0;
			context->last_symbol = 0;
			sym[_sl - 1] = '\0';
			context->parsing_state = _PS_NORMAL;
			if(_is_using_char(*(sym + 1))) {
#ifdef MB_ENABLE_MODULE
				char* ns = mb_strdup(sym + 2, strlen(sym + 2) + 1);
				if(_ls_find(s->using_modules, ns, (_ls_compare)_ht_cmp_string)) {
					safe_free(ns);
				} else {
					_ls_pushback(s->using_modules, ns);
				}

				goto _end_import;
#else /* MB_ENABLE_MODULE */
				_handle_error_now(s, SE_RN_NOT_SUPPORTED, 0, MB_FUNC_ERR);

				goto _end_import;
#endif /* MB_ENABLE_MODULE */
			}
			buf = _load_file(s, sym + 1, ":");
			if(buf) {
				if(buf != sym + 1) {
					mb_load_string(s, buf);
					safe_free(buf);
				}
			} else {
				if(!s->import_handler || s->import_handler(s, sym + 1) != MB_FUNC_OK) {
					_handle_error_now(s, SE_PS_FILE_OPEN_FAILED, 0, MB_FUNC_ERR);
				}
			}

_end_import:
			context->parsing_state = _PS_STRING;
			sym[_sl - 1] = '\"';
			context->current_symbol_nonius = n;
			memcpy(context->current_symbol, current_symbol, sizeof(current_symbol));
			result = _DT_NIL;
		}

		goto _exit;
	}
	/* Nil */
	if(!strcmp(sym, "NIL")) {
		tmp.integer = ~0;
		memcpy(*value, tmp.any, sizeof(_raw_t));

		result = _DT_NIL;

		goto _exit;
	}
	/* _array_t */
	glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, 0, 0);
	if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_ARRAY) {
		tmp.obj = (_object_t*)glbsyminscope->data;
		memcpy(*value, &(tmp.obj->data.array->type), sizeof(tmp.obj->data.array->type));

		result = _DT_ARRAY;

		goto _exit;
	}
	if(context->last_symbol && _IS_FUNC(context->last_symbol, _core_dim)) {
#ifdef MB_SIMPLE_ARRAY
		en = (sym[_sl - 1] == '$' ? _DT_STRING : _DT_REAL);
#else /* MB_SIMPLE_ARRAY */
		en = _DT_REAL;
#endif /* MB_SIMPLE_ARRAY */
		memcpy(*value, &en, sizeof(en));

		result = _DT_ARRAY;

		goto _exit;
	}
	/* _class_t */
#ifdef MB_ENABLE_CLASS
	if(context->last_symbol) {
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_CLASS) {
			if(_IS_FUNC(context->last_symbol, _core_class))
				_begin_class(s);
			result = _DT_CLASS;

			goto _exit;
		}
		if(_IS_FUNC(context->last_symbol, _core_class)) {
			if(s->last_instance) {
				_handle_error_now(s, SE_RN_DUPLICATE_CLASS, 0, MB_FUNC_ERR);

				goto _exit;
			}
			_begin_class(s);
			if(!_is_identifier_char(sym[0])) {
				result = _DT_NIL;

				goto _exit;
			}
			if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_VAR) {
				_handle_error_now(s, SE_RN_INVALID_CLASS, 0, MB_FUNC_ERR);

				goto _exit;
			}

			if(context->routine_state > 1) {
				_handle_error_now(s, SE_RN_INVALID_CLASS, 0, MB_FUNC_ERR);

				goto _exit;
			}

			result = _DT_CLASS;

			goto _exit;
		} else if(_IS_FUNC(context->last_symbol, _core_endclass)) {
			if(_end_class(s))
				_pop_scope(s);
		}
	}
#endif /* MB_ENABLE_CLASS */
	/* _routine_t */
	if(context->last_symbol && !_is_bracket(sym[0])) {
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_ROUTINE) {
			if(_IS_FUNC(context->last_symbol, _core_def))
				_begin_routine(s);
			result = _DT_ROUTINE;

			goto _exit;
		}
		if(_IS_FUNC(context->last_symbol, _core_def) || _IS_FUNC(context->last_symbol, _core_call)) {
			if(_IS_FUNC(context->last_symbol, _core_def))
				_begin_routine(s);
			if(!_is_identifier_char(sym[0])) {
				result = _DT_NIL;

				goto _exit;
			}
			if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_VAR) {
				_handle_error_now(s, SE_RN_INVALID_ROUTINE, 0, MB_FUNC_ERR);

				goto _exit;
			}

			if(_IS_FUNC(context->last_symbol, _core_def)) {
				if(context->routine_state > 1) {
					_handle_error_now(s, SE_RN_INVALID_ROUTINE, 0, MB_FUNC_ERR);

					goto _exit;
				}
			}

			result = _DT_ROUTINE;

			goto _exit;
		} else if(_IS_FUNC(context->last_symbol, _core_enddef)) {
			if(_end_routine(s))
				_pop_scope(s);
		}
	}
	/* _func_t */
	if(!context->last_symbol ||
		(context->last_symbol && ((context->last_symbol->type == _DT_FUNC && context->last_symbol->data.func->pointer != _core_close_bracket) ||
		context->last_symbol->type == _DT_SEP))) {
		if(strcmp("-", sym) == 0) {
			ptr = (intptr_t)_core_neg;
			memcpy(*value, &ptr, sizeof(intptr_t));

			result = _DT_FUNC;

			goto _exit;
		}
	}
	glbsyminscope = _find_func(s, sym, &mod);
	if(glbsyminscope) {
		if(context->last_symbol && context->last_symbol->type == _DT_ROUTINE) {
			if(_sl == 1 && sym[0] == '(')
				_begin_routine_parameter_list(s);
		}
		if(context->routine_params_state) {
			if(_sl == 1 && sym[0] == ')')
				_end_routine_parameter_list(s);
		}

#ifdef MB_ENABLE_MODULE
		if(mod) {
			_module_func_t* mp = (_module_func_t*)glbsyminscope->data;
			ptr = (intptr_t)mp->func;
			memcpy(*value, &ptr, sizeof(intptr_t));
		} else {
			ptr = (intptr_t)glbsyminscope->data;
			memcpy(*value, &ptr, sizeof(intptr_t));
		}
#else /* MB_ENABLE_MODULE */
		ptr = (intptr_t)glbsyminscope->data;
		memcpy(*value, &ptr, sizeof(intptr_t));
#endif /* MB_ENABLE_MODULE */

		result = _DT_FUNC;

		goto _exit;
	}
	/* MB_EOS */
	if(_sl == 1 && sym[0] == MB_EOS) {
		if(_IS_EOS(context->last_symbol))
			result = _DT_NIL;
		else
			result = _DT_EOS;

		goto _exit;
	}
	/* Separator */
	if(_sl == 1 && _is_separator(sym[0])) {
		result = _DT_SEP;

		goto _exit;
	}
	/* _var_t */
	glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, 0, 0);
	if(glbsyminscope) {
		if(((_object_t*)glbsyminscope->data)->type != _DT_LABEL) {
			memcpy(*value, &glbsyminscope->data, sizeof(glbsyminscope->data));

			result = _DT_VAR;

			goto _exit;
		}
	}
	/* _label_t */
	if(context->current_char == ':') {
		if(!context->last_symbol || _IS_EOS(context->last_symbol)) {
			glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, 0, 0);
			if(glbsyminscope) {
				memcpy(*value, &glbsyminscope->data, sizeof(glbsyminscope->data));
			}

			result = _DT_LABEL;

			goto _exit;
		}
	}
	if(context->last_symbol && (_IS_FUNC(context->last_symbol, _core_goto) || _IS_FUNC(context->last_symbol, _core_gosub))) {
		result = _DT_LABEL;

		goto _exit;
	}
	/* Otherwise */
	result = _DT_VAR;

_exit:
	return result;
}

int _parse_char(mb_interpreter_t* s, char c, int pos, unsigned short row, unsigned short col) {
	/* Parse a character */
	int result = MB_FUNC_OK;
	_parsing_context_t* context = 0;
	char last_char = '\0';

	mb_assert(s && s->parsing_context);

	context = s->parsing_context;

	last_char = context->current_char;
	context->current_char = c;

	if(context->parsing_state == _PS_NORMAL) {
		if(c >= 'a' && c <= 'z')
			c += 'A' - 'a';

		if(_is_blank(c)) { /* \t ' ' */
			_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
		} else if(_is_newline(c)) { /* \r \n EOF */
			_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
			_mb_check(result = _append_char_to_symbol(s, MB_EOS), _exit);
			_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
		} else if(_is_separator(c) || _is_bracket(c)) { /* , ; : ( ) */
			_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
			_mb_check(result = _append_char_to_symbol(s, c), _exit);
			_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
		} else if(_is_quotation_mark(c)) { /* " */
			_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
			_mb_check(result = _append_char_to_symbol(s, c), _exit);
			context->parsing_state = _PS_STRING;
		} else if(_is_comment(c)) { /* ' */
			_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
			_mb_check(result = _append_char_to_symbol(s, MB_EOS), _exit);
			_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
			context->parsing_state = _PS_COMMENT;
		} else {
			if(context->symbol_state == _SS_IDENTIFIER) {
				if(_is_identifier_char(c)) {
					_mb_check(result = _append_char_to_symbol(s, c), _exit);
				} else if(_is_operator_char(c)) {
					if(_is_exponent_prefix(context->current_symbol, 0, context->current_symbol_nonius - 2) && (last_char == 'e' || last_char == 'E') && c == '-') {
						_mb_check(result = _append_char_to_symbol(s, c), _exit);
					} else {
						context->symbol_state = _SS_OPERATOR;
						_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
						_mb_check(result = _append_char_to_symbol(s, c), _exit);
					}
				} else {
					_handle_error(s, SE_PS_INVALID_CHAR, 0, pos, row, col, MB_FUNC_ERR, _exit, result);
				}
			} else if(context->symbol_state == _SS_OPERATOR) {
				if(_is_identifier_char(c)) {
					context->symbol_state = _SS_IDENTIFIER;
					_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
					_mb_check(result = _append_char_to_symbol(s, c), _exit);
				} else if(_is_operator_char(c)) {
					if(c == '-')
						_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
					_mb_check(result = _append_char_to_symbol(s, c), _exit);
				} else {
					_handle_error(s, SE_PS_INVALID_CHAR, 0, pos, row, col, MB_FUNC_ERR, _exit, result);
				}
			} else {
				mb_assert(0 && "Impossible.");
			}
		}
	} else if(context->parsing_state == _PS_STRING) {
		if(_is_quotation_mark(c)) { /* " */
			_mb_check(result = _append_char_to_symbol(s, c), _exit);
			_mb_check(result = _cut_symbol(s, pos, row, col), _exit);
			context->parsing_state = _PS_NORMAL;
		} else {
			_mb_check(result = _append_char_to_symbol(s, c), _exit);
		}
	} else if(context->parsing_state == _PS_COMMENT) {
		if(_is_newline(c)) { /* \r \n EOF */
			context->parsing_state = _PS_NORMAL;
		} else {
			/* Do nothing */
		}
	} else {
		mb_assert(0 && "Unknown parsing state.");
	}

_exit:
	return result;
}

void _set_error_pos(mb_interpreter_t* s, int pos, unsigned short row, unsigned short col) {
	/* Set the position of an error */
	mb_assert(s);

	s->last_error_pos = pos;
	s->last_error_row = row;
	s->last_error_col = col;
}

int_t _get_size_of(_data_e type) {
	/* Get the size of a data type */
	int_t result = 0;

#ifdef MB_SIMPLE_ARRAY
	if(type == _DT_INT) {
		result = sizeof(int_t);
	} else if(type == _DT_REAL) {
		result = sizeof(real_t);
	} else if(type == _DT_STRING) {
		result = sizeof(char*);
	} else {
		mb_assert(0 && "Unsupported.");
	}
#else /* MB_SIMPLE_ARRAY */
	mb_unrefvar(type);
	result = sizeof(_raw_u);
#endif /* MB_SIMPLE_ARRAY */

	return result;
}

bool_t _try_get_value(_object_t* obj, mb_value_u* val, _data_e expected) {
	/* Try to get a value (typed as int_t, real_t or char*) */
	bool_t result = false;

	mb_assert(obj && val);

	if(obj->type == _DT_INT && (expected == _DT_UNKNOWN || expected == _DT_INT)) {
		val->integer = obj->data.integer;
		result = true;
	} else if(obj->type == _DT_REAL && (expected == _DT_UNKNOWN || expected == _DT_REAL)) {
		val->float_point = obj->data.float_point;
		result = true;
	} else if(obj->type == _DT_VAR) {
		result = _try_get_value(obj->data.variable->data, val, expected);
	}

	return result;
}

bool_t _is_number(void* obj) {
	/* Determine if an object is a number */
	bool_t result = false;
	_object_t* o = 0;

	mb_assert(obj);

	o = (_object_t*)obj;
	if(o->type == _DT_INT || o->type == _DT_REAL)
		result = true;
	else if(o->type == _DT_VAR)
		result = o->data.variable->data->type == _DT_INT || o->data.variable->data->type == _DT_REAL;

	return result;
}

bool_t _is_string(void* obj) {
	/* Determine if an object is a string value or a string variable */
	bool_t result = false;
	_object_t* o = 0;

	mb_assert(obj);

	o = (_object_t*)obj;
	if(o->type == _DT_STRING)
		result = true;
	else if(o->type == _DT_VAR)
		result = o->data.variable->data->type == _DT_STRING;

	return result;
}

char* _extract_string(_object_t* obj) {
	/* Extract a string from an object */
	char* result = 0;

	mb_assert(obj);

	if(obj->type == _DT_STRING)
		result = obj->data.string;
	else if(obj->type == _DT_VAR && obj->data.variable->data->type == _DT_STRING)
		result = obj->data.variable->data->data.string;

	if(!result)
		result = MB_NULL_STRING;

	return result;
}

bool_t _lock_ref_object(_lock_t* lk, _ref_t* ref, void* obj) {
	/* Lock an object */
	mb_assert(lk);

	_ref(ref, obj);
	if(*lk >= 0)
		++(*lk);
	else
		--(*lk);

	return true;
}

bool_t _unlock_ref_object(_lock_t* lk, _ref_t* ref, void* obj) {
	/* Unlock an object */
	bool_t result = true;

	mb_assert(lk);

	if(*lk > 0)
		--(*lk);
	else if(*lk < 0)
		++(*lk);
	else
		result = false;
	_unref(ref, obj);

	return result;
}

bool_t _write_on_ref_object(_lock_t* lk, _ref_t* ref, void* obj) {
	/* Write operation on a collection */
	bool_t result = true;
	mb_unrefvar(ref);
	mb_unrefvar(obj);

	mb_assert(lk);

	if(*lk > 0)
		*lk = -(*lk);
	else
		result = false;

	return result;
}

unsigned _ref(_ref_t* ref, void* data) {
	/* Add a referenct to a stub */
	mb_unrefvar(data);

	return ++(*(ref->count));
}

unsigned _unref(_ref_t* ref, void* data) {
	/* Remove a reference to a stub */
	unsigned result = 0;

	result = --(*(ref->count));
#ifdef MB_ENABLE_GC
	_gc_add(ref, data, 0);
	if(ref->count && !(*ref->count))
		_tidy_intermediate_value(ref, data);
	ref->on_unref(ref, data);
	if(!ref->count)
		_gc_remove(ref, data);
#else /* MB_ENABLE_GC */
	ref->on_unref(ref, data);
#endif /* MB_ENABLE_GC */

	return result;
}

void _create_ref(_ref_t* ref, _unref_func_t dtor, _data_e t, mb_interpreter_t* s) {
	/* Create a reference stub */
	if(ref->count)
		return;

	ref->count = (unsigned*)mb_malloc(sizeof(unsigned));
	*(ref->count) = 0;
	ref->on_unref = dtor;
	ref->type = t;
	ref->s = s;
}

void _destroy_ref(_ref_t* ref) {
	/* Destroy a reference stub */
	if(!ref->count)
		return;

	safe_free(ref->count);
	ref->on_unref = 0;
}

#ifdef MB_ENABLE_GC
void _gc_add(_ref_t* ref, void* data, _gc_t* gc) {
	/* Add a referenced object to GC */
	_ht_node_t* table = 0;

	mb_assert(ref && data);

	if(gc && _ht_find(gc->collected_table, ref))
		return;

	if(!ref->s->gc.table)
		return;

	if(ref->s->gc.collecting)
		table = ref->s->gc.recursive_table;
	else
		table = ref->s->gc.table;

	if(ref->count && *ref->count)
		_ht_set_or_insert(table, ref, data);
	else
		_ht_remove(table, ref, 0);
}

void _gc_remove(_ref_t* ref, void* data) {
	/* Remove a referenced object from GC */
	mb_unrefvar(data);

	mb_assert(ref && data);

	if(ref->s->gc.table)
		_ht_remove(ref->s->gc.table, ref, 0);
}

int _gc_add_reachable(void* data, void* extra, void* ht) {
	/* Get reachable objects */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	_var_t* var = 0;
	_ht_node_t* htable = (_ht_node_t*)ht;

	mb_assert(data && ht);

	obj = (_object_t*)data;
	if(_is_internal_object(obj))
		goto _exit;
	switch(obj->type) {
	case _DT_VAR:
		var = (_var_t*)obj->data.variable;
		_gc_add_reachable(var->data, extra, htable);

		break;
	case _DT_USERTYPE_REF:
		if(!_ht_find(htable, &obj->data.usertype_ref->ref))
			_ht_set_or_insert(htable, &obj->data.usertype_ref->ref, obj->data.usertype_ref);

		break;
	case _DT_ARRAY:
		if(!_ht_find(htable, &obj->data.array->ref))
			_ht_set_or_insert(htable, &obj->data.array->ref, obj->data.array);

		break;
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		if(!_ht_find(htable, &obj->data.list->ref)) {
			_ht_set_or_insert(htable, &obj->data.list->ref, obj->data.list);
			_LS_FOREACH(obj->data.list->list, _do_nothing_on_object, _gc_add_reachable, htable);
		}

		break;
	case _DT_DICT:
		if(!_ht_find(htable, &obj->data.dict->ref)) {
			_ht_set_or_insert(htable, &obj->data.dict->ref, obj->data.dict);
			_HT_FOREACH(obj->data.dict->dict, _do_nothing_on_object, _gc_add_reachable, htable);
		}

		break;
	case _DT_LIST_IT:
		if(!_ht_find(htable, &obj->data.list_it->list->ref)) {
			_ht_set_or_insert(htable, &obj->data.list_it->list->ref, obj->data.list_it->list);
			_LS_FOREACH(obj->data.list_it->list->list, _do_nothing_on_object, _gc_add_reachable, htable);
		}

		break;
	case _DT_DICT_IT:
		if(!_ht_find(htable, &obj->data.dict_it->dict->ref)) {
			_ht_set_or_insert(htable, &obj->data.dict_it->dict->ref, obj->data.dict_it->dict);
			_HT_FOREACH(obj->data.dict_it->dict->dict, _do_nothing_on_object, _gc_add_reachable, htable);
		}

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		if(!_ht_find(htable, &obj->data.instance->ref)) {
			_ht_set_or_insert(htable, &obj->data.instance->ref, obj->data.instance);
			_traverse_class(obj->data.instance, _gc_add_reachable, _add_class_meta_reachable, _META_LIST_MAX_DEPTH, false, htable, 0);
		}

		break;
#endif /* MB_ENABLE_CLASS */
	default: /* Do nothing */
		break;
	}

_exit:
	return result;
}

void _gc_get_reachable(mb_interpreter_t* s, _ht_node_t* ht) {
	/* Get all reachable referenced objects */
	_running_context_t* running = 0;
	_ht_node_t* global_scope = 0;

	mb_assert(s && ht);

	running = s->running_context;
	while(running) {
		global_scope = running->var_dict;
		if(global_scope) {
			_HT_FOREACH(global_scope, _do_nothing_on_object, _gc_add_reachable, ht);
		}

		running = running->prev;
	}
}

int _gc_destroy_garbage_in_list(void* data, void* extra, void* gc) {
	/* Destroy only the capsule (wrapper) of an object, leave the data behind, and add it to GC if possible */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	_gc_t* _gc = (_gc_t*)gc;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	_ADDGC(obj, _gc);
	safe_free(obj);

	result = _OP_RESULT_DEL_NODE;

	return result;
}

int _gc_destroy_garbage_in_dict(void* data, void* extra, void* gc) {
	/* Destroy only the capsule (wrapper) of an object, leave the data behind, deal with extra as well, and add it to GC if possible */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	_gc_t* _gc = (_gc_t*)gc;

	mb_assert(data);

	obj = (_object_t*)data;
	_ADDGC(obj, _gc);
	safe_free(obj);

	obj = (_object_t*)extra;
	_ADDGC(obj, _gc);
	safe_free(obj);

	result = _OP_RESULT_DEL_NODE;

	return result;
}

#ifdef MB_ENABLE_CLASS
int _gc_destroy_garbage_in_class(void* data, void* extra, void* gc) {
	/* Destroy only the capsule (wrapper) of an object, leave the data behind, deal with extra as well, and add it to GC if possible */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	_gc_t* _gc = (_gc_t*)gc;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	if(obj->type == _DT_VAR) {
		_gc_destroy_garbage_in_class(obj->data.variable->data, 0, gc);
		safe_free(obj->data.variable->name);
		safe_free(obj->data.variable);
	} else {
		_ADDGC(obj, _gc);
	}
	safe_free(obj);

	result = _OP_RESULT_DEL_NODE;

	return result;
}
#endif /* MB_ENABLE_CLASS */

int _gc_destroy_garbage(void* data, void* extra) {
	/* Destroy a garbage */
	int result = _OP_RESULT_NORMAL;
	_gc_t* gc = 0;
	_ref_t* ref = 0;
	bool_t cld = false;
#ifdef MB_ENABLE_COLLECTION_LIB
	_list_t* lst = 0;
	_dict_t* dct = 0;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	_class_t* instance = 0;
#endif /* MB_ENABLE_CLASS */

	mb_assert(data && extra);

	ref = (_ref_t*)extra;
	gc = &ref->s->gc;
	switch(ref->type) {
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		lst = (_list_t*)data;
		if(gc->collecting <= 1 && !_ht_find(gc->recursive_table, ref)) {
			_LS_FOREACH(lst->list, _do_nothing_on_object, _gc_destroy_garbage_in_list, gc);
		} else {
			_ls_foreach(lst->list, _destroy_object_capsule_only);
		}
		_ls_clear(lst->list);
		lst->count = 0;

		break;
	case _DT_DICT:
		dct = (_dict_t*)data;
		if(gc->collecting <= 1 && !_ht_find(gc->recursive_table, ref)) {
			_HT_FOREACH(dct->dict, _do_nothing_on_object, _gc_destroy_garbage_in_dict, gc);
		} else {
			_ht_foreach(dct->dict, _destroy_object_capsule_only_with_extra);
		}
		_ht_clear(dct->dict);

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		instance = (_class_t*)data;
		_HT_FOREACH(instance->scope->var_dict, _do_nothing_on_object, _gc_destroy_garbage_in_class, gc);
		_ht_clear(instance->scope->var_dict);
		_ls_clear(instance->meta_list);

		break;
#endif /* MB_ENABLE_CLASS */
	default: /* Do nothing */
		break;
	}
	if(ref->count) {
		cld = (*(ref->count)) == 1;
		_unref(ref, data);
		if(cld)
			_ht_set_or_insert(gc->collected_table, ref, data);
	}

	result = _OP_RESULT_DEL_NODE;

	return result;
}

void _gc_swap_tables(mb_interpreter_t* s) {
	/* Swap garbage tables */
	_ht_node_t* tmp = 0;

	mb_assert(s);

	tmp = s->gc.table;
	s->gc.table = s->gc.recursive_table;
	s->gc.recursive_table = tmp;
}

void _gc_try_trigger(mb_interpreter_t* s) {
	/* Try trigger garbage collection */
	mb_assert(s);

	if(s->gc.table->count >= MB_GC_GARBAGE_THRESHOLD)
		_gc_collect_garbage(s, 1);
}

void _gc_collect_garbage(mb_interpreter_t* s, int depth) {
	/* Collect all garbage */
	_ht_node_t* valid = 0;

	mb_assert(s);

	/* Avoid infinity loop */
	if(s->gc.collecting) return;
	s->gc.collecting++;
	/* Get reachable information */
	valid = _ht_create(0, _ht_cmp_ref, _ht_hash_ref, _do_nothing_on_object);
	_gc_get_reachable(s, valid);
	/* Get unreachable information */
	_HT_FOREACH(valid, _do_nothing_on_object, _ht_remove_exist, s->gc.table);
	/* Collect garbage */
	do {
		_ht_foreach(s->gc.table, _gc_destroy_garbage);
		_ht_clear(s->gc.table);
		if(s->gc.collecting > 1)
			s->gc.collecting--;

		if(!depth || !s->gc.recursive_table->count)
			break;

		_gc_swap_tables(s);
		s->gc.collecting++;
	} while(1);
	/* Tidy */
	_ht_clear(s->gc.collected_table);
	_ht_clear(valid);
	_ht_destroy(valid);
	s->gc.collecting--;
	mb_assert(!s->gc.collecting);
}
#endif /* MB_ENABLE_GC */

_usertype_ref_t* _create_usertype_ref(mb_interpreter_t* s, void* val, mb_dtor_func_t un, mb_clone_func_t cl, mb_hash_func_t hs, mb_cmp_func_t cp, mb_fmt_func_t ft) {
	/* Create a referenced usertype */
	_usertype_ref_t* result = (_usertype_ref_t*)mb_malloc(sizeof(_usertype_ref_t));
	memset(result, 0, sizeof(_usertype_ref_t));
	result->usertype = val;
	result->dtor = un;
	result->clone = cl;
	result->hash = hs;
	result->cmp = cp;
	result->fmt = ft;
	_create_ref(&result->ref, _unref_usertype_ref, _DT_USERTYPE_REF, s);

	return result;
}

void _destroy_usertype_ref(_usertype_ref_t* c) {
	/* Destroy a referenced usertype */
	if(c->dtor)
		c->dtor(c->ref.s, c->usertype);
	_destroy_ref(&c->ref);
	safe_free(c);
}

void _unref_usertype_ref(_ref_t* ref, void* data) {
	/* Unreference a referenced usertype */
	if(!(*(ref->count)))
		_destroy_usertype_ref((_usertype_ref_t*)data);
}

_array_t* _create_array(const char* n, _data_e t, mb_interpreter_t* s) {
	/* Create an array */
	_array_t* result = (_array_t*)mb_malloc(sizeof(_array_t));
	memset(result, 0, sizeof(_array_t));
	result->type = t;
	result->name = (char*)n;
	_create_ref(&result->ref, _unref_array, _DT_ARRAY, s);
	_ref(&result->ref, result);

	return result;
}

void _destroy_array(_array_t* arr) {
	/* Destroy an array */
	mb_assert(arr);

	_clear_array(arr);
	if(arr->name) {
		safe_free(arr->name);
	}
#ifndef MB_SIMPLE_ARRAY
	if(arr->types) {
		safe_free(arr->types);
	}
#endif /* MB_SIMPLE_ARRAY */
	_destroy_ref(&arr->ref);
	safe_free(arr);
}

void _init_array(_array_t* arr) {
	/* Initialize an array */
	int elemsize = 0;

	mb_assert(arr);

#ifdef MB_SIMPLE_ARRAY
	elemsize = (int)_get_size_of(arr->type);
#else /* MB_SIMPLE_ARRAY */
	elemsize = (int)_get_size_of(_DT_UNKNOWN);
#endif /* MB_SIMPLE_ARRAY */
	mb_assert(arr->count > 0);
	mb_assert(!arr->raw);
	arr->raw = (void*)mb_malloc(elemsize * arr->count);
	if(arr->raw) {
		memset(arr->raw, 0, elemsize * arr->count);
	}
#ifndef MB_SIMPLE_ARRAY
	arr->types = (_data_e*)mb_malloc(sizeof(_data_e) * arr->count);
	if(arr->types) {
		unsigned int ul = 0;
		for(ul = 0; ul < arr->count; ++ul) {
			arr->types[ul] = _DT_INT;
		}
	}
#endif /* MB_SIMPLE_ARRAY */
}

int _get_array_pos(struct mb_interpreter_t* s, _array_t* arr, int* d, int c) {
	/* Calculate the index, used when interactive with host */
	int result = 0;
	int i = 0;
	int n = 0;

	mb_assert(s && arr && d);

	if(c < 0 || c > arr->dimension_count) {
		result = -1;

		goto _exit;
	}
	for(i = 0; i < c; i++) {
		n = d[i];
		if(n < 0 || n >= arr->dimensions[i]) {
			result = -1;

			goto _exit;
		}
		if(result)
			result *= n;
		else
			result += n;
	}

_exit:
	return result;
}

int _get_array_index(mb_interpreter_t* s, _ls_node_t** l, _object_t* c, unsigned int* index, bool_t* literally) {
	/* Calculate the index, used when walking through an AST */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* arr = 0;
	_object_t* len = 0;
	_object_t subscript;
	_object_t* subscript_ptr = 0;
	mb_value_u val;
	int dcount = 0;
	unsigned int idx = 0;

	mb_assert(s && l && index);

	subscript_ptr = &subscript;
	_MAKE_NIL(subscript_ptr);

	if(literally) *literally = false;

	/* Array name */
	ast = (_ls_node_t*)*l;
	if(!c && ast && _is_array(ast->data))
		c = (_object_t*)ast->data;
	if(!_is_array(c)) {
		_handle_error_on_obj(s, SE_RN_ARRAY_IDENTIFIER_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	if(((_object_t*)c)->type == _DT_ARRAY)
		arr = (_object_t*)c;
	else
		arr = ((_object_t*)c)->data.variable->data;
	/* = */
	if(literally && ast->next && _IS_FUNC((_object_t*)ast->next->data, _core_equal)) {
		*literally = true;

		goto _exit;
	}
	/* ( */
	if(!ast->next || ((_object_t*)ast->next->data)->type != _DT_FUNC || ((_object_t*)ast->next->data)->data.func->pointer != _core_open_bracket) {
		_handle_error_on_obj(s, SE_RN_OPEN_BRACKET_EXPECTED, 0,
			(ast && ast->next) ? ((_object_t*)ast->next->data) : 0,
			MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	/* Array subscript */
	if(!ast->next) {
		_handle_error_on_obj(s, SE_RN_ARRAY_SUBSCRIPT_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	while(((_object_t*)ast->data)->type != _DT_FUNC || ((_object_t*)ast->data)->data.func->pointer != _core_close_bracket) {
		/* Calculate an integer value */
		result = _calc_expression(s, &ast, &subscript_ptr);
		if(result != MB_FUNC_OK)
			goto _exit;
		len = subscript_ptr;
		if(!_try_get_value(len, &val, _DT_INT)) {
			_handle_error_on_obj(s, SE_RN_TYPE_NOT_MATCH, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(val.integer < 0) {
			_handle_error_on_obj(s, SE_RN_ILLEGAL_BOUND, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(dcount + 1 > arr->data.array->dimension_count) {
			_handle_error_on_obj(s, SE_RN_DIMENSION_OUT_OF_BOUND, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if((int)val.integer >= arr->data.array->dimensions[dcount]) {
			_handle_error_on_obj(s, SE_RN_ARRAY_OUT_OF_BOUND, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(idx)
			idx *= (unsigned int)val.integer;
		else
			idx += (unsigned int)val.integer;
		/* Comma? */
		if(_IS_SEP(ast->data, ','))
			ast = ast->next;

		++dcount;
	}
	*index = idx;
	if(!arr->data.array->raw) {
		_handle_error_on_obj(s, SE_RN_ARRAY_OUT_OF_BOUND, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}

_exit:
	*l = ast;

	return result;
}

bool_t _get_array_elem(mb_interpreter_t* s, _array_t* arr, unsigned int index, mb_value_u* val, _data_e* type) {
	/* Get the value of an element in an array */
	bool_t result = true;
	int_t elemsize = 0;
	unsigned int pos = 0;
	void* rawptr = 0;

	mb_assert(s && arr && val && type);

	mb_assert(index < arr->count);

	elemsize = _get_size_of(arr->type);
	pos = (unsigned int)(elemsize * index);
	rawptr = (void*)((intptr_t)arr->raw + pos);
	if(arr->type == _DT_REAL) {
#ifdef MB_SIMPLE_ARRAY
		val->float_point = *((real_t*)rawptr);
		*type = _DT_REAL;
#else /* MB_SIMPLE_ARRAY */
		_copy_bytes(val->bytes, *((mb_val_bytes_t*)rawptr));
		*type = arr->types[index];
#endif /* MB_SIMPLE_ARRAY */
	} else if(arr->type == _DT_STRING) {
		val->string = *((char**)rawptr);
		*type = _DT_STRING;
	} else {
		mb_assert(0 && "Unsupported.");
	}

	return result;
}

int _set_array_elem(mb_interpreter_t* s, _ls_node_t* ast, _array_t* arr, unsigned int index, mb_value_u* val, _data_e* type) {
	/* Set the value of an element in an array */
	int result = MB_FUNC_OK;
	int_t elemsize = 0;
	unsigned int pos = 0;
	void* rawptr = 0;
	mb_unrefvar(ast);

	mb_assert(s && arr && val);

	mb_assert(index < arr->count);

	elemsize = _get_size_of(arr->type);
	pos = (unsigned int)(elemsize * index);
	rawptr = (void*)((intptr_t)arr->raw + pos);
#ifdef MB_SIMPLE_ARRAY
	switch(*type) {
	case _DT_INT:
		*((real_t*)rawptr) = (real_t)val->integer;

		break;
	case _DT_STRING: {
			size_t _sl = 0;
			if(arr->type != _DT_STRING) {
				_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
			}
			_sl = strlen(val->string);
			*((char**)rawptr) = (char*)mb_malloc(_sl + 1);
			memcpy(*((char**)rawptr), val->string, _sl + 1);
		}

		break;
	default:
		_copy_bytes(*((mb_val_bytes_t*)rawptr), val->bytes);

		break;
	}
#else /* MB_SIMPLE_ARRAY */
	switch(*type) {
	case _DT_STRING: {
			size_t _sl = 0;
			_sl = strlen(val->string);
			*((char**)rawptr) = (char*)mb_malloc(_sl + 1);
			memcpy(*((char**)rawptr), val->string, _sl + 1);
			arr->types[index] = _DT_STRING;
		}

		break;
	default:
		_copy_bytes(*((mb_val_bytes_t*)rawptr), val->bytes);
		arr->types[index] = *type;

		break;
	}
#endif /* MB_SIMPLE_ARRAY */

	goto _exit; /* Avoid an unreferenced label warning */

_exit:
	return result;
}

void _clear_array(_array_t* arr) {
	/* Clear an array */
	char* str = 0;
	int_t elemsize = 0;
	unsigned int pos = 0;
	void* rawptr = 0;
	unsigned int ul = 0;

	mb_assert(arr);

	if(arr->raw) {
#ifndef MB_SIMPLE_ARRAY
		if(arr->type == _DT_REAL) {
			for(ul = 0; ul < arr->count; ++ul) {
				if(arr->types[ul] == _DT_STRING) {
					elemsize = _get_size_of(arr->type);
					pos = (unsigned int)(elemsize * ul);
					rawptr = (void*)((intptr_t)arr->raw + pos);
					str = *((char**)rawptr);
					if(str) {
						safe_free(str);
					}
				}
			}
		}
#endif /* MB_SIMPLE_ARRAY */
		if(arr->type == _DT_STRING) {
			for(ul = 0; ul < arr->count; ++ul) {
				elemsize = _get_size_of(arr->type);
				pos = (unsigned int)(elemsize * ul);
				rawptr = (void*)((intptr_t)arr->raw + pos);
				str = *((char**)rawptr);
				if(str) {
					safe_free(str);
				}
			}
		}
		safe_free(arr->raw);
	}
}

bool_t _is_array(void* obj) {
	/* Determine if an object is an array value or an array variable */
	bool_t result = false;
	_object_t* o = 0;

	o = (_object_t*)obj;
	if(o && o->type == _DT_ARRAY)
		result = true;
	else if(o && o->type == _DT_VAR)
		result = o->data.variable->data->type == _DT_ARRAY;

	return result;
}

void _unref_array(_ref_t* ref, void* data) {
	/* Unreference an array */
	if(!(*(ref->count)))
		_destroy_array((_array_t*)data);
}

#ifdef MB_ENABLE_COLLECTION_LIB
_list_t* _create_list(mb_interpreter_t* s) {
	/* Create a list */
	_list_t* result = (_list_t*)mb_malloc(sizeof(_list_t));
	memset(result, 0, sizeof(_list_t));
	result->list = _ls_create();
	_create_ref(&result->ref, _unref_list, _DT_LIST, s);

	return result;
}

void _destroy_list(_list_t* c) {
	/* Destroy a list */
	_ls_foreach(c->list, _destroy_object);
	_ls_destroy(c->list);
	_destroy_ref(&c->ref);
	safe_free(c);
}

_dict_t* _create_dict(mb_interpreter_t* s) {
	/* Create a dictionary */
	_dict_t* result = (_dict_t*)mb_malloc(sizeof(_dict_t));
	memset(result, 0, sizeof(_dict_t));
	result->dict = _ht_create(0, _ht_cmp_object, _ht_hash_object, _destroy_object_with_extra);
	_create_ref(&result->ref, _unref_dict, _DT_DICT, s);

	return result;
}

void _destroy_dict(_dict_t* c) {
	/* Destroy a dictionary */
	_ht_foreach(c->dict, _destroy_object_with_extra);
	_ht_destroy(c->dict);
	_destroy_ref(&c->ref);
	safe_free(c);
}

_list_it_t* _create_list_it(_list_t* coll, bool_t lock) {
	/* Create an iterator of a list */
	_list_it_t* result = 0;

	mb_assert(coll);

	result = (_list_it_t*)mb_malloc(sizeof(_list_it_t));
	memset(result, 0, sizeof(_list_it_t));
	result->list = coll;
	result->locking = lock;
	result->curr = coll->list;
	if(lock)
		_lock_ref_object(&coll->lock, &coll->ref, coll);

	return result;
}

bool_t _destroy_list_it(_list_it_t* it) {
	/* Destroy an iterator of a list */
	bool_t result = true;

	mb_assert(it);

	_unlock_ref_object(&it->list->lock, &it->list->ref, it->list);
	safe_free(it);

	return result;
}

_list_it_t* _move_list_it_next(_list_it_t* it) {
	/* Move an iterator of a list to next node */
	_list_it_t* result = 0;

	if(!it || !it->list || !it->list->list || !it->curr)
		goto _exit;

	if(it->list->lock)
		it->curr = it->curr->next;

	if(it->curr)
		result = it;

_exit:
	return result;
}

_dict_it_t* _create_dict_it(_dict_t* coll, bool_t lock) {
	/* Create an iterator of a dictionary */
	_dict_it_t* result = 0;

	mb_assert(coll);

	result = (_dict_it_t*)mb_malloc(sizeof(_dict_it_t));
	memset(result, 0, sizeof(_dict_it_t));
	result->dict = coll;
	result->locking = lock;
	result->curr_bucket = 0;
	result->curr_node = (_ls_node_t*)(intptr_t)~0;
	if(lock)
		_lock_ref_object(&coll->lock, &coll->ref, coll);

	return result;
}

bool_t _destroy_dict_it(_dict_it_t* it) {
	/* Destroy an iterator of a dictionary */
	bool_t result = true;

	mb_assert(it);

	_unlock_ref_object(&it->dict->lock, &it->dict->ref, it->dict);
	safe_free(it);

	return result;
}

_dict_it_t* _move_dict_it_next(_dict_it_t* it) {
	/* Move an iterator of a dictionary to next node */
	_dict_it_t* result = 0;

	if(!it || !it->dict || !it->dict->dict || !it->curr_node)
		goto _exit;

	if(!it->dict->lock)
		goto _exit;

	if(it->curr_node && it->curr_node != (_ls_node_t*)(intptr_t)~0) {
		it->curr_node = it->curr_node->next;
		if(!it->curr_node)
			++it->curr_bucket;
	}
	if(!it->curr_node || it->curr_node == (_ls_node_t*)(intptr_t)~0) {
		for( ; it->curr_bucket < it->dict->dict->array_size; ++it->curr_bucket) {
			it->curr_node = it->dict->dict->array[it->curr_bucket];
			if(it->curr_node && it->curr_node->next) {
				it->curr_node = it->curr_node->next;

				break;
			}
		}
	}

	if(it->curr_node && it->curr_node->extra)
		result = it;

_exit:
	return result;
}

void _unref_list(_ref_t* ref, void* data) {
	/* Unreference a list */
	if(!(*(ref->count)))
		_destroy_list((_list_t*)data);
}

void _unref_dict(_ref_t* ref, void* data) {
	/* Unreference a dictionary */
	if(!(*(ref->count)))
		_destroy_dict((_dict_t*)data);
}

void _push_list(_list_t* coll, mb_value_t* val, _object_t* oarg) {
	/* Push a value to a list */
	mb_assert(coll && (val || oarg));

	if(val && !oarg)
		_create_internal_object_from_public_value(val, &oarg);
	_ls_pushback(coll->list, oarg);
	coll->count++;

	_write_on_ref_object(&coll->lock, &coll->ref, coll);
	_invalidate_list_cache(coll);
}

bool_t _pop_list(_list_t* coll, mb_value_t* val, mb_interpreter_t* s) {
	/* Pop a value from a list */
	_object_t* oval = 0;

	mb_assert(coll && val && s);

	oval = (_object_t*)_ls_popback(coll->list);
	if(oval) {
		_internal_object_to_public_value(oval, val);
		_destroy_object_capsule_only(oval, 0);
		coll->count--;

		if(val->type == MB_DT_STRING)
			_mark_lazy_destroy_string(s, val->value.string);

		_write_on_ref_object(&coll->lock, &coll->ref, coll);
		_invalidate_list_cache(coll);

		return true;
	} else {
		mb_make_nil(*val);

		return false;
	}
}

bool_t _insert_list(_list_t* coll, int_t idx, mb_value_t* val, _object_t** oval) {
	/* Insert a value into a list */
	_object_t* oarg = 0;

	mb_assert(coll && val);

	_create_internal_object_from_public_value(val, &oarg);
	if(oval)
		*oval = oarg;

	if(_ls_insert_at(coll->list, (int)idx, oarg)) {
		coll->count++;
		_write_on_ref_object(&coll->lock, &coll->ref, coll);
		_invalidate_list_cache(coll);

		return true;
	}

	return false;
}

bool_t _set_list(_list_t* coll, int_t idx, mb_value_t* val, _object_t** oval) {
	/* Set an element in a list with a specific index with a given value */
	_ls_node_t* result = 0;
	_object_t* oarg = 0;

	mb_assert(coll && (val || (oval && *oval)));

	result = _node_at_list(coll, (int)idx);
	if(result) {
		if(result->data)
			_destroy_object(result->data, 0);
		if(val) {
			_create_internal_object_from_public_value(val, &oarg);
			if(oval)
				*oval = oarg;
		} else {
			oarg = *oval;
		}
		result->data = oarg;

		_write_on_ref_object(&coll->lock, &coll->ref, coll);
		_invalidate_list_cache(coll);
	}

	return !!(result && result->data);
}

bool_t _remove_at_list(_list_t* coll, int_t idx) {
	/* Remove an element in a list with a specific index */
	bool_t result = false;
	_ls_node_t* node = 0;

	mb_assert(coll);

	node = _node_at_list(coll, (int)idx);
	if(node) {
		if(node->data) {
			_ls_remove(coll->list, node, _destroy_object);
			coll->count--;

			_write_on_ref_object(&coll->lock, &coll->ref, coll);
			_invalidate_list_cache(coll);

			result = true;
		}
	}

	return result;
}

_ls_node_t* _node_at_list(_list_t* coll, int index) {
	/* Get a node at a specific index in a list */
	_ls_node_t* result = 0;
	_ls_node_t* tmp = 0;
	int idx = index;

	mb_assert(coll);

	if(index >= 0 && index < (int)coll->count) {
		if(coll->cached_node && !(index < coll->cached_index / 2)) {
			while(index != coll->cached_index) {
				if(index > coll->cached_index) {
					coll->cached_node = coll->cached_node->next;
					coll->cached_index++;
				} else if(index < coll->cached_index) {
					coll->cached_node = coll->cached_node->prev;
					coll->cached_node--;
				}
			}
			result = coll->cached_node;
		} else {
			tmp = coll->list->next;
			while(tmp && idx) {
				tmp = tmp->next;
				--idx;
			}
			if(tmp) {
				result = tmp;
				coll->cached_node = tmp;
				coll->cached_index = index;
			}
		}
	}

	return result;
}

bool_t _at_list(_list_t* coll, int_t idx, mb_value_t* oval) {
	/* Get the value in a list with a specific index */
	_ls_node_t* result = 0;

	mb_assert(coll && oval);

	result = _node_at_list(coll, idx);
	if(oval && result && result->data)
		_internal_object_to_public_value((_object_t*)result->data, oval);

	return !!(result && result->data);
}

bool_t _find_list(_list_t* coll, mb_value_t* val) {
	/* Find a value in a list */
	bool_t result = false;
	_object_t* oarg = 0;

	mb_assert(coll && val);

	_create_internal_object_from_public_value(val, &oarg);
	result = !!_ls_find(coll->list, oarg, (_ls_compare)_ht_cmp_object);
	_destroy_object(oarg, 0);

	return result;
}

void _clear_list(_list_t* coll) {
	/* Clear a list */
	mb_assert(coll);

	_ls_foreach(coll->list, _destroy_object);
	_ls_clear(coll->list);
	coll->count = 0;

	_write_on_ref_object(&coll->lock, &coll->ref, coll);
	_invalidate_list_cache(coll);
}

void _sort_list(_list_t* coll) {
	/* Sort a list */
	mb_assert(coll);

	_ls_sort(coll->list, (_ls_compare)_ht_cmp_object);

	_write_on_ref_object(&coll->lock, &coll->ref, coll);
	_invalidate_list_cache(coll);
}

void _invalidate_list_cache(_list_t* coll) {
	/* Invalidate cached list index */
	mb_assert(coll);

	coll->cached_node = 0;
	coll->cached_index = 0;
}

void _set_dict(_dict_t* coll, mb_value_t* key, mb_value_t* val, _object_t* okey, _object_t* oval) {
	/* Set an element to a dictionary with a key-value pair */
	_ls_node_t* exist = 0;
	void* data = 0;
	void* extra = 0;

	mb_assert(coll && (key || okey) && (val || oval));

	if(key && !okey)
		_create_internal_object_from_public_value(key, &okey);
	if(val && !oval)
		_create_internal_object_from_public_value(val, &oval);
	exist = _ht_find(coll->dict, okey);
	if(exist) {
		data = exist->data;
		extra = exist->extra;
		_ht_remove(coll->dict, okey, (_ls_compare)_ls_cmp_extra_object);
	}
	_ht_set_or_insert(coll->dict, okey, oval);

	_write_on_ref_object(&coll->lock, &coll->ref, coll);
}

bool_t _remove_dict(_dict_t* coll, mb_value_t* key) {
	/* Remove an element to a dictionary with a specific key */
	_ls_node_t* result = 0;
	_object_t* okey = 0;
	void* data = 0;
	void* extra = 0;

	mb_assert(coll && key);

	_create_internal_object_from_public_value(key, &okey);
	result = _ht_find(coll->dict, okey);
	if(result && result->data) {
		data = result->data;
		extra = result->extra;
		_ht_remove(coll->dict, okey, _ls_cmp_extra_object);
		_destroy_object(okey, 0);

		_write_on_ref_object(&coll->lock, &coll->ref, coll);
	}

	return !!(result && result->data);
}

bool_t _find_dict(_dict_t* coll, mb_value_t* val, mb_value_t* oval) {
	/* Find a key in a dictionary */
	_ls_node_t* result = 0;
	_object_t* oarg = 0;

	mb_assert(coll && val);

	_create_internal_object_from_public_value(val, &oarg);
	result = _ht_find(coll->dict, oarg);
	_destroy_object(oarg, 0);
	if(oval && result && result->data)
		_internal_object_to_public_value((_object_t*)result->data, oval);

	return !!(result && result->data);
}

void _clear_dict(_dict_t* coll) {
	/* Clear a dictionary */
	mb_assert(coll);

	_ht_foreach(coll->dict, _destroy_object_with_extra);
	_ht_clear(coll->dict);

	_write_on_ref_object(&coll->lock, &coll->ref, coll);
}

bool_t _invalid_list_it(_list_it_t* it) {
	/* Determin whether a list iterator is invalid */
	if(!it) return false;

	return it && it->list && it->list->lock <= 0;
}

bool_t _invalid_dict_it(_dict_it_t* it) {
	/* Determin whether a dictionary iterator is invalid */
	if(!it) return false;

	return it && it->dict && it->dict->lock <= 0;
}

bool_t _assign_with_it(_object_t* tgt, _object_t* src) {
	/* Assign an iterator to another object */
	mb_assert(tgt && src);

	if(src->type != _DT_LIST_IT && src->type != _DT_DICT_IT)
		return false;

	switch(src->type) {
	case _DT_LIST_IT:
		if(src->data.list_it->locking) {
			tgt->data.list_it = _create_list_it(src->data.list_it->list, true);
		} else {
			tgt->data = src->data;
			tgt->data.list_it->locking = true;
			_lock_ref_object(&tgt->data.list_it->list->lock, &tgt->data.list_it->list->ref, tgt->data.list_it->list);
		}

		break;
	case _DT_DICT_IT:
		if(src->data.dict_it->locking) {
			tgt->data.dict_it = _create_dict_it(src->data.dict_it->dict, true);
		} else {
			tgt->data = src->data;
			tgt->data.dict_it->locking = true;
			_lock_ref_object(&tgt->data.dict_it->dict->lock, &tgt->data.dict_it->dict->ref, tgt->data.dict_it->dict);
		}

		break;
	default:
		mb_assert(0 && "Impossible.");

		break;
	}

	return true;
}

int _clone_to_list(void* data, void* extra, _list_t* coll) {
	/* Clone an object to a list */
	_object_t* obj = 0;
	_object_t* tgt = 0;
	mb_unrefvar(extra);

	mb_assert(data && coll);

	tgt = _create_object();
	obj = (_object_t*)data;
	_clone_object(coll->ref.s, obj, tgt);
	_push_list(coll, 0, tgt);
	_REF(tgt)

	return 1;
}

int _clone_to_dict(void* data, void* extra, _dict_t* coll) {
	/* Clone a key-value pair to a dictionary */
	_object_t* kobj = 0;
	_object_t* ktgt = 0;
	_object_t* vobj = 0;
	_object_t* vtgt = 0;

	mb_assert(data && extra && coll);

	ktgt = _create_object();
	kobj = (_object_t*)extra;
	_clone_object(coll->ref.s, kobj, ktgt);

	vtgt = _create_object();
	vobj = (_object_t*)data;
	_clone_object(coll->ref.s, vobj, vtgt);

	_set_dict(coll, 0, 0, ktgt, vtgt);
	_REF(ktgt)
	_REF(vtgt)

	return 1;
}
#endif /* MB_ENABLE_COLLECTION_LIB */

#ifdef MB_ENABLE_CLASS
void _init_class(mb_interpreter_t* s, _class_t* instance, char* n) {
	/* Initialize a class */
	_running_context_t* running = 0;

	mb_assert(s && instance && n);

	running = s->running_context;

	memset(instance, 0, sizeof(_class_t));
	_create_ref(&instance->ref, _unref_class, _DT_CLASS, s);
	_ref(&instance->ref, instance);
	instance->name = n;
	instance->meta_list = _ls_create();
	instance->scope = _create_running_context();
	instance->scope->var_dict = _ht_create(0, _ht_cmp_string, _ht_hash_string, 0);
}

void _begin_class(mb_interpreter_t* s) {
	/* Begin parsing a class */
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	context->class_state = _CLASS_STATE_PROC;
}

bool_t _end_class(mb_interpreter_t* s) {
	/* End parsing a class */
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	if(context->routine_state) {
		_handle_error_now(s, SE_RN_INVALID_ROUTINE, 0, MB_FUNC_ERR);
	}
	if(context->class_state == _CLASS_STATE_NONE) {
		_handle_error_now(s, SE_RN_INVALID_CLASS, 0, MB_FUNC_ERR);

		return false;
	}
	context->class_state = _CLASS_STATE_NONE;
	s->last_instance = 0;

	return true;
}

void _unref_class(_ref_t* ref, void* data) {
	/* Unreference a class instance */
	if(!(*(ref->count)))
		_destroy_class((_class_t*)data);
}

void _destroy_class(_class_t* c) {
	/* Destroy a class instance */
	if(c->meta_list) {
		_unlink_meta_class(c->ref.s, c);
		_ls_destroy(c->meta_list);
	}
	if(c->scope->var_dict) {
		_ht_foreach(c->scope->var_dict, _destroy_object);
		_ht_destroy(c->scope->var_dict);
	}
	safe_free(c->scope);
	_destroy_ref(&c->ref);
	safe_free(c->name);
	safe_free(c);
}

bool_t _traverse_class(_class_t* c, _class_scope_walker scope_walker, _class_meta_walker meta_walker, unsigned meta_depth, bool_t meta_walk_on_self, void* extra_data, void* ret) {
	/* Traverse all fields of a class instance, and its meta class instances recursively as well */
	bool_t result = true;
	_ls_node_t* node = 0;
	_class_t* meta = 0;

	mb_assert(c);

	if(scope_walker) {
		_HT_FOREACH(c->scope->var_dict, _do_nothing_on_object, scope_walker, extra_data);
	}
	if(meta_walk_on_self) {
		if(meta_walker) {
			result = meta_walker(c, extra_data, ret);
			if(!result)
				goto _exit;
		}
	}
	node = c->meta_list ? c->meta_list->next : 0;
	while(node) {
		meta = (_class_t*)node->data;
		if(meta_walker && meta_depth) {
			result = meta_walker(meta, extra_data, ret);
			if(!result) break;
		}
		result = _traverse_class(
			meta,
			scope_walker,
			meta_walker, meta_depth ? meta_depth - 1 : 0,
			meta_walk_on_self,
			extra_data, ret
		);
		if(!result) break;
		node = node->next;
	}

_exit:
	return result;
}

bool_t _link_meta_class(mb_interpreter_t* s, _class_t* derived, _class_t* base) {
	/* Link a class instance to another's meta list */
	mb_assert(s && derived && base);

	if(_ls_find(derived->meta_list, base, _ls_cmp_data)) {
		_handle_error_now(s, SE_RN_WRONG_META_CLASS, 0, MB_FUNC_ERR);

		return false;
	}

	_ls_pushback(derived->meta_list, base);
	_ref(&base->ref, base);

	return true;
}

void _unlink_meta_class(mb_interpreter_t* s, _class_t* derived) {
	/* Unlink a class instance's all meta class instances */
	mb_assert(s && derived);

	_LS_FOREACH(derived->meta_list, _do_nothing_on_object, _unlink_meta_instance, derived);
	_ls_clear(derived->meta_list);
}

int _unlink_meta_instance(void* data, void* extra, _class_t* derived) {
	/* Unlink a meta class instance */
	_class_t* base = 0;
	mb_unrefvar(extra);

	mb_assert(data && derived);

	base = (_class_t*)data;
	_unref(&base->ref, base);

	return 0;
}

int _clone_clsss_field(void* data, void* extra, void* n) {
	/* Clone fields of a class instance to another */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	_var_t* var = 0;
	_routine_t* sub = 0;
	_class_t* instance = (_class_t*)n;
	_object_t* ret = 0;
	mb_unrefvar(extra);

	mb_assert(data && n);

	obj = (_object_t*)data;
	if(_is_internal_object(obj))
		goto _exit;
	switch(obj->type) {
	case _DT_VAR:
		var = (_var_t*)obj->data.variable;
		if(!_search_identifier_in_scope_chain(instance->ref.s, instance->scope, var->name, 0, 0)) {
			ret = _duplicate_parameter(var, 0, instance->scope);
			_clone_object(instance->ref.s, obj, ret->data.variable->data);
		}

		break;
	case _DT_ROUTINE:
		sub = (_routine_t*)obj->data.routine;
		if(!_search_identifier_in_scope_chain(instance->ref.s, instance->scope, sub->name, 0, 0)) {
			_routine_t* routine = (_routine_t*)mb_malloc(sizeof(_routine_t));
			memset(routine, 0, sizeof(_routine_t));
			routine->name = mb_strdup(sub->name, 0);
			routine->instance = instance;
			routine->is_cloned = true;
			routine->type = sub->type;
			routine->func = sub->func;
			ret = _create_object();
			ret->type = _DT_ROUTINE;
			ret->data.routine = routine;
			ret->ref = false;

			_ht_set_or_insert(instance->scope->var_dict, obj->data.routine->name, ret);
		}

		break;
	default: /* Do nothing */
		break;
	}

_exit:
	return result;
}

bool_t _clone_class_meta_link(_class_t* meta, void* n, void* ret) {
	/* Link meta class to a new instance */
	_class_t* instance = (_class_t*)n;
	mb_unrefvar(ret);

	mb_assert(meta && n);

	_link_meta_class(instance->ref.s, instance, meta);

	return true;
}

bool_t _is_class(_class_t* instance, void* m, void* ret) {
	/* Detect whether a class instance is inherited from another */
	_class_t* meta = (_class_t*)m;
	bool_t* r = (bool_t*)ret;

	mb_assert(instance && meta && ret);

	*r = instance == meta;

	return !(*r);
}

bool_t _add_class_meta_reachable(_class_t* meta, void* ht, void* ret) {
	/* Add a meta class instance to a GC reachable table */
	_ht_node_t* htable = (_ht_node_t*)ht;
	mb_unrefvar(ret);

	mb_assert(meta && ht);

	if(!_ht_find(htable, &meta->ref))
		_ht_set_or_insert(htable, &meta->ref, meta);

	return true;
}
#endif /* MB_ENABLE_CLASS */

void _init_routine(mb_interpreter_t* s, _routine_t* routine, char* n, mb_routine_func_t f) {
	/* Initialize a routine */
	_running_context_t* running = 0;

	mb_assert(s && routine);

	running = s->running_context;

	memset(routine, 0, sizeof(_routine_t));
	routine->name = n;

	if(n && f)
		routine->type = _IT_NATIVE;
	else if(n && !f)
		routine->type = _IT_BASIC;
#ifdef MB_ENABLE_LAMBDA
	else if(!n && !f)
		routine->type = _IT_LAMBDA;
#endif /* MB_ENABLE_LAMBDA */

	switch(routine->type) {
	case _IT_BASIC:
		routine->func.basic.scope = _create_running_context();
		routine->func.basic.scope->var_dict = _ht_create(0, _ht_cmp_string, _ht_hash_string, 0);

		break;
#ifdef MB_ENABLE_LAMBDA
	case _IT_LAMBDA:
		_create_ref(&routine->func.lambda.ref, _unref_routine, _DT_ROUTINE, s);
		_ref(&routine->func.lambda.ref, routine);

		break;
#endif /* MB_ENABLE_LAMBDA */
	case _IT_NATIVE:
		routine->func.native.entry = f;

		break;
	}
}

void _begin_routine(mb_interpreter_t* s) {
	/* Begin parsing a routine */
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	context->routine_state++;
}

bool_t _end_routine(mb_interpreter_t* s) {
	/* End parsing a routine */
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	if(!context->routine_state) {
		_handle_error_now(s, SE_RN_INVALID_ROUTINE, 0, MB_FUNC_ERR);

		return false;
	}
	context->routine_state--;

	return true;
}

void _begin_routine_parameter_list(mb_interpreter_t* s) {
	/* Begin parsing the parameter list of a routine */
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	context->routine_params_state++;
}

void _end_routine_parameter_list(mb_interpreter_t* s) {
	/* End parsing the parameter list of a routine */
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	context->routine_params_state--;
}

_object_t* _duplicate_parameter(void* data, void* extra, _running_context_t* running) {
	/* Duplicate a parameter from a parameter list to variable dictionary */
	_var_t* ref = 0;
	_var_t* var = 0;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(running);

	if(data == 0)
		return 0;

	ref = (_var_t*)data;

	var = _create_var(&obj, ref->name, true);

	_ht_set_or_insert(running->var_dict, var->name, obj);

	return obj;
}

#ifdef MB_ENABLE_LAMBDA
void _unref_routine(_ref_t* ref, void* data) {
	/* Unreference a lambda routine */
	if(!(*(ref->count)))
		_destroy_routine((_routine_t*)data);
}

void _destroy_routine(_routine_t* r) {
	/* Destroy a lambda routine */
	if(r->name) {
		safe_free(r->name);
	}
	if(!r->is_cloned) {
		switch(r->type) {
		case _IT_BASIC:
			if(r->func.basic.scope) {
				if(r->func.basic.scope->var_dict) {
					_ht_foreach(r->func.basic.scope->var_dict, _destroy_object);
					_ht_destroy(r->func.basic.scope->var_dict);
				}
				safe_free(r->func.basic.scope);
			}
			if(r->func.basic.parameters)
				_ls_destroy(r->func.basic.parameters);

			break;
		case _IT_LAMBDA:
			_destroy_ref(&r->func.lambda.ref);
			if(r->func.lambda.parameters)
				_ls_destroy(r->func.lambda.parameters);
			if(r->func.lambda.upvalues)
				_ls_destroy(r->func.lambda.upvalues);

			break;
		case _IT_NATIVE: /* Do nothing */
			break;
		}
	}
	safe_free(r);
}
#endif /* MB_ENABLE_LAMBDA */

#ifdef MB_ENABLE_CLASS
_running_context_t* _reference_scope_by_class(mb_interpreter_t* s, _running_context_t* p, _class_t* c) {
	/* Create a scope reference to an exist one by a class */
	_running_context_t* result = 0;
	mb_unrefvar(c);

	mb_assert(s && p);

	if(p->meta == _SCOPE_META_REF)
		p = p->ref;

	result = _create_running_context();
	result->meta = _SCOPE_META_REF;
	result->ref = p;

	return result;
}

_running_context_t* _push_scope_by_class(mb_interpreter_t* s, _running_context_t* p) {
	/* Push a scope by a class */
	mb_assert(s);

	if(_find_scope(s, p))
		p = _reference_scope_by_class(s, p, 0);
	p->prev = s->running_context;
	s->running_context = p;

	return s->running_context;
}

_ls_node_t* _search_identifier_in_class(mb_interpreter_t* s, _class_t* instance, const char* n, _ht_node_t** ht) {
	/* Try to search an identifire from a class */
	_ls_node_t* result = 0;
	_ls_node_t* node = 0;
	_class_t* meta = 0;

	mb_assert(s && instance && n);

	result = _search_identifier_in_scope_chain(s, instance->scope, n, 0, ht);

	if(!result) {
		node = instance->meta_list ? instance->meta_list->next : 0;
		while(node) {
			meta = (_class_t*)node->data;
			result = _search_identifier_in_class(s, meta, n, ht);
			if(result) break;
			node = node->next;
		}
	}

	return result;
}

_ls_node_t* _search_identifier_accessor(mb_interpreter_t* s, _running_context_t* scope, const char* n, _ht_node_t** ht) {
	/* Try to search an identifier accessor in a scope */
	_ls_node_t* result = 0;
	_object_t* obj = 0;
	char acc[_SINGLE_SYMBOL_MAX_LENGTH];
	int i = 0;
	int j = 0;
	_class_t* instance = 0;

	mb_assert(s && n);

	while((i == 0) || (i > 0 && n[i - 1])) {
		acc[j] = n[i];
		if(_is_accessor(acc[j]) || acc[j] == '\0') {
			acc[j] = '\0';
			if(instance)
				result = _search_identifier_in_class(s, instance, acc, ht);
			else
				result = _search_identifier_in_scope_chain(s, scope, acc, 0, ht);
			if(!result) return 0;
			obj = (_object_t*)result->data;
			if(!obj) return 0;
			switch(obj->type) {
			case _DT_VAR:
				if(obj->data.variable->data->type == _DT_CLASS)
					instance = obj->data.variable->data->data.instance;

				break;
			case _DT_CLASS:
				instance = obj->data.instance;

				break;
			case _DT_ROUTINE:
				break;
			default:
				mb_assert(0 && "Unsupported.");

				return 0;
			}

			j = 0;
			i++;

			continue;
		}
		j++;
		i++;
	}

	return result;
}
#endif /* MB_ENABLE_CLASS */

_running_context_t* _reference_scope_by_routine(mb_interpreter_t* s, _running_context_t* p, _routine_t* r) {
	/* Create a scope reference to an exist one by a routine */
	_running_context_t* result = 0;

	mb_assert(s && p);

	if(p->meta == _SCOPE_META_REF)
		p = p->ref;

	result = _create_running_context();
	result->meta = _SCOPE_META_REF;
	result->ref = p;
	if(r && r->func.basic.parameters) {
		result->var_dict = _ht_create(0, _ht_cmp_string, _ht_hash_string, 0);
		_LS_FOREACH(r->func.basic.parameters, _do_nothing_on_object, _duplicate_parameter, result);
	}

	return result;
}

_running_context_t* _push_weak_scope_by_routine(mb_interpreter_t* s, _running_context_t* p, _routine_t* r) {
	/* Push a weak scope by a routine */
	mb_assert(s);

	if(_find_scope(s, p))
		p = _reference_scope_by_routine(s, p, r);
	if(p)
		p->prev = s->running_context;

	return p;
}

_running_context_t* _push_scope_by_routine(mb_interpreter_t* s, _running_context_t* p) {
	/* Push a scope by a routine */
	mb_assert(s);

	if(_find_scope(s, p))
		p = _reference_scope_by_routine(s, p, 0);
	if(p) {
		p->prev = s->running_context;
		s->running_context = p;
	}

	return s->running_context;
}

void _unreference_scope(mb_interpreter_t* s, _running_context_t* p) {
	/* Unreference and destroy a scope */
	mb_unrefvar(s);

	if(p->var_dict) {
		_ht_foreach(p->var_dict, _destroy_object);
		_ht_destroy(p->var_dict);
	}
	safe_free(p);
}

_running_context_t* _pop_weak_scope(mb_interpreter_t* s, _running_context_t* p) {
	/* Pop a weak scope */
	mb_assert(s);

	if(p)
		p->prev = 0;

	return p;
}

_running_context_t* _pop_scope(mb_interpreter_t* s) {
	/* Pop a scope */
	_running_context_t* running = 0;

	mb_assert(s);

	running = s->running_context;
	s->running_context = running->prev;
	running->prev = 0;
	if(running->meta == _SCOPE_META_REF)
		_unreference_scope(s, running);

	return s->running_context;
}

_running_context_t* _find_scope(mb_interpreter_t* s, _running_context_t* p) {
	/* Find a scope in a scope chain */
	_running_context_t* running = 0;

	mb_assert(s);

	running = s->running_context;
	while(running) {
		if(running == p)
			return running;

		if(running->ref == p)
			return running->ref;

		running = running->prev;
	}

	return running;
}

_running_context_t* _get_scope_to_add_routine(mb_interpreter_t* s) {
	/* Get a proper scope to add a routine */
	_parsing_context_t* context = 0;
	_running_context_t* running = 0;
	unsigned short class_state = _CLASS_STATE_NONE;

	mb_assert(s);

	context = s->parsing_context;
	running = s->running_context;
#ifdef MB_ENABLE_CLASS
	class_state = context->class_state;
#endif /* MB_ENABLE_CLASS */
	if(class_state != _CLASS_STATE_NONE) {
		if(running)
			running = running->prev;
	} else {
		while(running) {
			if(running->meta == _SCOPE_META_ROOT)
				break;

			running = running->prev;
		}
	}

	return running;
}

_ls_node_t* _search_identifier_in_scope_chain(mb_interpreter_t* s, _running_context_t* scope, const char* n, int pathing, _ht_node_t** ht) {
	/* Try to search an identifier in a scope chain */
	_ls_node_t* result = 0;
	_running_context_t* running = 0;
	_ht_node_t* fn = 0;

	mb_assert(s && n);

#ifdef MB_ENABLE_CLASS
	if(pathing) {
		result = _search_identifier_accessor(s, scope, n, ht);
		if(result)
			goto _exit;
	}

	if(s->last_routine && s->last_routine->instance) {
		_class_t* lastinst = s->last_routine->instance;
		s->last_routine->instance = 0;
		result = _search_identifier_in_class(s, lastinst, n, ht);
		s->last_routine->instance = lastinst;
		if(result)
			goto _exit;
	}
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(pathing);
#endif /* MB_ENABLE_CLASS */

	if(scope)
		running = scope;
	else
		running = s->running_context;
	while(running && !result) {
		if(running->var_dict) {
			result = _ht_find(running->var_dict, (void*)n);
			fn = running->var_dict;
			if(!result && running->meta == _SCOPE_META_REF) {
				result = _ht_find(running->ref->var_dict, (void*)n);
				fn = running->ref->var_dict;
			}
			if(result)
				break;
		}

		running = running->prev;
	}

#ifdef MB_ENABLE_CLASS
_exit:
#endif /* MB_ENABLE_CLASS */
	if(ht) *ht = fn;

	return result;
}

_array_t* _search_array_in_scope_chain(mb_interpreter_t* s, _array_t* i, _object_t** o) {
	/* Try to search an array in a scope chain */
	_object_t* obj = 0;
	_ls_node_t* scp = 0;
	_array_t* result = 0;

	mb_assert(s && i);

	result = i;
	scp = _search_identifier_in_scope_chain(s, 0, result->name, 0, 0);
	if(scp) {
		obj = (_object_t*)scp->data;
		if(obj && obj->type == _DT_ARRAY) {
			result = obj->data.array;
			if(o) *o = obj;
		}
	}

	return result;
}

_var_t* _search_var_in_scope_chain(mb_interpreter_t* s, _var_t* i) {
	/* Try to search a variable in a scope chain */
	_object_t* obj = 0;
	_ls_node_t* scp = 0;
	_var_t* result = 0;

	mb_assert(s && i);

	result = i;
	scp = _search_identifier_in_scope_chain(s, 0, result->name, 1, 0);
	if(scp) {
		obj = (_object_t*)scp->data;
		if(obj && obj->type == _DT_VAR)
			result = obj->data.variable;
	}

	return result;
}

_var_t* _create_var(_object_t** oobj, const char* n, bool_t dup_name) {
	/* Create a variable object */
	_object_t* obj = 0;
	_var_t* var = 0;

	var = (_var_t*)mb_malloc(sizeof(_var_t));
	memset(var, 0, sizeof(_var_t));
	if(dup_name)
		var->name = mb_strdup(n, strlen(n) + 1);
	else
		var->name = (char*)n;
	var->data = _create_object();

	if(!oobj || !(*oobj))
		obj = _create_object();
	else
		obj = *oobj;
	_MAKE_NIL(obj);
	obj->type = _DT_VAR;
	obj->data.variable = var;
	obj->ref = false;

	if(oobj) *oobj = obj;

	return var;
}

_object_t* _create_object(void) {
	/* Create an _object_t struct */
	_object_t* result = (_object_t*)mb_malloc(sizeof(_object_t));
	_MAKE_NIL(result);

	return result;
}

int _clone_object(mb_interpreter_t* s, _object_t* obj, _object_t* tgt) {
	/* Clone the data of an object */
	int result = 0;

	mb_assert(obj && tgt);

	_MAKE_NIL(tgt);
	tgt->type = _DT_NIL;
	if(_is_internal_object(obj))
		goto _exit;
	tgt->type = obj->type;
	switch(obj->type) {
	case _DT_VAR:
		_clone_object(s, obj->data.variable->data, tgt);

		break;
	case _DT_STRING:
		tgt->data.string = mb_strdup(obj->data.string, 0);

		break;
	case _DT_USERTYPE_REF:
		tgt->data.usertype_ref = _create_usertype_ref(
			obj->data.usertype_ref->ref.s,
			obj->data.usertype_ref->clone(obj->data.usertype_ref->ref.s, obj->data.usertype_ref->usertype),
			obj->data.usertype_ref->dtor, obj->data.usertype_ref->clone,
			obj->data.usertype_ref->hash, obj->data.usertype_ref->cmp, obj->data.usertype_ref->fmt
		);
		_ref(&tgt->data.usertype_ref->ref, tgt->data.usertype_ref);

		break;
	case _DT_FUNC:
		tgt->data.func->name = mb_strdup(obj->data.func->name, strlen(obj->data.func->name) + 1);
		tgt->data.func->pointer = obj->data.func->pointer;

		break;
	case _DT_ARRAY:
		tgt->data.array = obj->data.array;
		mb_assert(0 && "Not implemented.");

		break;
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		tgt->data.list = _create_list(obj->data.list->ref.s);
		_ref(&tgt->data.list->ref, tgt->data.list);
		_LS_FOREACH(obj->data.list->list, _do_nothing_on_object, _clone_to_list, tgt->data.list);

		break;
	case _DT_LIST_IT:
		tgt->data.list_it = _create_list_it(obj->data.list_it->list, false);

		break;
	case _DT_DICT:
		tgt->data.dict = _create_dict(obj->data.dict->ref.s);
		_ref(&tgt->data.dict->ref, tgt->data.dict);
		_HT_FOREACH(obj->data.dict->dict, _do_nothing_on_object, _clone_to_dict, tgt->data.dict);

		break;
	case _DT_DICT_IT:
		tgt->data.dict_it = _create_dict_it(obj->data.dict_it->dict, false);

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
	case _DT_LABEL:
		tgt->data.label->name = mb_strdup(obj->data.label->name, 0);
		tgt->data.label->node = obj->data.label->node;

		break;
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		tgt->data.instance = (_class_t*)mb_malloc(sizeof(_class_t));
		_init_class(s, tgt->data.instance, mb_strdup(obj->data.instance->name, 0));
		_push_scope_by_class(s, tgt->data.instance->scope);
		_traverse_class(obj->data.instance, _clone_clsss_field, _clone_class_meta_link, 1, false, tgt->data.instance, 0);
		_pop_scope(s);

		break;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		mb_assert(0 && "Not implemented.");

		break;
	case _DT_NIL: /* Fall through */
	case _DT_UNKNOWN: /* Fall through */
	case _DT_TYPE: /* Fall through */
	case _DT_INT: /* Fall through */
	case _DT_REAL: /* Fall through */
	case _DT_SEP: /* Fall through */
	case _DT_EOS: /* Fall through */
	case _DT_USERTYPE:
		tgt->data = obj->data;

		break;
	default:
		mb_assert(0 && "Invalid type.");

		break;
	}
	tgt->ref = false;
#ifdef MB_ENABLE_SOURCE_TRACE
	tgt->source_pos = 0;
	tgt->source_row = 0;
	tgt->source_col = 0;
#else /* MB_ENABLE_SOURCE_TRACE */
	tgt->source_pos = 0;
#endif /* MB_ENABLE_SOURCE_TRACE */
	++result;

_exit:
	return result;
}

int _dispose_object(_object_t* obj) {
	/* Dispose the data of an object */
	int result = 0;
	_var_t* var = 0;

	mb_assert(obj);

	if(_is_internal_object(obj))
		goto _exit;
	switch(obj->type) {
	case _DT_VAR:
		if(!obj->ref) {
			var = (_var_t*)obj->data.variable;
			safe_free(var->name);
			mb_assert(var->data->type != _DT_VAR);
			_destroy_object(var->data, 0);
			safe_free(var);
		}

		break;
	case _DT_STRING:
		if(!obj->ref) {
			if(obj->data.string) {
				safe_free(obj->data.string);
			}
		}

		break;
	case _DT_FUNC:
		safe_free(obj->data.func->name);
		safe_free(obj->data.func);

		break;
	_UNREF_USERTYPE_REF(obj)
	_UNREF_ARRAY(obj)
	_UNREF_COLL(obj)
	_UNREF_CLASS(obj)
	_UNREF_COLL_IT(obj)
	_UNREF_ROUTINE(obj)
	case _DT_LABEL:
		if(!obj->ref) {
			safe_free(obj->data.label->name);
			safe_free(obj->data.label);
		}

		break;
	case _DT_NIL: /* Fall through */
	case _DT_UNKNOWN: /* Fall through */
	case _DT_TYPE: /* Fall through */
	case _DT_INT: /* Fall through */
	case _DT_REAL: /* Fall through */
	case _DT_SEP: /* Fall through */
	case _DT_EOS: /* Fall through */
	case _DT_USERTYPE: /* Do nothing */
		break;
	default:
		mb_assert(0 && "Invalid type.");

		break;
	}
	obj->ref = false;
	obj->type = _DT_NIL;
	memset(&obj->data, 0, sizeof(obj->data));
#ifdef MB_ENABLE_SOURCE_TRACE
	obj->source_pos = 0;
	obj->source_row = 0;
	obj->source_col = 0;
#else /* MB_ENABLE_SOURCE_TRACE */
	obj->source_pos = 0;
#endif /* MB_ENABLE_SOURCE_TRACE */
	++result;

_exit:
	return result;
}

int _destroy_object(void* data, void* extra) {
	/* Destroy an object and its data */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	if(!_dispose_object(obj))
		goto _exit;
	safe_free(obj);

_exit:
	result = _OP_RESULT_DEL_NODE;

	return result;
}

int _destroy_object_with_extra(void* data, void* extra) {
	/* Destroy an object, including its data and extra data */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;

	mb_assert(data);

	obj = (_object_t*)data;
	if(!_dispose_object(obj))
		goto _exit;
	safe_free(obj);
	obj = (_object_t*)extra;
	if(!_dispose_object(obj))
		goto _exit;
	safe_free(obj);

_exit:
	result = _OP_RESULT_DEL_NODE;

	return result;
}

int _destroy_object_not_compile_time(void* data, void* extra) {
	/* Destroy an object which is not come from compile time */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	if(!obj->source_pos) {
		if(!_dispose_object(obj))
			goto _exit;
		safe_free(obj);
	}

_exit:
	result = _OP_RESULT_DEL_NODE;

	return result;
}

int _destroy_object_capsule_only(void* data, void* extra) {
	/* Destroy only the capsule (wrapper) of an object, leave the data behind */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	safe_free(obj);

	result = _OP_RESULT_DEL_NODE;

	return result;
}

int _destroy_object_capsule_only_with_extra(void* data, void* extra) {
	/* Destroy only the capsule (wrapper) of an object, leave the data behind, deal with extra as well */
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;

	mb_assert(data);

	obj = (_object_t*)data;
	safe_free(obj);

	obj = (_object_t*)extra;
	safe_free(obj);

	result = _OP_RESULT_DEL_NODE;

	return result;
}

int _do_nothing_on_object(void* data, void* extra) {
	/* Do nothing with an object, this is a helper function */
	int result = _OP_RESULT_NORMAL;
	mb_unrefvar(data);
	mb_unrefvar(extra);

	return result;
}

int _remove_source_object(void* data, void* extra) {
	/* Remove an object referenced to source code */
	int result = _OP_RESULT_DEL_NODE;
	mb_unrefvar(extra);

	mb_assert(data);

	return result;
}

int _destroy_memory(void* data, void* extra) {
	/* Destroy a chunk of memory */
	int result = _OP_RESULT_NORMAL;
	mb_unrefvar(extra);

	mb_assert(data);

	safe_free(data);

	return result;
}

int _compare_numbers(const _object_t* first, const _object_t* second) {
	/* Compare two numbers in two _object_t */
	int result = 0;

	mb_assert(first && second);
	mb_assert((first->type == _DT_INT || first->type == _DT_REAL) && (second->type == _DT_INT || second->type == _DT_REAL));

	if(first->type == _DT_INT && second->type == _DT_INT) {
		if(first->data.integer > second->data.integer)
			result = 1;
		else if(first->data.integer < second->data.integer)
			result = -1;
	} else if(first->type == _DT_REAL && second->type == _DT_REAL) {
		if(first->data.float_point > second->data.float_point)
			result = 1;
		else if(first->data.float_point < second->data.float_point)
			result = -1;
	} else {
		if((first->type == _DT_INT ? (real_t)first->data.integer : first->data.float_point) > (second->type == _DT_INT ? (real_t)second->data.integer : second->data.float_point))
			result = 1;
		else if((first->type == _DT_INT ? (real_t)first->data.integer : first->data.float_point) > (second->type == _DT_INT ? (real_t)second->data.integer : second->data.float_point))
			result = -1;
	}

	return result;
}

bool_t _is_internal_object(_object_t* obj) {
	/* Determine whether an object is internal*/
	bool_t result = false;

	mb_assert(obj);

	result = (_exp_assign == obj) ||
		(_OBJ_BOOL_TRUE == obj) || (_OBJ_BOOL_FALSE == obj);

	return result;
}

_data_e _public_type_to_internal_type(mb_data_e t) {
	/* Convert a public mb_data_e type to an internal _data_e */
	switch(t) {
	case MB_DT_TYPE:
		return _DT_TYPE;
	case MB_DT_NIL:
		return _DT_NIL;
	case MB_DT_INT:
		return _DT_INT;
	case MB_DT_REAL:
		return _DT_REAL;
	case MB_DT_STRING:
		return _DT_STRING;
	case MB_DT_USERTYPE:
		return _DT_USERTYPE;
	case MB_DT_USERTYPE_REF:
		return _DT_USERTYPE_REF;
	case MB_DT_ARRAY:
		return _DT_ARRAY;
#ifdef MB_ENABLE_COLLECTION_LIB
	case MB_DT_LIST:
		return _DT_LIST;
	case MB_DT_LIST_IT:
		return _DT_LIST_IT;
	case MB_DT_DICT:
		return _DT_DICT;
	case MB_DT_DICT_IT:
		return _DT_DICT_IT;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case MB_DT_CLASS:
		return _DT_CLASS;
#endif /* MB_ENABLE_CLASS */
	case MB_DT_ROUTINE:
		return _DT_ROUTINE;
	default:
		return _DT_UNKNOWN;
	}
}

mb_data_e _internal_type_to_public_type(_data_e t) {
	/* Convert an internal mb_data_e type to a public _data_e */
	switch(t) {
	case _DT_TYPE:
		return MB_DT_TYPE;
	case _DT_NIL:
		return MB_DT_NIL;
	case _DT_INT:
		return MB_DT_INT;
	case _DT_REAL:
		return MB_DT_REAL;
	case _DT_STRING:
		return MB_DT_STRING;
	case _DT_USERTYPE:
		return MB_DT_USERTYPE;
	case _DT_USERTYPE_REF:
		return MB_DT_USERTYPE_REF;
	case _DT_ARRAY:
		return MB_DT_ARRAY;
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		return MB_DT_LIST;
	case _DT_LIST_IT:
		return MB_DT_LIST_IT;
	case _DT_DICT:
		return MB_DT_DICT;
	case _DT_DICT_IT:
		return MB_DT_DICT_IT;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		return MB_DT_CLASS;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		return MB_DT_ROUTINE;
	default:
		return MB_DT_UNKNOWN;
	}
}

int _public_value_to_internal_object(mb_value_t* pbl, _object_t* itn) {
	/* Assign a public mb_value_t to an internal _object_t */
	int result = MB_FUNC_OK;

	mb_assert(pbl && itn);

	_UNREF(itn)

	switch(pbl->type) {
	case MB_DT_TYPE:
		itn->type = _DT_TYPE;
		itn->data.type = pbl->value.type;

		break;
	case MB_DT_NIL:
		itn->type = _DT_NIL;
		itn->data.integer = false;

		break;
	case MB_DT_INT:
		itn->type = _DT_INT;
		itn->data.integer = pbl->value.integer;

		break;
	case MB_DT_REAL:
		itn->type = _DT_REAL;
		itn->data.float_point = pbl->value.float_point;

		break;
	case MB_DT_STRING:
		itn->type = _DT_STRING;
		itn->data.string = pbl->value.string;

		break;
	case MB_DT_USERTYPE:
		itn->type = _DT_USERTYPE;
		memcpy(itn->data.raw, pbl->value.bytes, sizeof(mb_val_bytes_t));

		break;
	case MB_DT_USERTYPE_REF:
		itn->type = _DT_USERTYPE_REF;
		itn->data.usertype_ref = (_usertype_ref_t*)pbl->value.usertype_ref;

		break;
	case MB_DT_ARRAY:
		itn->type = _DT_ARRAY;
		itn->data.array = (_array_t*)pbl->value.array;
		itn->ref = false;

		break;
#ifdef MB_ENABLE_COLLECTION_LIB
	case MB_DT_LIST:
		itn->type = _DT_LIST;
		itn->data.list = (_list_t*)pbl->value.list;

		break;
	case MB_DT_LIST_IT:
		itn->type = _DT_LIST_IT;
		itn->data.list_it = (_list_it_t*)pbl->value.list_it;

		break;
	case MB_DT_DICT:
		itn->type = _DT_DICT;
		itn->data.dict = (_dict_t*)pbl->value.dict;

		break;
	case MB_DT_DICT_IT:
		itn->type = _DT_DICT_IT;
		itn->data.dict_it = (_dict_it_t*)pbl->value.dict_it;

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case MB_DT_CLASS:
		itn->type = _DT_CLASS;
		itn->data.instance = (_class_t*)pbl->value.instance;

		break;
#endif /* MB_ENABLE_CLASS */
	case MB_DT_ROUTINE:
		itn->type = _DT_ROUTINE;
		itn->data.routine = (_routine_t*)pbl->value.routine;

		break;
	default:
		result = MB_FUNC_ERR;

		break;
	}

	return result;
}

int _internal_object_to_public_value(_object_t* itn, mb_value_t* pbl) {
	/* Assign an internal _object_t to a public mb_value_t */
	int result = MB_FUNC_OK;

	mb_assert(pbl && itn);

	switch(itn->type) {
	case _DT_VAR:
		result = _internal_object_to_public_value(itn->data.variable->data, pbl);

		break;
	case _DT_TYPE:
		pbl->type = MB_DT_TYPE;
		pbl->value.type = itn->data.type;

		break;
	case _DT_NIL:
		mb_make_nil(*pbl);

		break;
	case _DT_INT:
		pbl->type = MB_DT_INT;
		pbl->value.integer = itn->data.integer;

		break;
	case _DT_REAL:
		pbl->type = MB_DT_REAL;
		pbl->value.float_point = itn->data.float_point;

		break;
	case _DT_STRING:
		pbl->type = MB_DT_STRING;
		pbl->value.string = itn->data.string;

		break;
	case _DT_USERTYPE:
		pbl->type = MB_DT_USERTYPE;
		memcpy(pbl->value.bytes, itn->data.raw, sizeof(mb_val_bytes_t));

		break;
	case _DT_USERTYPE_REF:
		pbl->type = MB_DT_USERTYPE_REF;
		pbl->value.usertype_ref = itn->data.usertype_ref;

		break;
	case _DT_ARRAY:
		pbl->type = MB_DT_ARRAY;
		pbl->value.array = itn->data.array;

		break;
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		pbl->type = MB_DT_LIST;
		pbl->value.list = itn->data.list;

		break;
	case _DT_LIST_IT:
		pbl->type = MB_DT_LIST_IT;
		pbl->value.list_it = itn->data.list;

		break;
	case _DT_DICT:
		pbl->type = MB_DT_DICT;
		pbl->value.dict = itn->data.dict;

		break;
	case _DT_DICT_IT:
		pbl->type = MB_DT_DICT_IT;
		pbl->value.dict_it = itn->data.dict_it;

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		pbl->type = MB_DT_CLASS;
		pbl->value.instance = itn->data.instance;

		break;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		pbl->type = MB_DT_ROUTINE;
		pbl->value.routine = itn->data.routine;

		break;
	default:
		result = MB_FUNC_ERR;

		break;
	}

	return result;
}

int _create_internal_object_from_public_value(mb_value_t* pbl, _object_t** itn) {
	/* Create an internal object from a public value */
	int result = MB_FUNC_OK;

	mb_assert(pbl && itn);

	*itn = _create_object();
	_public_value_to_internal_object(pbl, *itn);
	if((*itn)->type == _DT_STRING) {
		(*itn)->ref = false;
		(*itn)->data.string = mb_strdup((*itn)->data.string, 0);
	}

	return result;
}

int _compare_public_value_and_internal_object(mb_value_t* pbl, _object_t* itn) {
	/* Compare a public value and an internal object */
	int result = 0;
	mb_value_t tmp;

	mb_make_nil(tmp);
	_internal_object_to_public_value(itn, &tmp);
	if(pbl->type != tmp.type) {
		result = pbl->type - tmp.type;
	} else {
		result = _cmp_bytes(pbl->value.bytes, tmp.value.bytes);
	}

	return result;
}

void _try_clear_intermediate_value(void* data, void* extra, mb_interpreter_t* s) {
	/* Try clear the intermediate value when destroying an object */
	_object_t* obj = 0;
	_running_context_t* running = 0;
	mb_unrefvar(extra);

	mb_assert(s);

	if(data == 0)
		return;

	obj = (_object_t*)data;
	running = s->running_context;

	if(!_compare_public_value_and_internal_object(&running->intermediate_value, obj)) {
		mb_make_nil(running->intermediate_value);
	}
}

void _mark_lazy_destroy_string(mb_interpreter_t* s, char* ch) {
	/* Mark a string as lazy destroy */
	_object_t* temp_obj = 0;

	mb_assert(s && ch);

	temp_obj = _create_object();
	temp_obj->type = _DT_STRING;
	temp_obj->ref = false;
	temp_obj->data.string = ch;
	_ls_pushback(s->lazy_destroy_objects, temp_obj);
}

void _assign_public_value(mb_value_t* tgt, mb_value_t* src) {
	/* Assign a value with another */
	_object_t obj;
	mb_value_t nil;

	mb_assert(tgt);

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(tgt, &obj);
	_UNREF(&obj)

	mb_make_nil(nil);
	if(!src)
		src = &nil;
	memcpy(tgt, src, sizeof(mb_value_t));
	*src = nil;
}

void _swap_public_value(mb_value_t* tgt, mb_value_t* src) {
	/* Swap two public values */
	mb_value_t tmp;

	mb_assert(tgt && src);

	tmp = *tgt;
	*tgt = *src;
	*src = tmp;
}

int _clear_scope_chain(mb_interpreter_t* s) {
	/* Clear a scope chain */
	int result = 0;
	_running_context_t* running = 0;
	_running_context_t* prev = 0;

	mb_assert(s);

	running = s->running_context;
	while(running) {
		prev = running->prev;

		_ht_foreach(running->var_dict, _destroy_object);
		_ht_clear(running->var_dict);

		result++;
		running = prev;
	}

	return result;
}

int _dispose_scope_chain(mb_interpreter_t* s) {
	/* Dispose a scope chain */
	int result = 0;
	_running_context_t* running = 0;
	_running_context_t* prev = 0;

	mb_assert(s);

	running = s->running_context;
	while(running) {
		prev = running->prev;

		_ht_foreach(running->var_dict, _destroy_object);
		_ht_clear(running->var_dict);
		_ht_destroy(running->var_dict);
		running->var_dict = 0;
		mb_dispose_value(s, running->intermediate_value);
		safe_free(running);

		result++;
		running = prev;
	}
	s->running_context = 0;

	return result;
}

void _tidy_scope_chain(mb_interpreter_t* s) {
	/* Tidy the scope chain */
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	if(!context) return;

	while(context->routine_state && s->running_context->meta != _SCOPE_META_ROOT) {
		if(_end_routine(s))
			_pop_scope(s);
	}
#ifdef MB_ENABLE_CLASS
	while(context->class_state != _CLASS_STATE_NONE) {
		if(_end_class(s))
			_pop_scope(s);
	}
#endif /* MB_ENABLE_CLASS */
}

void _tidy_intermediate_value(_ref_t* ref, void* data) {
	/* Tidy the intermediate value */
	_object_t tmp;

	mb_assert(ref && data);

	if(!ref->s->running_context)
		return;

	_MAKE_NIL(&tmp);
	_public_value_to_internal_object(&ref->s->running_context->intermediate_value, &tmp);
	if(tmp.data.usertype == data) {
		switch(tmp.type) {
		case _DT_ARRAY: /* Fall through */
#ifdef MB_ENABLE_COLLECTION_LIB
		case _DT_LIST: /* Fall through */
		case _DT_DICT: /* Fall through */
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		case _DT_CLASS: /* Fall through */
#endif /* MB_ENABLE_CLASS */
		case _DT_USERTYPE_REF:
			mb_make_nil(ref->s->running_context->intermediate_value);

			break;
		default: /* Do nothing */
			break;
		}
	}
}

void _stepped(mb_interpreter_t* s, _ls_node_t* ast) {
	/* Called each step */
	_object_t* obj = 0;

	mb_assert(s);

	if(s->debug_stepped_handler) {
		if(ast && ast->data) {
			obj = (_object_t*)ast->data;
#ifdef MB_ENABLE_SOURCE_TRACE
			s->debug_stepped_handler(s, obj->source_pos, obj->source_row, obj->source_col);
#else /* MB_ENABLE_SOURCE_TRACE */
			s->debug_stepped_handler(s, obj->source_pos, 0, 0);
#endif /* MB_ENABLE_SOURCE_TRACE */
		} else {
			s->debug_stepped_handler(s, -1, 0, 0);
		}
	}
}

int _execute_statement(mb_interpreter_t* s, _ls_node_t** l) {
	/* Execute the ast, core execution function */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_running_context_t* running = 0;
	_ls_node_t* sub_stack = 0;
	bool_t skip_to_eoi = true;

	mb_assert(s && l);

	running = s->running_context;
	sub_stack = s->sub_stack;

	ast = *l;

	obj = (_object_t*)ast->data;

_retry:
	switch(obj->type) {
	case _DT_FUNC:
#ifdef MB_ENABLE_STACK_TRACE
		_ls_pushback(s->stack_frames, obj->data.func->name);
#endif /* MB_ENABLE_STACK_TRACE */
		result = (obj->data.func->pointer)(s, (void**)&ast);
#ifdef MB_ENABLE_STACK_TRACE
		_ls_popback(s->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */
		if(result == MB_FUNC_IGNORE) {
			result = MB_FUNC_OK;
			obj = (_object_t*)ast->data;

			goto _retry;
		}

		break;
	case _DT_VAR:
#ifdef MB_ENABLE_CLASS
		if(obj->data.variable->pathing) {
			/* Need to path */
			_ls_node_t* pathed = _search_identifier_in_scope_chain(s, 0, obj->data.variable->name, obj->data.variable->pathing, 0);
			if(pathed && pathed->data) {
				if(obj != (_object_t*)pathed->data) {
					/* Found another node */
					obj = (_object_t*)pathed->data;

					goto _retry;
				} else {
					/* Final node */
					result = _core_let(s, (void**)&ast);
				}
			} else {
				mb_assert(0 && "Impossible.");
			}
		} else if(obj->data.variable->data->type == _DT_ROUTINE) {
			obj = obj->data.variable->data;

			goto _retry;
		} else {
			/* Do not need to path */
			result = _core_let(s, (void**)&ast);
		}
#else /* MB_ENABLE_CLASS */
		result = _core_let(s, (void**)&ast);
#endif /* MB_ENABLE_CLASS */

		break;
	case _DT_ARRAY:
		result = _core_let(s, (void**)&ast);

		break;
	case _DT_INT: /* Fall through */
	case _DT_REAL: /* Fall through */
	case _DT_STRING:
		_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, 0, DON(ast), MB_FUNC_ERR, _exit, result);

		break;
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, 0, DON(ast), MB_FUNC_ERR, _exit, result);

		break;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		ast = ast->prev;
		result = _core_call(s, (void**)&ast);

		break;
	default: /* Do nothing */
		break;
	}

	if(s->schedule_suspend_tag) {
		if(s->schedule_suspend_tag == MB_FUNC_SUSPEND)
			mb_suspend(s, (void**)&ast);
		result = s->schedule_suspend_tag;
		s->schedule_suspend_tag = 0;
	}

	if(result != MB_FUNC_OK && result != MB_FUNC_SUSPEND && result != MB_SUB_RETURN)
		goto _exit;

	if(ast) {
		obj = (_object_t*)ast->data;
		if(_IS_EOS(obj)) {
			ast = ast->next;
		} else if(_IS_SEP(obj, ':')) {
			skip_to_eoi = false;
			ast = ast->next;
		} else if(obj && obj->type == _DT_VAR) {
			_handle_error_on_obj(s, SE_RN_COLON_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		} else if(_IS_FUNC(obj, _core_enddef) && result != MB_SUB_RETURN) {
			ast = (_ls_node_t*)_ls_popback(sub_stack);
		} else if(obj && obj->type == _DT_FUNC && (_is_operator(obj->data.func->pointer) || _is_flow(obj->data.func->pointer))) {
			ast = ast->next;
		} else if(obj && obj->type == _DT_FUNC) {
			/* Do nothing */
		} else if(obj && obj->type != _DT_FUNC) {
			ast = ast->next;
		} else {
			_handle_error_on_obj(s, SE_RN_COLON_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
	}

	if(skip_to_eoi && s->skip_to_eoi && s->skip_to_eoi == _ls_back(s->sub_stack)) {
		s->skip_to_eoi = 0;
		obj = (_object_t*)ast->data;
		if(obj->type != _DT_EOS) {
			result = _skip_to(s, &ast, 0, _DT_EOS);
			if(result != MB_FUNC_OK)
				goto _exit;
		}
	}

_exit:
	*l = ast;

	_stepped(s, ast);

	return result;
}

int _skip_to(mb_interpreter_t* s, _ls_node_t** l, mb_func_t f, _data_e t) {
	/* Skip current execution flow to a specific function */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* tmp = 0;
	_object_t* obj = 0;

	mb_assert(s && l);

	ast = *l;
	mb_assert(ast && ast->prev);
	do {
		if(!ast) {
			_handle_error_on_obj(s, SE_RN_SYNTAX, 0, DON(tmp), MB_FUNC_ERR, _exit, result);
		}
		tmp = ast;
		obj = (_object_t*)ast->data;
		*l = ast;
		ast = ast->next;
	} while(!(_IS_FUNC(obj, f)) && obj->type != t);

_exit:
	return result;
}

int _skip_if_chunk(mb_interpreter_t* s, _ls_node_t** l) {
	/* Skip current IF execution flow to next chunk */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* tmp = 0;
	_object_t* obj = 0;
	int nested = 0;
	unsigned mask = 0;

	mb_assert(s && l);

	ast = *l;
	mb_assert(ast && ast->prev);
	do {
		if(!ast) {
			_handle_error_on_obj(s, SE_RN_SYNTAX, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}
		tmp = ast;
		obj = (_object_t*)ast->data;
		*l = ast;
		ast = ast->next;
		if(ast && _IS_FUNC((_object_t*)ast->data, _core_if)) {
			if(++nested > sizeof(mask) * 8) {
				_handle_error_on_obj(s, SE_RN_NESTED_TOO_DEEP, 0, TON(l), MB_FUNC_ERR, _exit, result);
			}
		} else if(ast && nested && _IS_FUNC((_object_t*)ast->data, _core_then)) {
			if(!(ast && ast->next && _IS_EOS(ast->next->data)))
				mask |= 1 << (nested - 1);
		} else if(ast && nested &&
			(((mask & (1 << (nested - 1))) && _IS_EOS(ast->data)) ||
			(!(mask & (1 << (nested - 1))) && _IS_FUNC((_object_t*)ast->data, _core_endif)))
		) {
			if(--nested < 0) {
				_handle_error_on_obj(s, SE_RN_STRUCTURE_NOT_COMPLETED, 0, TON(l), MB_FUNC_ERR, _exit, result);
			}
			ast = ast->next;
		}
	} while(nested || (!_IS_FUNC(obj, _core_elseif) && !_IS_FUNC(obj, _core_else) && !_IS_FUNC(obj, _core_endif)));

_exit:
	return result;
}

int _skip_struct(mb_interpreter_t* s, _ls_node_t** l, mb_func_t open_func, mb_func_t close_func) {
	/* Skip current structure */
	int result = MB_FUNC_OK;
	int count = 0;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_object_t* obj_prev = 0;

	mb_assert(s && l && open_func && close_func);

	ast = (_ls_node_t*)*l;

	count = 1;
	do {
		if(!ast->next) {
			_handle_error_on_obj(s, SE_RN_STRUCTURE_NOT_COMPLETED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		obj_prev = (_object_t*)ast->data;
		ast = ast->next;
		obj = (_object_t*)ast->data;
		if(_IS_FUNC(obj, open_func))
			++count;
		else if(_IS_FUNC(obj, close_func) && _IS_EOS(obj_prev))
			--count;
	} while(count);

_exit:
	*l = ast;

	return result;
}

_running_context_t* _create_running_context(void) {
	/* Create a running context */
	_running_context_t* result = (_running_context_t*)mb_malloc(sizeof(_running_context_t));
	memset(result, 0, sizeof(_running_context_t));
	result->calc_depth = _INFINITY_CALC_DEPTH;

	return result;
}

_parsing_context_t* _reset_parsing_context(_parsing_context_t* context) {
	/* Reset the parsing context of a MY-BASIC environment */
	_ls_node_t* imp = 0;

	if(!context)
		context = (_parsing_context_t*)mb_malloc(sizeof(_parsing_context_t));
	else
		imp = context->imported;
	memset(context, 0, sizeof(_parsing_context_t));
	context->parsing_row = 1;
	if(!imp)
		imp = _ls_create();
	context->imported = imp;

	return context;
}

void _destroy_parsing_context(_parsing_context_t** context) {
	/* Destroy the parsing context of a MY-BASIC environment */
	if(!context || !(*context))
		return;

	if(*context) {
		if((*context)->imported) {
			_ls_foreach((*context)->imported, _destroy_memory);
			_ls_destroy((*context)->imported);
		}
		safe_free(*context);
	}
}

#ifdef MB_ENABLE_MODULE
_module_func_t* _create_module_func(mb_interpreter_t* s, mb_func_t f) {
	/* Create a module function structure */
	_module_func_t* result = 0;

	mb_assert(s);

	if(!s->with_module)
		return result;

	result = (_module_func_t*)mb_malloc(sizeof(_module_func_t));
	result->module = mb_strdup(s->with_module, 0);
	result->func = f;

	return result;
}

int _ls_destroy_module_func(void* data, void* extra) {
	/* Destroy a module function structure */
	int result = _OP_RESULT_NORMAL;
	_module_func_t* mod = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	mod = (_module_func_t*)data;
	safe_free(mod->module);
	safe_free(mod);

	result = _OP_RESULT_DEL_NODE;

	return result;
}

int _ht_destroy_module_func_list(void* data, void* extra) {
	/* Destroy a module function structure */
	int result = _OP_RESULT_NORMAL;
	_ls_node_t* lst = 0;
	char* n = 0;

	mb_assert(data);

	lst = (_ls_node_t*)data;
	n = (char*)extra;
	_ls_foreach(lst, _ls_destroy_module_func);
	_ls_destroy(lst);
	safe_free(n);

	result = _OP_RESULT_DEL_NODE;

	return result;
}
#endif /* MB_ENABLE_MODULE */

char* _generate_func_name(mb_interpreter_t* s, char* n, bool_t with_mod) {
	/* Generate a function name to be registered */
	char* name = 0;
	size_t _sl = 0;
	mb_unrefvar(with_mod);

	mb_assert(s && n);

	_sl = strlen(n);
#ifdef MB_ENABLE_MODULE
	if(with_mod && s->with_module) {
		size_t _ml = strlen(s->with_module);
		name = (char*)mb_malloc(_ml + 1 + _sl + 1);
		memcpy(name, s->with_module, _ml);
		name[_ml] = '.';
		memcpy(name + _ml + 1, n, _sl + 1);
	} else {
		name = (char*)mb_malloc(_sl + 1);
		memcpy(name, n, _sl + 1);
	}
#else /* MB_ENABLE_MODULE */
	name = (char*)mb_malloc(_sl + 1);
	memcpy(name, n, _sl + 1);
#endif /* MB_ENABLE_MODULE */
	mb_strupr(name);

	return name;
}

int _register_func(mb_interpreter_t* s, char* n, mb_func_t f, bool_t local) {
	/* Register a function to a MY-BASIC environment */
	int result = 0;
	_ht_node_t* scope = 0;
	_ls_node_t* exists = 0;
	char* name = 0;

	mb_assert(s);

	if(!n)
		return result;

	n = mb_strdup(n, strlen(n) + 1);
	mb_strupr(n);

	scope = local ? s->local_func_dict : s->global_func_dict;
#ifdef MB_ENABLE_MODULE
	if(s->with_module)
		name = _generate_func_name(s, n, true);
#endif /* MB_ENABLE_MODULE */
	if(!name)
		name = mb_strdup(n, strlen(n) + 1);
	exists = _ht_find(scope, (void*)name);
	if(!exists) {
		result += _ht_set_or_insert(scope, (void*)name, (void*)(intptr_t)f);
	} else {
		_set_current_error(s, SE_CM_FUNC_EXISTS, 0);
		safe_free(name);
	}

#ifdef MB_ENABLE_MODULE
	if(s->with_module) {
		_ls_node_t* tmp = 0;
		exists = _ht_find(s->module_func_dict, (void*)n);
		if(!exists) {
			name = _generate_func_name(s, n, false);
			result += _ht_set_or_insert(s->module_func_dict, (void*)name, _ls_create());
		}
		exists = _ht_find(s->module_func_dict, (void*)n);
		exists = (_ls_node_t*)exists->data;
		tmp = _ls_find(exists, s, (_ls_compare)_ht_cmp_module_func);
		if(!tmp)
			_ls_pushback(exists, _create_module_func(s, f));
		else
			_set_current_error(s, SE_CM_FUNC_EXISTS, 0);
	}
#endif /* MB_ENABLE_MODULE */

	safe_free(n);

	return result;
}

int _remove_func(mb_interpreter_t* s, char* n, bool_t local) {
	/* Remove a function from a MY-BASIC environment */
	int result = 0;
	_ht_node_t* scope = 0;
	_ls_node_t* exists = 0;
	char* name = 0;

	mb_assert(s);

	if(!n)
		return result;

	n = mb_strdup(n, strlen(n) + 1);
	mb_strupr(n);

	scope = local ? s->local_func_dict : s->global_func_dict;
#ifdef MB_ENABLE_MODULE
	if(s->with_module)
		name = _generate_func_name(s, n, true);
#endif /* MB_ENABLE_MODULE */
	if(!name)
		name = mb_strdup(n, strlen(n) + 1);
	exists = _ht_find(scope, (void*)name);
	if(exists)
		result += _ht_remove(scope, (void*)name, _ls_cmp_extra_string);
	else
		_set_current_error(s, SE_CM_FUNC_NOT_EXISTS, 0);
	safe_free(name);

#ifdef MB_ENABLE_MODULE
	if(s->with_module) {
		_ls_node_t* tmp = 0;
		exists = _ht_find(s->module_func_dict, (void*)n);
		if(exists) {
			exists = (_ls_node_t*)exists->data;
			tmp = _ls_find(exists, s, (_ls_compare)_ht_cmp_module_func);
			if(tmp)
				_ls_remove(exists, tmp, _ls_destroy_module_func);
		}
	}
#endif /* MB_ENABLE_MODULE */

	safe_free(n);

	return result;
}

_ls_node_t* _find_func(mb_interpreter_t* s, char* n, bool_t* mod) {
	/* Find function interface in function dictionary */
	_ls_node_t* result = 0;
	mb_unrefvar(mod);

	mb_assert(s && n);

	n = mb_strdup(n, strlen(n) + 1);
	mb_strupr(n);

	result = _ht_find(s->local_func_dict, (void*)n);
	if(!result)
		result = _ht_find(s->global_func_dict, (void*)n);

#ifdef MB_ENABLE_MODULE
	if(!result) {
		result = _ht_find(s->module_func_dict, (void*)n);
		if(result && result->data) {
			_module_func_t* mp = 0;
			result = (_ls_node_t*)result->data;
			result = result->next;
			while(result) {
				mp = (_module_func_t*)result->data;
				if(_ls_find(s->using_modules, mp->module, (_ls_compare)_ht_cmp_string))
					break;
				result = result->next;
			}
			*mod = true;
		}
	}
#endif /* MB_ENABLE_MODULE */

	safe_free(n);

	return result;
}

int _open_constant(mb_interpreter_t* s) {
	/* Open global constant */
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	unsigned long ul = 0;

	mb_assert(s);

	running = s->running_context;

	ul = _ht_set_or_insert(running->var_dict, "TRUE", _OBJ_BOOL_TRUE);
	mb_assert(ul);
	ul = _ht_set_or_insert(running->var_dict, "FALSE", _OBJ_BOOL_FALSE);
	mb_assert(ul);

	return result;
}

int _close_constant(mb_interpreter_t* s) {
	/* Close global constant */
	int result = MB_FUNC_OK;

	mb_assert(s);

	return result;
}

int _open_core_lib(mb_interpreter_t* s) {
	/* Open the core functional libraries */
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < _countof(_core_libs); ++i)
		result += _register_func(s, _core_libs[i].name, _core_libs[i].pointer, true);

	return result;
}

int _close_core_lib(mb_interpreter_t* s) {
	/* Close the core functional libraries */
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < _countof(_core_libs); ++i)
		result += _remove_func(s, _core_libs[i].name, true);

	return result;
}

int _open_std_lib(mb_interpreter_t* s) {
	/* Open the standard functional libraries */
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < _countof(_std_libs); ++i)
		result += _register_func(s, _std_libs[i].name, _std_libs[i].pointer, true);

	return result;
}

int _close_std_lib(mb_interpreter_t* s) {
	/* Close the standard functional libraries */
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < _countof(_std_libs); ++i)
		result += _remove_func(s, _std_libs[i].name, true);

	return result;
}

#ifdef MB_ENABLE_COLLECTION_LIB
int _open_coll_lib(mb_interpreter_t* s) {
	/* Open the collection functional libraries */
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < _countof(_coll_libs); ++i)
		result += _register_func(s, _coll_libs[i].name, _coll_libs[i].pointer, true);

	return result;
}

int _close_coll_lib(mb_interpreter_t* s) {
	/* Close the collection functional libraries */
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < _countof(_coll_libs); ++i)
		result += _remove_func(s, _coll_libs[i].name, true);

	return result;
}
#endif /* MB_ENABLE_COLLECTION_LIB */

/* ========================================================} */

/*
** {========================================================
** Public functions definitions
*/

unsigned int mb_ver(void) {
	/* Get the version number of this MY-BASIC system */
	return _MB_VERSION;
}

const char* mb_ver_string(void) {
	/* Get the version text of this MY-BASIC system */
	return _MB_VERSION_STRING;
}

int mb_init(void) {
	/* Initialize the MY-BASIC system */
	int result = MB_FUNC_OK;

	mb_assert(!_exp_assign);
	_exp_assign = _create_object();
	_exp_assign->type = _DT_FUNC;
	_exp_assign->data.func = (_func_t*)mb_malloc(sizeof(_func_t));
	memset(_exp_assign->data.func, 0, sizeof(_func_t));
	_exp_assign->data.func->name = (char*)mb_malloc(strlen("#") + 1);
	memcpy(_exp_assign->data.func->name, "#", strlen("#") + 1);
	_exp_assign->data.func->pointer = _core_dummy_assign;

	mb_assert(!_OBJ_BOOL_TRUE);
	if(!_OBJ_BOOL_TRUE) {
		_OBJ_BOOL_TRUE = _create_object();

		_OBJ_BOOL_TRUE->type = _DT_VAR;
		_OBJ_BOOL_TRUE->data.variable = (_var_t*)mb_malloc(sizeof(_var_t));
		memset(_OBJ_BOOL_TRUE->data.variable, 0, sizeof(_var_t));
		_OBJ_BOOL_TRUE->data.variable->name = (char*)mb_malloc(strlen("TRUE") + 1);
		memset(_OBJ_BOOL_TRUE->data.variable->name, 0, strlen("TRUE") + 1);
		strcpy(_OBJ_BOOL_TRUE->data.variable->name, "TRUE");

		_OBJ_BOOL_TRUE->data.variable->data = _create_object();
		_OBJ_BOOL_TRUE->data.variable->data->type = _DT_INT;
		_OBJ_BOOL_TRUE->data.variable->data->data.integer = 1;
	}
	mb_assert(!_OBJ_BOOL_FALSE);
	if(!_OBJ_BOOL_FALSE) {
		_OBJ_BOOL_FALSE = _create_object();

		_OBJ_BOOL_FALSE->type = _DT_VAR;
		_OBJ_BOOL_FALSE->data.variable = (_var_t*)mb_malloc(sizeof(_var_t));
		memset(_OBJ_BOOL_FALSE->data.variable, 0, sizeof(_var_t));
		_OBJ_BOOL_FALSE->data.variable->name = (char*)mb_malloc(strlen("FALSE") + 1);
		memset(_OBJ_BOOL_FALSE->data.variable->name, 0, strlen("FALSE") + 1);
		strcpy(_OBJ_BOOL_FALSE->data.variable->name, "FALSE");

		_OBJ_BOOL_FALSE->data.variable->data = _create_object();
		_OBJ_BOOL_FALSE->data.variable->data->type = _DT_INT;
		_OBJ_BOOL_FALSE->data.variable->data->data.integer = 0;
	}

	return result;
}

int mb_dispose(void) {
	/* Close the MY-BASIC system */
	int result = MB_FUNC_OK;

	mb_assert(_exp_assign);
	safe_free(_exp_assign->data.func->name);
	safe_free(_exp_assign->data.func);
	safe_free(_exp_assign);
	_exp_assign = 0;

	mb_assert(_OBJ_BOOL_TRUE);
	if(_OBJ_BOOL_TRUE) {
		safe_free(_OBJ_BOOL_TRUE->data.variable->data);
		safe_free(_OBJ_BOOL_TRUE->data.variable->name);
		safe_free(_OBJ_BOOL_TRUE->data.variable);
		safe_free(_OBJ_BOOL_TRUE);
		_OBJ_BOOL_TRUE = 0;
	}
	mb_assert(_OBJ_BOOL_FALSE);
	if(_OBJ_BOOL_FALSE) {
		safe_free(_OBJ_BOOL_FALSE->data.variable->data);
		safe_free(_OBJ_BOOL_FALSE->data.variable->name);
		safe_free(_OBJ_BOOL_FALSE->data.variable);
		safe_free(_OBJ_BOOL_FALSE);
		_OBJ_BOOL_FALSE = 0;
	}

	return result;
}

int mb_open(struct mb_interpreter_t** s) {
	/* Initialize a MY-BASIC environment */
	int result = MB_FUNC_OK;
	_ht_node_t* local_scope = 0;
	_ht_node_t* global_scope = 0;
	_running_context_t* running = 0;

	*s = (mb_interpreter_t*)mb_malloc(sizeof(mb_interpreter_t));
	memset(*s, 0, sizeof(mb_interpreter_t));

	(*s)->in_neg_expr = _ls_create();

	local_scope = _ht_create(0, _ht_cmp_string, _ht_hash_string, _ls_free_extra);
	(*s)->local_func_dict = local_scope;

	global_scope = _ht_create(0, _ht_cmp_string, _ht_hash_string, _ls_free_extra);
	(*s)->global_func_dict = global_scope;

#ifdef MB_ENABLE_MODULE
	global_scope = _ht_create(0, _ht_cmp_string, _ht_hash_string, _ht_destroy_module_func_list);
	(*s)->module_func_dict = global_scope;
	(*s)->using_modules = _ls_create();
#endif /* MB_ENABLE_MODULE */

	(*s)->parsing_context = _reset_parsing_context((*s)->parsing_context);

	(*s)->temp_values = _ls_create();
	(*s)->lazy_destroy_objects = _ls_create();

#ifdef MB_ENABLE_GC
	(*s)->gc.table = _ht_create(0, _ht_cmp_ref, _ht_hash_ref, _do_nothing_on_object);
	(*s)->gc.recursive_table = _ht_create(0, _ht_cmp_ref, _ht_hash_ref, _do_nothing_on_object);
	(*s)->gc.collected_table = _ht_create(0, _ht_cmp_ref, _ht_hash_ref, _do_nothing_on_object);
#endif /* MB_ENABLE_GC */

	running = _create_running_context();
	running->meta = _SCOPE_META_ROOT;
	(*s)->running_context = running;
	global_scope = _ht_create(0, _ht_cmp_string, _ht_hash_string, 0);
	running->var_dict = global_scope;

#ifdef MB_ENABLE_STACK_TRACE
	(*s)->stack_frames = _ls_create();
#endif /* MB_ENABLE_STACK_TRACE */

	(*s)->sub_stack = _ls_create();

	(*s)->ast = _ls_create();

	_open_core_lib(*s);
	_open_std_lib(*s);
#ifdef MB_ENABLE_COLLECTION_LIB
	_open_coll_lib(*s);
#endif /* MB_ENABLE_COLLECTION_LIB */

	result = _open_constant(*s);
	mb_assert(MB_FUNC_OK == result);

	return result;
}

int mb_close(struct mb_interpreter_t** s) {
	/* Close a MY-BASIC environment */
	int result = MB_FUNC_OK;
	_ht_node_t* local_scope = 0;
	_ht_node_t* global_scope = 0;
	_ls_node_t* ast;

	mb_assert(s);

#ifdef MB_ENABLE_COLLECTION_LIB
	_close_coll_lib(*s);
#endif /* MB_ENABLE_COLLECTION_LIB */
	_close_std_lib(*s);
	_close_core_lib(*s);

	ast = (*s)->ast;
	_ls_foreach(ast, _destroy_object);
	_ls_destroy(ast);

	_ls_destroy((*s)->sub_stack);

#ifdef MB_ENABLE_STACK_TRACE
	_ls_destroy((*s)->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */

	_tidy_scope_chain(*s);
	_dispose_scope_chain(*s);

#ifdef MB_ENABLE_GC
	_gc_collect_garbage(*s, 1);
	_ht_destroy((*s)->gc.table);
	_ht_destroy((*s)->gc.recursive_table);
	_ht_destroy((*s)->gc.collected_table);
	(*s)->gc.table = 0;
	(*s)->gc.recursive_table = 0;
	(*s)->gc.collected_table = 0;
#endif /* MB_ENABLE_GC */

	_ls_foreach((*s)->temp_values, _destroy_object);
	_ls_destroy((*s)->temp_values);
	_ls_foreach((*s)->lazy_destroy_objects, _destroy_object);
	_ls_destroy((*s)->lazy_destroy_objects);

	_destroy_parsing_context(&(*s)->parsing_context);

#ifdef MB_ENABLE_MODULE
	global_scope = (*s)->module_func_dict;
	_ht_foreach(global_scope, _ht_destroy_module_func_list);
	_ht_destroy(global_scope);
	_ls_foreach((*s)->using_modules, _destroy_memory);
	_ls_destroy((*s)->using_modules);
#endif /* MB_ENABLE_MODULE */

	global_scope = (*s)->global_func_dict;
	_ht_foreach(global_scope, _ls_free_extra);
	_ht_destroy(global_scope);

	local_scope = (*s)->local_func_dict;
	_ht_foreach(local_scope, _ls_free_extra);
	_ht_destroy(local_scope);

	_ls_destroy((*s)->in_neg_expr);

	_close_constant(*s);

	safe_free(*s);

	return result;
}

int mb_reset(struct mb_interpreter_t** s, bool_t clrf/* = false*/) {
	/* Reset a MY-BASIC environment */
	int result = MB_FUNC_OK;
	_ht_node_t* global_scope = 0;
	_ls_node_t* ast;
	_parsing_context_t* context = 0;
	_running_context_t* running = 0;

	mb_assert(s);

	(*s)->jump_set = _JMP_NIL;
	(*s)->last_routine = 0;
	(*s)->no_eat_comma_mark = 0;
	(*s)->last_error = SE_NO_ERR;
	(*s)->last_error_func = 0;

	running = (*s)->running_context;
	_ls_clear((*s)->sub_stack);
	(*s)->suspent_point = 0;
	running->next_loop_var = 0;
	memset(&(running->intermediate_value), 0, sizeof(mb_value_t));

	(*s)->parsing_context = context = _reset_parsing_context((*s)->parsing_context);

	ast = (*s)->ast;
	_ls_foreach(ast, _destroy_object);
	_ls_clear(ast);

#ifdef MB_ENABLE_STACK_TRACE
	_ls_clear((*s)->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */

	_clear_scope_chain(*s);

	if(clrf) {
#ifdef MB_ENABLE_MODULE
		global_scope = (*s)->module_func_dict;
		_ht_foreach(global_scope, _ht_destroy_module_func_list);
		_ht_clear(global_scope);
		_ls_foreach((*s)->using_modules, _destroy_memory);
		_ls_clear((*s)->using_modules);
#endif /* MB_ENABLE_MODULE */

		global_scope = (*s)->global_func_dict;
		_ht_foreach(global_scope, _ls_free_extra);
		_ht_clear(global_scope);
	}

	result = _open_constant(*s);
	mb_assert(MB_FUNC_OK == result);

	return result;
}

int mb_register_func(struct mb_interpreter_t* s, const char* n, mb_func_t f) {
	/* Register an API function to a MY-BASIC environment */
	return _register_func(s, (char*)n, f, false);
}

int mb_remove_func(struct mb_interpreter_t* s, const char* n) {
	/* Remove an API function from a MY-BASIC environment */
	return _remove_func(s, (char*)n, false);
}

int mb_remove_reserved_func(struct mb_interpreter_t* s, const char* n) {
	/* Remove a reserved API from a MY-BASIC environment */
	return _remove_func(s, (char*)n, true);
}

int mb_begin_module(struct mb_interpreter_t* s, const char* n) {
	/* Begin a module, all functions registered with a module will put inside it */
	int result = MB_FUNC_OK;

	mb_assert(s && n);

#ifdef MB_ENABLE_MODULE
	s->with_module = mb_strdup(n, strlen(n) + 1);

	goto _exit; /* Avoid an unreferenced label warning */
#else /* MB_ENABLE_MODULE */
	_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, (_object_t*)0, MB_FUNC_WARNING, _exit, result);
#endif /* MB_ENABLE_MODULE */

_exit:
	return result;
}

int mb_end_module(struct mb_interpreter_t* s) {
	/* End a module */
	int result = MB_FUNC_OK;

	mb_assert(s);

#ifdef MB_ENABLE_MODULE
	if(s->with_module) {
		safe_free(s->with_module);
	}

	goto _exit; /* Avoid an unreferenced label warning */
#else /* MB_ENABLE_MODULE */
	_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, (_object_t*)0, MB_FUNC_WARNING, _exit, result);
#endif /* MB_ENABLE_MODULE */

_exit:
	return result;
}

int mb_attempt_func_begin(struct mb_interpreter_t* s, void** l) {
	/* Try attempting to begin an API function */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	obj = (_object_t*)ast->data;
	if(!(obj->type == _DT_FUNC)) {
		_handle_error_on_obj(s, SE_RN_STRUCTURE_NOT_COMPLETED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;

	++s->no_eat_comma_mark;

_exit:
	*l = ast;

	return result;
}

int mb_attempt_func_end(struct mb_interpreter_t* s, void** l) {
	/* Try attempting to end an API function */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	--s->no_eat_comma_mark;

	return result;
}

int mb_attempt_open_bracket(struct mb_interpreter_t* s, void** l) {
	/* Try attempting an open bracket */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	ast = ast->next;
	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_open_bracket)) {
		_handle_error_on_obj(s, SE_RN_OPEN_BRACKET_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;

_exit:
	*l = ast;

	return result;
}

int mb_attempt_close_bracket(struct mb_interpreter_t* s, void** l) {
	/* Try attempting a close bracket */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	if(!ast) {
		_handle_error_on_obj(s, SE_RN_CLOSE_BRACKET_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_close_bracket)) {
		_handle_error_on_obj(s, SE_RN_CLOSE_BRACKET_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;

_exit:
	*l = ast;

	return result;
}

int mb_has_arg(struct mb_interpreter_t* s, void** l) {
	/* Detect if there is any more argument */
	int result = 0;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	if(ast) {
		obj = (_object_t*)ast->data;
		if(!_IS_FUNC(obj, _core_close_bracket) && obj->type != _DT_EOS)
			result = obj->type != _DT_SEP && obj->type != _DT_EOS;
	}

	return result;
}

int mb_pop_int(struct mb_interpreter_t* s, void** l, int_t* val) {
	/* Pop an integer argument */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	int_t tmp = 0;

	mb_assert(s && l && val);

	mb_make_nil(arg);

	mb_check(mb_pop_value(s, l, &arg));

	switch(arg.type) {
	case MB_DT_INT:
		tmp = arg.value.integer;

		break;
	case MB_DT_REAL:
		tmp = (int_t)(arg.value.float_point);

		break;
	default:
		result = MB_FUNC_ERR;

		goto _exit;
	}

	*val = tmp;

_exit:
	return result;
}

int mb_pop_real(struct mb_interpreter_t* s, void** l, real_t* val) {
	/* Pop a float point argument */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	real_t tmp = 0;

	mb_assert(s && l && val);

	mb_make_nil(arg);

	mb_check(mb_pop_value(s, l, &arg));

	switch(arg.type) {
	case MB_DT_INT:
		tmp = (real_t)(arg.value.integer);

		break;
	case MB_DT_REAL:
		tmp = arg.value.float_point;

		break;
	default:
		result = MB_FUNC_ERR;

		goto _exit;
	}

	*val = tmp;

_exit:
	return result;
}

int mb_pop_string(struct mb_interpreter_t* s, void** l, char** val) {
	/* Pop a string argument */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	char* tmp = 0;

	mb_assert(s && l && val);

	mb_make_nil(arg);

	mb_check(mb_pop_value(s, l, &arg));

	switch(arg.type) {
	case MB_DT_STRING:
		tmp = arg.value.string;

		break;
	default:
		result = MB_FUNC_ERR;

		goto _exit;
	}

	*val = tmp;

_exit:
	return result;
}

int mb_pop_usertype(struct mb_interpreter_t* s, void** l, void** val) {
	/* Pop a usertype argument */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	void* tmp = 0;

	mb_assert(s && l && val);

	mb_make_nil(arg);

	mb_check(mb_pop_value(s, l, &arg));

	switch(arg.type) {
	case MB_DT_USERTYPE:
		tmp = arg.value.usertype;

		break;
	default:
		result = MB_FUNC_ERR;

		goto _exit;
	}

	*val = tmp;

_exit:
	return result;
}

int mb_pop_value(struct mb_interpreter_t* s, void** l, mb_value_t* val) {
	/* Pop an argument value */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t val_obj;
	_object_t* val_ptr = 0;
	_running_context_t* running = 0;
	int* inep = 0;

	mb_assert(s && l && val);

	running = s->running_context;

	if(!_ls_empty(s->in_neg_expr))
		inep = (int*)_ls_back(s->in_neg_expr)->data;

	val_ptr = &val_obj;
	_MAKE_NIL(val_ptr);

	ast = (_ls_node_t*)*l;
	result = _calc_expression(s, &ast, &val_ptr);
	if(result != MB_FUNC_OK)
		goto _exit;

	if(val_ptr->type == _DT_STRING && !val_ptr->ref) {
		_ls_foreach(s->temp_values, _destroy_object);
		_ls_clear(s->temp_values);

		val_ptr = _create_object();
		memcpy(val_ptr, &val_obj, sizeof(_object_t));
		_ls_pushback(s->temp_values, val_ptr);
	}
	_REF(val_ptr)

	if(s->no_eat_comma_mark < _NO_EAT_COMMA && (!inep || (inep && !(*inep)))) {
		if(ast && _IS_SEP(ast->data, ','))
			ast = ast->next;
	}

	result = _internal_object_to_public_value(val_ptr, val);
	if(result != MB_FUNC_OK)
		goto _exit;

_exit:
	*l = ast;

	return result;
}

int mb_push_int(struct mb_interpreter_t* s, void** l, int_t val) {
	/* Push an integer argument */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	arg.type = MB_DT_INT;
	arg.value.integer = val;
	mb_check(mb_push_value(s, l, arg));

	return result;
}

int mb_push_real(struct mb_interpreter_t* s, void** l, real_t val) {
	/* Push a float point argument */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	arg.type = MB_DT_REAL;
	arg.value.float_point = val;
	mb_convert_to_int_if_posible(arg);
	mb_check(mb_push_value(s, l, arg));

	return result;
}

int mb_push_string(struct mb_interpreter_t* s, void** l, char* val) {
	/* Push a string argument */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	arg.type = MB_DT_STRING;
	arg.value.string = val;
	mb_check(mb_push_value(s, l, arg));
	_mark_lazy_destroy_string(s, val);

	return result;
}

int mb_push_usertype(struct mb_interpreter_t* s, void** l, void* val) {
	/* Push a usertype argument */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	arg.type = MB_DT_USERTYPE;
	arg.value.usertype = val;
	mb_check(mb_push_value(s, l, arg));

	return result;
}

int mb_push_value(struct mb_interpreter_t* s, void** l, mb_value_t val) {
	/* Push an argument value */
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_object_t obj;

	mb_assert(s && l);

	running = s->running_context;
	_assign_public_value(&running->intermediate_value, &val);

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&running->intermediate_value, &obj);
	_REF(&obj)

#ifdef MB_ENABLE_GC
	_gc_try_trigger(s);
#endif /* MB_ENABLE_GC */

	return result;
}

int mb_begin_class(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t** meta, int c, mb_value_t* out) {
	/* Begin a class */
#ifdef MB_ENABLE_CLASS
	int result = MB_FUNC_OK;
	_class_t* instance = 0;
	_object_t* obj = 0;
	_running_context_t* running = 0;
	_ls_node_t* tmp = 0;
	_var_t* var = 0;
	int i = 0;
	_object_t mo;
	_class_t* mi = 0;

	mb_assert(s && l && n && out);

	running = s->running_context;

	tmp = (_ls_node_t*)*l;

	_using_jump_set_of_structured(s, tmp, _exit, result);

	tmp = _search_identifier_in_scope_chain(s, 0, n, 0, 0);
	if(tmp && tmp->data) {
		obj = (_object_t*)tmp->data;
		if(obj->type == _DT_VAR)
			var = obj->data.variable;
	}
	if(s->last_instance || (obj && !var)) {
		_handle_error_on_obj(s, SE_RN_DUPLICATE_CLASS, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

	obj = _create_object();
	obj->type = _DT_CLASS;

	instance = (_class_t*)mb_malloc(sizeof(_class_t));
	_init_class(s, instance, mb_strdup(n, strlen(n) + 1));

	for(i = 0; i < c; i++) {
		if(meta[i]->type != MB_DT_CLASS) {
			_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}
		_MAKE_NIL(&mo);
		_public_value_to_internal_object(meta[i], &mo);
		mi = mo.data.instance;
		_link_meta_class(s, instance, mi);
	}

	_push_scope_by_class(s, instance->scope);
	obj->data.instance = instance;

	if(var) {
		_destroy_object(var->data, 0);
		var->data = obj;
	} else {
		_ht_set_or_insert(running->var_dict, (void*)n, obj);
	}

	s->last_instance = instance;

_exit:
	return result;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(n);
	mb_unrefvar(meta);
	mb_unrefvar(c);
	mb_unrefvar(out);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_CLASS */
}

int mb_end_class(struct mb_interpreter_t* s, void** l) {
	/* End a class */
#ifdef MB_ENABLE_CLASS
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_pop_scope(s);

	s->last_instance = 0;

	return result;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(s);
	mb_unrefvar(l);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_CLASS */
}

int mb_get_class_userdata(struct mb_interpreter_t* s, void** l, void** d) {
	/* Get the userdata of a class instance */
#ifdef MB_ENABLE_CLASS
	int result = MB_FUNC_OK;

	mb_assert(s && l && d);

	if(s && s->last_instance) {
		if(d)
			*d = s->last_instance->userdata;
	} else if(s && s->last_routine && s->last_routine->instance) {
		if(d)
			*d = s->last_routine->instance->userdata;
	} else {
		if(d) *d = 0;

		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

_exit:
	return result;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(d);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_CLASS */
}

int mb_set_class_userdata(struct mb_interpreter_t* s, void** l, void* d) {
	/* Set the userdata of a class instance */
#ifdef MB_ENABLE_CLASS
	int result = MB_FUNC_OK;

	mb_assert(s && l && d);

	if(s && s->last_instance) {
		s->last_instance->userdata = d;
	} else {
		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

_exit:
	return result;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(d);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_CLASS */
}

int mb_get_value_by_name(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t* val) {
	/* Get a value by its identifier name */
	int result = MB_FUNC_OK;
	_ls_node_t* tmp = 0;
	_object_t* obj = 0;

	mb_assert(s && l && n);

	mb_make_nil(*val);

	tmp = _search_identifier_in_scope_chain(s, 0, n, 1, 0);
	if(tmp && tmp->data) {
		obj = (_object_t*)tmp->data;
		_internal_object_to_public_value(obj, val);
	}

	return result;
}

int mb_add_var(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t val, bool_t force) {
	/* Add a variable with a specific name */
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_object_t* obj = 0;
	_var_t* var = 0;
	_ls_node_t* tmp = 0;

	mb_assert(s && l && n);

	running = s->running_context;

	tmp = _ht_find(running->var_dict, (void*)n);

	if(tmp) {
		if(force) {
			result = mb_set_var_value(s, tmp->data, val);

			goto _exit;
		} else {
			_handle_error_on_obj(s, SE_RN_DUPLICATE_ID, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}
	}

	var = _create_var(&obj, n, true);
	_public_value_to_internal_object(&val, var->data);

	_ht_set_or_insert(running->var_dict, var->name, obj);

_exit:
	return result;
}

int mb_get_var(struct mb_interpreter_t* s, void** l, void** v) {
	/* Get a token literally, store it in an argument if it's a variable */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	mb_assert(s && l);

	if(v) *v = 0;

	ast = (_ls_node_t*)*l;
	if(ast) {
		obj = (_object_t*)ast->data;
		if(_IS_SEP(obj, ',')) {
			ast = ast->next;
			obj = (_object_t*)ast->data;
		}
		ast = ast->next;
	}

	if(obj && obj->type == _DT_VAR) {
		if(v)
			*v = obj;
	}

	*l = ast;

	return result;
}

int mb_get_var_value(struct mb_interpreter_t* s, void* v, mb_value_t* val) {
	/* Get the value of a variable */
	int result = MB_FUNC_OK;
	_object_t* obj = 0;

	mb_assert(s);

	if(!val || !v) goto _exit;

	obj = (_object_t*)v;
	if(obj->type != _DT_VAR) goto _exit;

	_internal_object_to_public_value(obj->data.variable->data, val);

_exit:
	return result;
}

int mb_set_var_value(struct mb_interpreter_t* s, void* v, mb_value_t val) {
	/* Set the value of a variable */
	int result = MB_FUNC_OK;
	_object_t* obj = 0;

	mb_assert(s);

	if(!v) goto _exit;
	obj = (_object_t*)v;
	if(obj->type != _DT_VAR) goto _exit;

	_public_value_to_internal_object(&val, obj->data.variable->data);

_exit:
	return result;
}

int mb_init_array(struct mb_interpreter_t* s, void** l, mb_data_e t, int* d, int c, void** a) {
	/* Create an array */
	int result = MB_FUNC_OK;
	_array_t* arr = 0;
	_data_e type = _DT_NIL;
	int j = 0;
	int n = 0;
	mb_unrefvar(t);

	mb_assert(s && l && d && a);

	*a = 0;
#ifdef MB_SIMPLE_ARRAY
	if(t == MB_DT_REAL) {
		type = _DT_REAL;
	} else if(t == MB_DT_STRING) {
		type = _DT_STRING;
	} else {
		result = MB_NEED_COMPLEX_ARRAY;

		goto _exit;
	}
#else /* MB_SIMPLE_ARRAY */
	type = _DT_REAL;
#endif /* MB_SIMPLE_ARRAY */

	arr = _create_array(0, type, s);
	for(j = 0; j < c; j++) {
		n = d[j];
		arr->dimensions[arr->dimension_count++] = n;
		if(arr->count)
			arr->count *= n;
		else
			arr->count += n;
	}
	_init_array(arr);
	if(!arr->raw) {
		arr->dimension_count = 0;
		arr->dimensions[0] = 0;
		arr->count = 0;
	}
	*a = arr;

	goto _exit; /* Avoid an unreferenced label warning */

_exit:
	return result;
}

int mb_get_array_len(struct mb_interpreter_t* s, void** l, void* a, int r, int* i) {
	/* Get the length of an array */
	int result = MB_FUNC_OK;
	_array_t* arr = 0;

	mb_assert(s && l);

	arr = (_array_t*)a;
	if(r < 0 || r >= arr->dimension_count) {
		result = MB_RANK_OUT_OF_BOUNDS;

		goto _exit;
	}
	if(i)
		*i = arr->dimensions[r];

_exit:
	return result;
}

int mb_get_array_elem(struct mb_interpreter_t* s, void** l, void* a, int* d, int c, mb_value_t* val) {
	/* Get an element of an array with a specific index */
	int result = MB_FUNC_OK;
	_array_t* arr = 0;
	int index = 0;
	_data_e type = _DT_NIL;

	mb_assert(s && l);

	arr = (_array_t*)a;
	if(c < 0 || c > arr->dimension_count) {
		result = MB_RANK_OUT_OF_BOUNDS;

		goto _exit;
	}
	if(!val)
		goto _exit;

	index = _get_array_pos(s, arr, d, c);
	if(index < 0) {
		result = MB_INDEX_OUT_OF_BOUNDS;

		goto _exit;
	}

	_get_array_elem(s, arr, index, &val->value, &type);
	val->type = _internal_type_to_public_type(type);

_exit:
	return result;
}

int mb_set_array_elem(struct mb_interpreter_t* s, void** l, void* a, int* d, int c, mb_value_t val) {
	/* Set an element of an array with a specific index */
	int result = MB_FUNC_OK;
	_array_t* arr = 0;
	int index = 0;
	_data_e type = _DT_NIL;

	mb_assert(s && l);

	arr = (_array_t*)a;
	if(c < 0 || c > arr->dimension_count) {
		result = MB_RANK_OUT_OF_BOUNDS;

		goto _exit;
	}

	index = _get_array_pos(s, arr, d, c);
	if(index < 0) {
		result = MB_INDEX_OUT_OF_BOUNDS;

		goto _exit;
	}

	type = _public_type_to_internal_type(val.type);
	_set_array_elem(s, 0, arr, (unsigned int)index, &val.value, &type);

_exit:
	return result;
}

int mb_init_coll(struct mb_interpreter_t* s, void** l, mb_value_t* coll) {
	/* Initialize a collection */
	int result = MB_FUNC_OK;

	mb_assert(s);

#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll->type) {
	case MB_DT_LIST:
		coll->value.list = _create_list(s);

		break;
	case MB_DT_DICT:
		coll->value.dict = _create_dict(s);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(coll);
	_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	return result;
}

int mb_get_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t idx, mb_value_t* val) {
	/* Get an element of a collection */
	int result = MB_FUNC_OK;
	_object_t ocoll;
	int_t i = 0;
	mb_value_t ret;

	mb_assert(s);

	mb_make_nil(ret);

	_MAKE_NIL(&ocoll);
#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll.type) {
	case MB_DT_LIST:
		mb_int_val(idx, i);
		_public_value_to_internal_object(&coll, &ocoll);
		if(!_at_list(ocoll.data.list, i, &ret)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		if(!_find_dict(ocoll.data.dict, &idx, &ret)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(idx);
	mb_unrefvar(coll);
	mb_unrefvar(i);
	_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	if(val) *val = ret;

	return result;
}

int mb_set_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t idx, mb_value_t val) {
	/* Set an element of a collection */
	int result = MB_FUNC_OK;
	_object_t ocoll;
	int_t i = 0;
	_object_t* oval = 0;
	mb_value_t ret;

	mb_assert(s);

	mb_make_nil(ret);

	_MAKE_NIL(&ocoll);
#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll.type) {
	case MB_DT_LIST:
		mb_int_val(idx, i);
		_public_value_to_internal_object(&coll, &ocoll);
		while((int)ocoll.data.list->count <= i)
			_push_list(ocoll.data.list, &ret, 0);
		if(!_set_list(ocoll.data.list, i, &val, &oval)) {
			_destroy_object(oval, 0);

			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		_set_dict(ocoll.data.dict, &idx, &val, 0, 0);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(val);
	mb_unrefvar(idx);
	mb_unrefvar(coll);
	mb_unrefvar(oval);
	mb_unrefvar(i);
	_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	return result;
}

int mb_remove_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t idx) {
	/* Remove an element from a collection */
	int result = MB_FUNC_OK;
	_object_t ocoll;
	int_t i = 0;
	mb_value_t ret;

	mb_assert(s);

	mb_make_nil(ret);

	_MAKE_NIL(&ocoll);
#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll.type) {
	case MB_DT_LIST:
		mb_int_val(idx, i);
		_public_value_to_internal_object(&coll, &ocoll);
		if(!_remove_at_list(ocoll.data.list, i)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		if(!_remove_dict(ocoll.data.dict, &idx)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(coll);
	mb_unrefvar(idx);
	mb_unrefvar(i);
	_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	return result;
}

int mb_count_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, int* c) {
	/* Tell the element count of a collection */
	int result = MB_FUNC_OK;
	_object_t ocoll;
#ifdef MB_ENABLE_COLLECTION_LIB
	_list_t* lst = 0;
	_dict_t* dct = 0;
#endif /* MB_ENABLE_COLLECTION_LIB */
	int ret = 0;

	mb_assert(s);

	_MAKE_NIL(&ocoll);
#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll.type) {
	case MB_DT_LIST:
		lst = (_list_t*)coll.value.list;
		ret = (int)lst->count;

		break;
	case MB_DT_DICT:
		dct = (_dict_t*)coll.value.dict;
		ret = (int)_ht_count(dct->dict);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(coll);
	_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	if(c) *c = ret;

	return result;
}

int mb_make_ref_value(struct mb_interpreter_t* s, void* val, mb_value_t* out, mb_dtor_func_t un, mb_clone_func_t cl, mb_hash_func_t hs, mb_cmp_func_t cp, mb_fmt_func_t ft) {
	/* Create a referenced usertype value */
	int result = MB_FUNC_OK;
	_usertype_ref_t* ref = 0;

	mb_assert(s && out);

	if(out) {
		ref = _create_usertype_ref(s, val, un, cl, hs, cp, ft);
		out->type = MB_DT_USERTYPE_REF;
		out->value.usertype_ref = ref;
	}

	return result;
}

int mb_get_ref_value(struct mb_interpreter_t* s, void** l, mb_value_t val, void** out) {
	/* Get the data of a referenced usertype value */
	int result = MB_FUNC_OK;
	_usertype_ref_t* ref = 0;

	mb_assert(s && out);

	if(val.type != MB_DT_USERTYPE_REF) {
		_handle_error_on_obj(s, SE_RN_TYPE_NOT_MATCH, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

	if(out) {
		ref = (_usertype_ref_t*)val.value.usertype_ref;
		*out = ref->usertype;
	}

_exit:
	return result;
}

int mb_ref_value(struct mb_interpreter_t* s, void** l, mb_value_t val) {
	/* Add reference to a value */
	int result = MB_FUNC_OK;
	_object_t obj;

	mb_assert(s && l);

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&val, &obj);
	if(obj.type != _DT_USERTYPE_REF &&
#ifdef MB_ENABLE_COLLECTION_LIB
		obj.type != _DT_LIST && obj.type != _DT_DICT &&
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		obj.type != _DT_CLASS &&
#endif /* MB_ENABLE_CLASS */
		obj.type != _DT_ARRAY
	) {
		_handle_error_on_obj(s, SE_RN_REFERENCED_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}
	_REF(&obj);

_exit:
	return result;
}

int mb_unref_value(struct mb_interpreter_t* s, void** l, mb_value_t val) {
	/* Remove reference to a value */
	int result = MB_FUNC_OK;
	_object_t obj;

	mb_assert(s && l);

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&val, &obj);
	if(obj.type != _DT_USERTYPE_REF &&
#ifdef MB_ENABLE_COLLECTION_LIB
		obj.type != _DT_LIST && obj.type != _DT_DICT &&
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		obj.type != _DT_CLASS &&
#endif /* MB_ENABLE_CLASS */
		obj.type != _DT_ARRAY
	) {
		_handle_error_on_obj(s, SE_RN_REFERENCED_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}
	_UNREF(&obj);

_exit:
	return result;
}

int mb_dispose_value(struct mb_interpreter_t* s, mb_value_t val) {
	/* Dispose a value */
	int result = MB_FUNC_OK;

	mb_assert(s);

	if(val.type == MB_DT_STRING)
		safe_free(val.value.string);

	_assign_public_value(&val, 0);

	return result;
}

int mb_get_routine(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t* val) {
	/* Get a sub routine with a specific name */
	int result = MB_FUNC_OK;
	_object_t* obj = 0;
	_ls_node_t* scp = 0;

	mb_assert(s && l && n && val);

	mb_make_nil(*val);

	scp = _search_identifier_in_scope_chain(s, 0, n, 0, 0);
	if(scp) {
		obj = (_object_t*)scp->data;
		if(obj) {
			if(obj->type == _DT_ROUTINE) {
				_internal_object_to_public_value(obj, val);
			} else {
				_handle_error_on_obj(s, SE_RN_ROUTINE_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
			}
		}
	}

_exit:
	return result;
}

int mb_set_routine(struct mb_interpreter_t* s, void** l, const char* n, mb_routine_func_t f, bool_t force) {
	/* Set a sub routine with a specific name and functor */
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_object_t* obj = 0;
	_routine_t* routine = 0;
	_ls_node_t* tmp = 0;
	_var_t* var = 0;

	mb_assert(s && l);

	running = s->running_context;

	tmp = _ht_find(running->var_dict, (void*)n);

	if(tmp) {
		if(force) {
			obj = (_object_t*)tmp->data;
			if(obj->type == _DT_VAR)
				var = obj->data.variable;
		} else {
			_handle_error_on_obj(s, SE_RN_DUPLICATE_ROUTINE, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}
	}

	routine = (_routine_t*)mb_malloc(sizeof(_routine_t));
	_init_routine(s, routine, mb_strdup(n, strlen(n) + 1), f);

	obj = _create_object();
	obj->type = _DT_ROUTINE;
	obj->data.routine = routine;
	obj->ref = false;

#ifdef MB_ENABLE_CLASS
	routine->instance = s->last_instance;
#endif /* MB_ENABLE_CLASS */

	if(var && force) {
		_destroy_object(var->data, 0);
		var->data = obj;
	} else {
		_ht_set_or_insert(running->var_dict, routine->name, obj);
	}

_exit:
	return result;
}

int mb_eval_routine(struct mb_interpreter_t* s, void** l, mb_value_t val, mb_value_t* args, unsigned argc) {
	/* Evaluate a sub routine */
	int result = MB_FUNC_OK;
	_object_t obj;

	if(val.type != MB_DT_ROUTINE) {
		_handle_error_on_obj(s, SE_RN_ROUTINE_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&val, &obj);
	result = _eval_routine(s, (_ls_node_t**)l, args, argc, obj.data.routine, _has_routine_fun_arg, _pop_routine_fun_arg);

_exit:
	return result;
}

int mb_load_string(struct mb_interpreter_t* s, const char* l) {
	/* Load and parse a script string */
	int result = MB_FUNC_OK;
	char ch = 0;
	int status = 0;
	int i = 0;
	unsigned short _row = 0;
	unsigned short _col = 0;
	char wrapped = '\0';
	_parsing_context_t* context = 0;

	mb_assert(s && s->parsing_context);

	context = s->parsing_context;

	while(l[i]) {
		ch = l[i];
		if((ch == '\n' || ch == '\r') && (!wrapped || wrapped == ch)) {
			wrapped = ch;
			++context->parsing_row;
			context->parsing_col = 0;
		} else {
			wrapped = '\0';
			++context->parsing_col;
		}
		status = _parse_char(s, ch, context->parsing_pos, _row, _col);
		result = status;
		if(status) {
			_set_error_pos(s, context->parsing_pos, _row, _col);
			_handle_error_now(s, s->last_error, s->last_error_func, result);

			goto _exit;
		}
		_row = context->parsing_row;
		_col = context->parsing_col;
		++i;
		++context->parsing_pos;
	};
	status = _parse_char(s, MB_EOS, context->parsing_pos, context->parsing_row, context->parsing_col);

_exit:
	context->parsing_state = _PS_NORMAL;

	return result;
}

int mb_load_file(struct mb_interpreter_t* s, const char* f) {
	/* Load and parse a script file */
	int result = MB_FUNC_OK;
	char* buf = 0;
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;

	s->parsing_context = context = _reset_parsing_context(s->parsing_context);

	buf = _load_file(s, f, 0);
	if(buf) {
		result = mb_load_string(s, buf);
		safe_free(buf);

		if(result)
			goto _exit;
	} else {
		_set_current_error(s, SE_PS_FILE_OPEN_FAILED, 0);

		++result;
	}

_exit:
	context->parsing_state = _PS_NORMAL;

	return result;
}

int mb_run(struct mb_interpreter_t* s) {
	/* Run loaded and parsed script */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;

	_destroy_parsing_context(&s->parsing_context);

	s->handled_error = false;

	if(s->suspent_point) {
		ast = s->suspent_point;
		ast = ast->next;
		s->suspent_point = 0;
	} else {
		mb_assert(!s->no_eat_comma_mark);
		while(s->running_context->prev)
			s->running_context = s->running_context->prev;
		ast = s->ast;
		ast = ast->next;
		if(!ast) {
			result = MB_FUNC_ERR;
			_set_error_pos(s, 0, 0, 0);
			_handle_error_now(s, SE_RN_EMPTY_PROGRAM, 0, result);

			goto _exit;
		}
	}

	do {
		result = _execute_statement(s, &ast);
		_ls_foreach(s->lazy_destroy_objects, _destroy_object);
		_ls_clear(s->lazy_destroy_objects);
		if(result != MB_FUNC_OK && result != MB_SUB_RETURN) {
			if(result != MB_FUNC_SUSPEND && s->error_handler) {
				if(result >= MB_EXTENDED_ABORT)
					s->last_error = SE_EA_EXTENDED_ABORT;
				_handle_error_now(s, s->last_error, s->last_error_func, result);
			}

			goto _exit;
		}
	} while(ast);

_exit:
	_ls_foreach(s->temp_values, _destroy_object);
	_ls_clear(s->temp_values);

	return result;
}

int mb_suspend(struct mb_interpreter_t* s, void** l) {
	/* Suspend current execution and save the context */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;

	mb_assert(s && l && *l);

	ast = (_ls_node_t*)*l;
	s->suspent_point = ast;

	return result;
}

int mb_schedule_suspend(struct mb_interpreter_t* s, int t) {
	/* Schedule to suspend current execution */
	int result = MB_FUNC_OK;

	mb_assert(s);

	if(t == MB_FUNC_OK)
		t = MB_FUNC_SUSPEND;
	s->schedule_suspend_tag = t;

	return result;
}

int mb_debug_get(struct mb_interpreter_t* s, const char* n, mb_value_t* val) {
	/* Get the value of an identifier */
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_ls_node_t* v = 0;
	_object_t* obj = 0;
	mb_value_t tmp;

	mb_assert(s && n);

	running = s->running_context;

	v = _search_identifier_in_scope_chain(s, 0, n, 0, 0);
	if(v) {
		obj = (_object_t*)v->data;
		mb_assert(obj->type == _DT_VAR);
		if(val)
			result = _internal_object_to_public_value(obj->data.variable->data, val);
		else
			result = _internal_object_to_public_value(obj->data.variable->data, &tmp);
	} else {
		if(val) {
			mb_make_nil(*val);
		}
		result = MB_DEBUG_ID_NOT_FOUND;
	}

	return result;
}

int mb_debug_set(struct mb_interpreter_t* s, const char* n, mb_value_t val) {
	/* Set the value of an identifier */
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_ls_node_t* v = 0;
	_object_t* obj = 0;

	mb_assert(s && n);

	running = s->running_context;

	v = _search_identifier_in_scope_chain(s, 0, n, 0, 0);
	if(v) {
		obj = (_object_t*)v->data;
		mb_assert(obj->type == _DT_VAR);
		result = _public_value_to_internal_object(&val, obj->data.variable->data);
	} else {
		result = MB_DEBUG_ID_NOT_FOUND;
	}

	return result;
}

int mb_debug_get_stack_trace(struct mb_interpreter_t* s, void** l, char** fs, unsigned fc) {
	/* Get stack frame names of an interpreter instance */
#ifdef MB_ENABLE_STACK_TRACE
	int result = MB_FUNC_OK;
	_ls_node_t* f = 0;
	unsigned i = 0;
	mb_unrefvar(l);

	mb_assert(s);

	if(fs && fc) {
		f = s->stack_frames->prev;
		while(f && f->data && i < fc) {
			fs[i++] = (char*)f->data;
			f = f->prev;
		}
	}
	while(i < fc)
		fs[i++] = 0;

	return result;
#else /* MB_ENABLE_STACK_TRACE */
	int result = MB_FUNC_OK;
	mb_unrefvar(fs);
	mb_unrefvar(fc);

	_handle_error_on_obj(s, SE_RN_STACK_TRACE_DISABLED, 0, TON(l), MB_FUNC_ERR, _exit, result);

_exit:
	return result;
#endif /* MB_ENABLE_STACK_TRACE */
}

int mb_debug_set_stepped_handler(struct mb_interpreter_t* s, mb_debug_stepped_handler_t h) {
	/* Set a stepped handler to an interpreter instance */
	int result = MB_FUNC_OK;

	mb_assert(s);

	s->debug_stepped_handler = h;

	return result;
}

const char* mb_get_type_string(mb_data_e t) {
	/* Get type description text */
	switch(t) {
	case MB_DT_NIL:
		return "NIL";
	case MB_DT_TYPE:
		return "TYPE";
	case MB_DT_INT:
		return "INTEGER";
	case MB_DT_REAL:
		return "REAL";
	case MB_DT_NUM:
		return "NUMBER";
	case MB_DT_STRING:
		return "STRING";
	case MB_DT_USERTYPE:
		return "USERTYPE";
	case MB_DT_USERTYPE_REF:
		return "USERTYPE_REF";
	case MB_DT_ARRAY:
		return "ARRAY";
#ifdef MB_ENABLE_COLLECTION_LIB
	case MB_DT_LIST:
		return "LIST";
	case MB_DT_LIST_IT:
		return "LIST_ITERATOR";
	case MB_DT_DICT:
		return "DICT";
	case MB_DT_DICT_IT:
		return "DICT_ITERATOR";
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case MB_DT_CLASS:
		return "CLASS";
#endif /* MB_ENABLE_CLASS */
	case MB_DT_ROUTINE:
		return "ROUTINE";
	case MB_DT_UNKNOWN: /* Fall through */
	default:
		return "UNKNOWN";
	}
}

int mb_raise_error(struct mb_interpreter_t* s, void** l, mb_error_e err, int ret) {
	/* Raise an error */
	int result = MB_FUNC_ERR;

	mb_assert(s);

	_handle_error_on_obj(s, err, 0, TON(l), ret, _exit, result);

_exit:
	return result;
}

mb_error_e mb_get_last_error(struct mb_interpreter_t* s) {
	/* Get last error information */
	mb_error_e result = SE_NO_ERR;

	mb_assert(s);

	result = s->last_error;
	s->last_error = SE_NO_ERR;

	return result;
}

const char* mb_get_error_desc(mb_error_e err) {
	/* Get error description text */
	if(err < _countof(_ERR_DESC))
		return _ERR_DESC[err];

	return 0;
}

int mb_set_error_handler(struct mb_interpreter_t* s, mb_error_handler_t h) {
	/* Set an error handler to an interpreter instance */
	int result = MB_FUNC_OK;

	mb_assert(s);

	s->error_handler = h;

	return result;
}

int mb_set_printer(struct mb_interpreter_t* s, mb_print_func_t p) {
	/* Set a print functor to an interpreter instance */
	int result = MB_FUNC_OK;

	mb_assert(s);

	s->printer = p;

	return result;
}

int mb_set_inputer(struct mb_interpreter_t* s, mb_input_func_t p) {
	/* Set an input functor to an interpreter instance */
	int result = MB_FUNC_OK;

	mb_assert(s);

	s->inputer = p;

	return result;
}

int mb_get_userdata(struct mb_interpreter_t* s, void** d) {
	/* Get the userdata of an interpreter instance */
	int result = MB_FUNC_OK;

	mb_assert(s && d);

	if(s && d)
		*d = s->userdata;

	return result;
}

int mb_set_userdata(struct mb_interpreter_t* s, void* d) {
	/* Set the userdata of an interpreter instance */
	int result = MB_FUNC_OK;

	mb_assert(s);

	if(s)
		s->userdata = d;

	return result;
}

int mb_set_import_handler(struct mb_interpreter_t* s, mb_import_handler_t h) {
	/* Set an import handler to an interpreter instance */
	int result = MB_FUNC_OK;

	mb_assert(s);

	s->import_handler = h;

	return result;
}

int mb_gets(char* buf, int s) {
	/* Safe stdin reader function */
	int result = 0;

	if(fgets(buf, s, stdin) == 0) {
		fprintf(stderr, "Error reading.\n");
		exit(1);
	}
	result = (int)strlen(buf);
	if(buf[result - 1] == '\n')
		buf[result - 1] = '\0';

	return result;
}

char* mb_memdup(const char* val, unsigned size) {
	/* Duplicate a string for internal use */
	char* result = 0;

	if(val != 0) {
		result = (char*)mb_malloc(size);
		memcpy(result, val, size);
	}

	return result;
}

int mb_set_memory_manager(mb_memory_allocate_func_t a, mb_memory_free_func_t f) {
	/* Register an allocator and a freer globally */
	_mb_allocate_func = a;
	_mb_free_func = f;

	return 0;
}

/* ========================================================} */

/*
** {========================================================
** Lib definitions
*/

/** Core lib */
int _core_dummy_assign(mb_interpreter_t* s, void** l) {
	/* Operator #, dummy assignment */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_add(mb_interpreter_t* s, void** l) {
	/* Operator + */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_connect_strings(l);
		} else {
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}
	} else {
		_instruct_num_op_num(+, l);
	}

_exit:
	return result;
}

int _core_min(mb_interpreter_t* s, void** l) {
	/* Operator - */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_instruct_num_op_num(-, l);

	return result;
}

int _core_mul(mb_interpreter_t* s, void** l) {
	/* Operator * */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_instruct_num_op_num(*, l);

	return result;
}

int _core_div(mb_interpreter_t* s, void** l) {
	/* Operator / */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_proc_div_by_zero(s, l, _exit, result, SE_RN_DIVIDE_BY_ZERO);
	_instruct_num_op_num(/, l);

_exit:
	return result;
}

int _core_mod(mb_interpreter_t* s, void** l) {
	/* Operator MOD */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_proc_div_by_zero(s, l, _exit, result, SE_RN_MOD_BY_ZERO);
	_instruct_int_op_int(%, l);

_exit:
	return result;
}

int _core_pow(mb_interpreter_t* s, void** l) {
	/* Operator ^ */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_instruct_fun_num_num(pow, l);

	return result;
}

int _core_open_bracket(mb_interpreter_t* s, void** l) {
	/* Operator ( */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_close_bracket(mb_interpreter_t* s, void** l) {
	/* Operator ) */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_neg(mb_interpreter_t* s, void** l) {
	/* Operator - (negative) */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	_running_context_t* running = 0;
	int* inep = 0;
	int calc_depth = 0;

	mb_assert(s && l);

	running = s->running_context;

	if(!_ls_empty(s->in_neg_expr))
		inep = (int*)_ls_back(s->in_neg_expr)->data;

	if(inep)
		(*inep)++;

	calc_depth = running->calc_depth;
	running->calc_depth = 1;

	mb_make_nil(arg);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_func_end(s, l));

	if(inep)
		(*inep)--;

	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = -(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.float_point = -(arg.value.float_point);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);

		break;
	}

	mb_check(mb_push_value(s, l, arg));

_exit:
	running->calc_depth = calc_depth;

	return result;
}

int _core_equal(mb_interpreter_t* s, void** l) {
	/* Operator = */
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(==, l);
		} else {
			_set_tuple3_result(l, 0);
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(==, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(==, l);
	}

_exit:
	return result;
}

int _core_less(mb_interpreter_t* s, void** l) {
	/* Operator < */
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(<, l);
		} else {
			if(_is_string(((_tuple3_t*)*l)->e1)) {
				_set_tuple3_result(l, 0);
			} else {
				_set_tuple3_result(l, 1);
			}
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(<, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(<, l);
	}

_exit:
	return result;
}

int _core_greater(mb_interpreter_t* s, void** l) {
	/* Operator > */
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(>, l);
		} else {
			if(_is_string(((_tuple3_t*)*l)->e1)) {
				_set_tuple3_result(l, 1);
			} else {
				_set_tuple3_result(l, 0);
			}
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(>, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(>, l);
	}

_exit:
	return result;
}

int _core_less_equal(mb_interpreter_t* s, void** l) {
	/* Operator <= */
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(<=, l);
		} else {
			if(_is_string(((_tuple3_t*)*l)->e1)) {
				_set_tuple3_result(l, 0);
			} else {
				_set_tuple3_result(l, 1);
			}
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(<=, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(<=, l);
	}

_exit:
	return result;
}

int _core_greater_equal(mb_interpreter_t* s, void** l) {
	/* Operator >= */
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(>=, l);
		} else {
			if(_is_string(((_tuple3_t*)*l)->e1)) {
				_set_tuple3_result(l, 1);
			} else {
				_set_tuple3_result(l, 0);
			}
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(>=, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(>=, l);
	}

_exit:
	return result;
}

int _core_not_equal(mb_interpreter_t* s, void** l) {
	/* Operator <> */
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(!=, l);
		} else {
			_set_tuple3_result(l, 1);
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(!=, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(!=, l);
	}

_exit:
	return result;
}

int _core_and(mb_interpreter_t* s, void** l) {
	/* Operator AND */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_instruct_bool_op_bool(&&, l);

	return result;
}

int _core_or(mb_interpreter_t* s, void** l) {
	/* Operator OR */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_instruct_bool_op_bool(||, l);

	return result;
}

int _core_not(mb_interpreter_t* s, void** l) {
	/* Operator NOT */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	_running_context_t* running = 0;
	int calc_depth = 0;

	mb_assert(s && l);

	running = s->running_context;

	calc_depth = running->calc_depth;
	running->calc_depth = 1;

	mb_make_nil(arg);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_func_end(s, l));

	switch(arg.type) {
	case MB_DT_NIL:
		arg.value.integer = true;
		arg.type = MB_DT_INT;

		break;
	case MB_DT_INT:
		arg.value.integer = (int_t)(!arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.integer = (int_t)(!((int_t)arg.value.float_point));
		arg.type = MB_DT_INT;

		break;
	default:
		arg.value.integer = false;
		arg.type = MB_DT_INT;

		break;
	}
	mb_check(mb_push_int(s, l, arg.value.integer));

	running->calc_depth = calc_depth;

	return result;
}

int _core_is(mb_interpreter_t* s, void** l) {
	/* Operator IS */
	int result = MB_FUNC_OK;
	_object_t* fst = 0;
	_object_t* scd = 0;
	_object_t* val = 0;
	bool_t is_a = 0;

	mb_assert(s && l);

	fst = (_object_t*)((_tuple3_t*)*l)->e1;
	scd = (_object_t*)((_tuple3_t*)*l)->e2;
	val = (_object_t*)((_tuple3_t*)*l)->e3;

	if(!fst || !scd) {
		_handle_error_on_obj(s, SE_RN_SYNTAX, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}
	if(scd->type == _DT_TYPE) {
		val->type = _DT_INT;
		if((fst->type == _DT_INT || fst->type == _DT_REAL) && scd->data.type == MB_DT_NUM)
			val->data.integer = 1;
		else
			val->data.integer = (int_t)(_internal_type_to_public_type(fst->type) == scd->data.type);
	} else {
#ifdef MB_ENABLE_CLASS
		if(!_IS_CLASS(fst) || !_IS_CLASS(scd)) {
			_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}
		_traverse_class(fst->data.instance, 0, _is_class, _META_LIST_MAX_DEPTH, true, scd->data.instance, &is_a);
		val->type = _DT_INT;
		val->data.integer = (int_t)is_a;
#else /* MB_ENABLE_CLASS */
		mb_unrefvar(is_a);
		_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_CLASS */
	}

_exit:
	return result;
}

int _core_let(mb_interpreter_t* s, void** l) {
	/* LET statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_running_context_t* running = 0;
	_var_t* var = 0;
	_array_t* arr = 0;
	_object_t* arr_obj = 0;
	unsigned int arr_idx = 0;
	bool_t literally = false;
	_object_t* val = 0;
#ifdef MB_ENABLE_COLLECTION_LIB
	int_t idx = 0;
	mb_value_t key;
	bool_t is_coll = false;
#endif /* MB_ENABLE_COLLECTION_LIB */

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	obj = (_object_t*)ast->data;
	if(obj->type == _DT_FUNC)
		ast = ast->next;
	if(!ast || !ast->data) {
		_handle_error_on_obj(s, SE_RN_SYNTAX, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	obj = (_object_t*)ast->data;
	if(obj->type == _DT_ARRAY) {
		arr_obj = obj;
		arr = _search_array_in_scope_chain(s, obj->data.array, &arr_obj);
		result = _get_array_index(s, &ast, 0, &arr_idx, &literally);
		if(result != MB_FUNC_OK)
			goto _exit;
	} else if(obj->type == _DT_VAR && obj->data.variable->data->type == _DT_ARRAY) {
		arr_obj = obj->data.variable->data;
		arr = _search_array_in_scope_chain(s, obj->data.variable->data->data.array, &arr_obj);
		result = _get_array_index(s, &ast, 0, &arr_idx, &literally);
		if(result != MB_FUNC_OK)
			goto _exit;
	} else if(obj->type == _DT_VAR) {
		var = _search_var_in_scope_chain(s, obj->data.variable);
	} else {
		_handle_error_on_obj(s, SE_RN_VAR_OR_ARRAY_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	ast = ast->next;
	if(!ast || !ast->data) {
		_handle_error_on_obj(s, SE_RN_SYNTAX, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
#ifdef MB_ENABLE_COLLECTION_LIB
	if(var && _IS_COLL(var->data)) {
		obj = (_object_t*)ast->data;
		if(_IS_FUNC(obj, _core_open_bracket)) {
			mb_check(mb_attempt_open_bracket(s, l));

			switch(var->data->type) {
			case _DT_LIST:
				mb_check(mb_pop_int(s, l, &idx));

				break;
			case _DT_DICT:
				mb_make_nil(key);
				mb_check(mb_pop_value(s, l, &key));

				break;
			default: /* Do nothing */
				break;
			}

			mb_check(mb_attempt_close_bracket(s, l));
		}
		ast = (_ls_node_t*)*l;
		is_coll = true;
	}
#endif /* MB_ENABLE_COLLECTION_LIB */
	obj = (_object_t*)ast->data;
	if(obj->type != _DT_FUNC || strcmp(obj->data.func->name, "=") != 0) {
		_handle_error_on_obj(s, SE_RN_ASSIGN_OPERATOR_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	ast = ast->next;
	val = _create_object();
	result = _calc_expression(s, &ast, &val);

	if(var) {
		if(val->type != _DT_UNKNOWN) {
#ifdef MB_ENABLE_COLLECTION_LIB
			if(is_coll) {
				switch(var->data->type) {
				case _DT_LIST:
					if(!_set_list(var->data->data.list, idx, 0, &val)) {
						_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
					}

					break;
				case _DT_DICT:
					_set_dict(var->data->data.dict, &key, 0, 0, val);

					break;
				default: /* Do nothing */
					break;
				}

				goto _exit;
			}
#endif /* MB_ENABLE_COLLECTION_LIB */
			_dispose_object(var->data);
			var->data->type = val->type;
#ifdef MB_ENABLE_COLLECTION_LIB
			if(val->type == _DT_LIST_IT || val->type == _DT_DICT_IT)
				_assign_with_it(var->data, val);
			else
				var->data->data = val->data;
#else /* MB_ENABLE_COLLECTION_LIB */
			var->data->data = val->data;
#endif /* MB_ENABLE_COLLECTION_LIB */
			if(val->type == _DT_ROUTINE) {
#ifdef MB_ENABLE_LAMBDA
				var->data->ref = val->ref;
#else /* MB_ENABLE_LAMBDA */
				var->data->ref = 1;
#endif /* MB_ENABLE_LAMBDA */
			} else {
				var->data->ref = val->ref;
			}
		}
	} else if(arr && literally) {
		if(val->type != _DT_UNKNOWN) {
			_unref(&arr_obj->data.array->ref, arr_obj->data.array);
			arr_obj->type = val->type;
#ifdef MB_ENABLE_COLLECTION_LIB
			if(val->type == _DT_LIST_IT || val->type == _DT_DICT_IT)
				_assign_with_it(arr_obj, val);
			else
				arr_obj->data = val->data;
#else /* MB_ENABLE_COLLECTION_LIB */
			arr_obj->data = val->data;
#endif /* MB_ENABLE_COLLECTION_LIB */
			arr_obj->ref = val->ref;
		}
	} else if(arr) {
		mb_value_u _val;
		switch(val->type) {
		case _DT_NIL: /* Fall through */
		case _DT_INT: /* Fall through */
		case _DT_REAL: /* Fall through */
		case _DT_STRING: /* Fall through */
		case _DT_USERTYPE:
			_copy_bytes(_val.bytes, val->data.bytes);

			break;
		default:
			safe_free(val);
			_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

			break;
		}
		result = _set_array_elem(s, ast, arr, arr_idx, &_val, &val->type);
		if(result != MB_FUNC_OK)
			goto _exit;
		if(val->type == _DT_STRING && !val->ref) {
			safe_free(val->data.string);
		}
	}
	_REF(val)
	safe_free(val);

_exit:
	*l = ast;

	return result;
}

int _core_dim(mb_interpreter_t* s, void** l) {
	/* DIM statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* arr = 0;
	_object_t* len = 0;
	mb_value_u val;
	_array_t dummy;

	mb_assert(s && l);

	/* Array name */
	ast = (_ls_node_t*)*l;
	if(!ast->next || ((_object_t*)ast->next->data)->type != _DT_ARRAY) {
		_handle_error_on_obj(s, SE_RN_ARRAY_IDENTIFIER_EXPECTED, 0, (ast && ast->next) ? ((_object_t*)ast->next->data) : 0, MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	arr = (_object_t*)ast->data;
	memset(&dummy, 0, sizeof(_array_t));
	dummy.type = arr->data.array->type;
	dummy.name = arr->data.array->name;
	/* ( */
	if(!ast->next || ((_object_t*)ast->next->data)->type != _DT_FUNC || ((_object_t*)ast->next->data)->data.func->pointer != _core_open_bracket) {
		_handle_error_on_obj(s, SE_RN_OPEN_BRACKET_EXPECTED, 0, (ast && ast->next) ? ((_object_t*)ast->next->data) : 0, MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	/* Array subscript */
	if(!ast->next) {
		_handle_error_on_obj(s, SE_RN_ARRAY_SUBSCRIPT_EXPECTED, 0, (ast && ast->next) ? ((_object_t*)ast->next->data) : 0, MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	while(((_object_t*)ast->data)->type != _DT_FUNC || ((_object_t*)ast->data)->data.func->pointer != _core_close_bracket) {
		/* Get an integer value */
		len = (_object_t*)ast->data;
		if(!_try_get_value(len, &val, _DT_INT)) {
			_handle_error_on_obj(s, SE_RN_TYPE_NOT_MATCH, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(val.integer <= 0) {
			_handle_error_on_obj(s, SE_RN_ILLEGAL_BOUND, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(dummy.dimension_count >= MB_MAX_DIMENSION_COUNT) {
			_handle_error_on_obj(s, SE_RN_DIMENSION_TOO_MUCH, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		dummy.dimensions[dummy.dimension_count++] = (int)val.integer;
		if(dummy.count)
			dummy.count *= (unsigned int)val.integer;
		else
			dummy.count += (unsigned int)val.integer;
		ast = ast->next;
		/* Comma? */
		if(_IS_SEP(ast->data, ','))
			ast = ast->next;
	}
	/* Create or modify raw data */
	_clear_array(arr->data.array);
	dummy.ref = arr->data.array->ref;
	*(arr->data.array) = dummy;
	_init_array(arr->data.array);
	if(!arr->data.array->raw) {
		arr->data.array->dimension_count = 0;
		arr->data.array->dimensions[0] = 0;
		arr->data.array->count = 0;
		_handle_error_on_obj(s, SE_RN_OUT_OF_MEMORY, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}

_exit:
	*l = ast;

	return result;
}

int _core_if(mb_interpreter_t* s, void** l) {
	/* IF statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* val = 0;
	_object_t* obj = 0;
	bool_t multi_line = false;
	_running_context_t* running = 0;

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	val = _create_object();

_elseif:
	_MAKE_NIL(val);
	result = _calc_expression(s, &ast, &val);
	if(result != MB_FUNC_OK)
		goto _exit;

	obj = (_object_t*)ast->data;
	if(val->data.integer) {
		if(!_IS_FUNC(obj, _core_then)) {
			_handle_error_on_obj(s, SE_RN_INTEGER_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}

		if(ast && ast->next && _IS_EOS(ast->next->data))
			multi_line = true;
		else
			s->skip_to_eoi = _ls_back(s->sub_stack);
		do {
			ast = ast->next;
			while(ast && _IS_EOS(ast->data))
				ast = ast->next;
			result = _execute_statement(s, &ast);
			if(result != MB_FUNC_OK)
				goto _exit;
			if(ast)
				ast = ast->prev;
		} while(ast && (
				(!multi_line && _IS_SEP(ast->data, ':')) || (
					multi_line && ast->next && (
						!_IS_FUNC(ast->next->data, _core_elseif) &&
						!_IS_FUNC(ast->next->data, _core_else) &&
						!_IS_FUNC(ast->next->data, _core_endif)
					)
				)
			)
		);

		if(!ast)
			goto _exit;

		obj = (_object_t*)ast->data;
		if(obj->type != _DT_EOS) {
			s->skip_to_eoi = 0;
			result = _skip_to(s, &ast, 0, _DT_EOS);
			if(result != MB_FUNC_OK)
				goto _exit;
		}
	} else {
		if(ast && ast->next && _IS_EOS(ast->next->data)) {
			multi_line = true;

			_skip_if_chunk(s, &ast);
		}
		if(multi_line && ast && _IS_FUNC(ast->data, _core_elseif)) {
			ast = ast->next;

			goto _elseif;
		}
		if(multi_line && ast && _IS_FUNC(ast->data, _core_endif))
			goto _exit;

		result = _skip_to(s, &ast, _core_else, _DT_EOS);
		if(result != MB_FUNC_OK)
			goto _exit;

		obj = (_object_t*)ast->data;
		if(obj->type != _DT_EOS) {
			if(!_IS_FUNC(obj, _core_else)) {
				_handle_error_on_obj(s, SE_RN_ELSE_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
			}

			do {
				ast = ast->next;
				while(_IS_EOS(ast->data))
					ast = ast->next;
				result = _execute_statement(s, &ast);
				if(result != MB_FUNC_OK)
					goto _exit;
				if(ast)
					ast = ast->prev;
			} while(ast && (
					(!multi_line && _IS_SEP(ast->data, ':')) ||
					(multi_line && !_IS_FUNC(ast->next->data, _core_endif))
				)
			);
		}
	}

_exit:
	if(result == MB_SUB_RETURN) {
		ast = ast->prev;
	} else {
		if(multi_line)
			result = _skip_to(s, &ast, _core_endif, _DT_NIL);
	}

	*l = ast;

	if(val->type != _DT_UNKNOWN)
		_destroy_object(val, 0);

	return result;
}

int _core_then(mb_interpreter_t* s, void** l) {
	/* THEN statement */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_elseif(mb_interpreter_t* s, void** l) {
	/* ELSEIF statement */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_else(mb_interpreter_t* s, void** l) {
	/* ELSE statement */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_endif(mb_interpreter_t* s, void** l) {
	/* ENDIF statement */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_for(mb_interpreter_t* s, void** l) {
	/* FOR statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* to_node = 0;
	_object_t* obj = 0;
	_object_t to_val;
	_object_t step_val;
	_object_t* to_val_ptr = 0;
	_object_t* step_val_ptr = 0;
	_var_t* var_loop = 0;
	_tuple3_t ass_tuple3;
	_tuple3_t* ass_tuple3_ptr = 0;
	_running_context_t* running = 0;

	mb_assert(s && l);

	running = s->running_context;
	ast = (_ls_node_t*)*l;
	ast = ast->next;

	to_val_ptr = &to_val;
	_MAKE_NIL(to_val_ptr);
	step_val_ptr = &step_val;
	_MAKE_NIL(step_val_ptr);
	ass_tuple3_ptr = &ass_tuple3;

	obj = (_object_t*)ast->data;
	if(obj->type != _DT_VAR) {
		_handle_error_on_obj(s, SE_RN_LOOP_VAR_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	var_loop = obj->data.variable;

	result = _execute_statement(s, &ast);
	if(result != MB_FUNC_OK)
		goto _exit;
	ast = ast->prev;

	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_to)) {
		_handle_error_on_obj(s, SE_RN_TO_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	ast = ast->next;
	if(!ast) {
		_handle_error_on_obj(s, SE_RN_SYNTAX, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	to_node = ast;

_to:
	ast = to_node;

	result = _calc_expression(s, &ast, &to_val_ptr);
	if(result != MB_FUNC_OK)
		goto _exit;

	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_step)) {
		step_val = _OBJ_INT_UNIT;
	} else {
		ast = ast->next;
		if(!ast) {
			_handle_error_on_obj(s, SE_RN_SYNTAX, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}

		result = _calc_expression(s, &ast, &step_val_ptr);
		if(result != MB_FUNC_OK)
			goto _exit;
	}

	if((_compare_numbers(step_val_ptr, &_OBJ_INT_ZERO) == 1 && _compare_numbers(var_loop->data, to_val_ptr) == 1) ||
		(_compare_numbers(step_val_ptr, &_OBJ_INT_ZERO) == -1 && _compare_numbers(var_loop->data, to_val_ptr) == -1)) {
		/* End looping */
		if(_skip_struct(s, &ast, _core_for, _core_next) != MB_FUNC_OK)
			goto _exit;
		_skip_to(s, &ast, 0, _DT_EOS);

		goto _exit;
	} else {
		/* Keep looping */
		obj = (_object_t*)ast->data;
		while(!_IS_FUNC(obj, _core_next)) {
			result = _execute_statement(s, &ast);
			if(result == MB_LOOP_CONTINUE) { /* NEXT */
				if(!running->next_loop_var || running->next_loop_var == var_loop) { /* This loop */
					running->next_loop_var = 0;
					result = MB_FUNC_OK;

					break;
				} else { /* Not this loop */
					if(_skip_struct(s, &ast, _core_for, _core_next) != MB_FUNC_OK)
						goto _exit;
					_skip_to(s, &ast, 0, _DT_EOS);

					goto _exit;
				}
			} else if(result == MB_LOOP_BREAK) { /* EXIT */
				if(_skip_struct(s, &ast, _core_for, _core_next) != MB_FUNC_OK)
					goto _exit;
				_skip_to(s, &ast, 0, _DT_EOS);
				result = MB_FUNC_OK;

				goto _exit;
			} else if(result != MB_FUNC_OK && result != MB_SUB_RETURN) { /* Normally */
				goto _exit;
			}

			if(!ast) {
				_handle_error_on_obj(s, SE_RN_NEXT_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
			}
			obj = (_object_t*)ast->data;
		}

		ass_tuple3.e1 = var_loop->data;
		ass_tuple3.e2 = step_val_ptr;
		ass_tuple3.e3 = var_loop->data;
		_instruct_num_op_num(+, &ass_tuple3_ptr);

		goto _to;
	}

_exit:
	*l = ast;

	return result;
}

int _core_to(mb_interpreter_t* s, void** l) {
	/* TO statement */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_step(mb_interpreter_t* s, void** l) {
	/* STEP statement */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_next(mb_interpreter_t* s, void** l) {
	/* NEXT statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_running_context_t* running = 0;

	mb_assert(s && l);

	running = s->running_context;
	ast = (_ls_node_t*)*l;

	result = MB_LOOP_CONTINUE;

	ast = ast->next;
	if(ast && ((_object_t*)ast->data)->type == _DT_VAR) {
		obj = (_object_t*)ast->data;
		running->next_loop_var = obj->data.variable;
	}

	*l = ast;

	return result;
}

int _core_while(mb_interpreter_t* s, void** l) {
	/* WHILE statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* loop_begin_node = 0;
	_object_t* obj = 0;
	_object_t loop_cond;
	_object_t* loop_cond_ptr = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	loop_cond_ptr = &loop_cond;
	_MAKE_NIL(loop_cond_ptr);

	loop_begin_node = ast;

_loop_begin:
	ast = loop_begin_node;

	result = _calc_expression(s, &ast, &loop_cond_ptr);
	if(result != MB_FUNC_OK)
		goto _exit;

	if(loop_cond_ptr->data.integer) {
		/* Keep looping */
		obj = (_object_t*)ast->data;
		while(!_IS_FUNC(obj, _core_wend)) {
			result = _execute_statement(s, &ast);
			if(result == MB_LOOP_BREAK) { /* EXIT */
				if(_skip_struct(s, &ast, _core_while, _core_wend) != MB_FUNC_OK)
					goto _exit;
				_skip_to(s, &ast, 0, _DT_EOS);
				result = MB_FUNC_OK;

				goto _exit;
			} else if(result != MB_FUNC_OK && result != MB_SUB_RETURN) { /* Normally */
				goto _exit;
			}

			obj = (_object_t*)ast->data;
		}

		goto _loop_begin;
	} else {
		/* End looping */
		if(_skip_struct(s, &ast, _core_while, _core_wend) != MB_FUNC_OK)
			goto _exit;
		_skip_to(s, &ast, 0, _DT_EOS);

		goto _exit;
	}

_exit:
	*l = ast;

	return result;
}

int _core_wend(mb_interpreter_t* s, void** l) {
	/* WEND statement */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_do(mb_interpreter_t* s, void** l) {
	/* DO statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* loop_begin_node = 0;
	_object_t* obj = 0;
	_object_t loop_cond;
	_object_t* loop_cond_ptr = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	obj = (_object_t*)ast->data;
	if(!_IS_EOS(obj)) {
		_handle_error_on_obj(s, SE_RN_SYNTAX, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;

	loop_cond_ptr = &loop_cond;
	_MAKE_NIL(loop_cond_ptr);

	loop_begin_node = ast;

_loop_begin:
	ast = loop_begin_node;

	obj = (_object_t*)ast->data;
	while(!_IS_FUNC(obj, _core_until)) {
		result = _execute_statement(s, &ast);
		if(result == MB_LOOP_BREAK) { /* EXIT */
			if(_skip_struct(s, &ast, _core_do, _core_until) != MB_FUNC_OK)
				goto _exit;
			_skip_to(s, &ast, 0, _DT_EOS);
			result = MB_FUNC_OK;

			goto _exit;
		} else if(result != MB_FUNC_OK && result != MB_SUB_RETURN) { /* Normally */
			goto _exit;
		}

		obj = (_object_t*)ast->data;
	}

	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_until)) {
		_handle_error_on_obj(s, SE_RN_UNTIL_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;

	result = _calc_expression(s, &ast, &loop_cond_ptr);
	if(result != MB_FUNC_OK)
		goto _exit;

	if(loop_cond_ptr->data.integer) {
		/* End looping */
		_skip_to(s, &ast, 0, _DT_EOS);

		goto _exit;
	} else {
		/* Keep looping */
		goto _loop_begin;
	}

_exit:
	*l = ast;

	return result;
}

int _core_until(mb_interpreter_t* s, void** l) {
	/* UNTIL statement */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_exit(mb_interpreter_t* s, void** l) {
	/* EXIT statement */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	result = MB_LOOP_BREAK;

	return result;
}

int _core_goto(mb_interpreter_t* s, void** l) {
	/* GOTO statement */
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_label_t* label = 0;
	_ls_node_t* glbsyminscope = 0;

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	_using_jump_set_of_instructional(s, ast, _exit, result);

	obj = (_object_t*)ast->data;
	if(obj->type != _DT_LABEL) {
		_handle_error_on_obj(s, SE_RN_JUMP_LABEL_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	label = (_label_t*)obj->data.label;
	if(!label->node) {
		glbsyminscope = _ht_find(running->var_dict, label->name);
		if(!(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_LABEL)) {
			_handle_error_on_obj(s, SE_RN_LABEL_NOT_EXISTS, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		label->node = ((_object_t*)glbsyminscope->data)->data.label->node;
	}

	mb_assert(label->node && label->node->prev);
	ast = label->node->prev;
	if(ast && !ast->data)
		ast = ast->next;

_exit:
	*l = ast;

	return result;
}

int _core_gosub(mb_interpreter_t* s, void** l) {
	/* GOSUB statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;

	mb_assert(s && l);

	running = s->running_context;
	ast = (_ls_node_t*)*l;

	_using_jump_set_of_instructional(s, ast, _exit, result);

	result = _core_goto(s, l);
	if(result == MB_FUNC_OK)
		_ls_pushback(s->sub_stack, ast);

_exit:
	return result;
}

int _core_return(mb_interpreter_t* s, void** l) {
	/* RETURN statement */
	int result = MB_SUB_RETURN;
	_ls_node_t* ast = 0;
	_ls_node_t* sub_stack = 0;
	_running_context_t* running = 0;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	running = s->running_context;
	sub_stack = s->sub_stack;

	if(running->prev) {
		ast = (_ls_node_t*)*l;
		ast = ast->next;
		if(mb_has_arg(s, (void**)&ast)) {
			mb_check(mb_pop_value(s, (void**)&ast, &arg));
			mb_check(mb_push_value(s, (void**)&ast, arg));

			if(arg.type == MB_DT_STRING) {
				_ls_foreach(s->temp_values, _destroy_object_capsule_only);
				_ls_clear(s->temp_values);
			}
		}
	}
	ast = (_ls_node_t*)_ls_popback(sub_stack);
	if(!ast) {
		_handle_error_on_obj(s, SE_RN_NO_RETURN_POINT, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	*l = ast;

_exit:
	return result;
}

int _core_call(mb_interpreter_t* s, void** l) {
	/* CALL statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_routine_t* routine = 0;
	mb_value_t ret;
	_ls_node_t* pathed = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	obj = (_object_t*)ast->data;

_retry:
	switch(obj->type) {
	case _DT_FUNC:
		if(_IS_FUNC(obj, _core_open_bracket)) {
			mb_check(mb_attempt_open_bracket(s, l));

			ast = ast->next;
			obj = (_object_t*)ast->data;
#ifdef MB_ENABLE_CLASS
			if(obj->type == _DT_VAR) {
				pathed = _search_identifier_in_scope_chain(s, 0, obj->data.variable->name, obj->data.variable->pathing, 0);
				if(pathed && pathed->data)
					obj = (_object_t*)pathed->data;
			}
#endif /* MB_ENABLE_CLASS */
			if(!obj || obj->type != _DT_ROUTINE) {
				_handle_error_on_obj(s, SE_RN_ROUTINE_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
			}
			ret.type = MB_DT_ROUTINE;
			ret.value.routine = obj->data.routine;
			ast = ast->next;
			*l = ast;

			mb_check(mb_attempt_close_bracket(s, l));

			mb_check(mb_push_value(s, l, ret));
		}

		break;
	case _DT_VAR:
		if(obj->data.variable->data->type == _DT_ROUTINE) {
			obj = obj->data.variable->data;

			goto _retry;
		}
#ifdef MB_ENABLE_CLASS
		pathed = _search_identifier_in_scope_chain(s, 0, obj->data.variable->name, obj->data.variable->pathing, 0);
		if(pathed && pathed->data)
			obj = (_object_t*)pathed->data;
		/* Fall through */
#else /* MB_ENABLE_CLASS */
		mb_unrefvar(pathed);
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		routine = (_routine_t*)obj->data.routine;
		result = _eval_routine(s, &ast, 0, 0, routine, _has_routine_lex_arg, _pop_routine_lex_arg);
		if(ast)
			ast = ast->prev;

		break;
	default: /* Do nothing */
		break;
	}

	*l = ast;

_exit:
	return result;
}

int _core_def(mb_interpreter_t* s, void** l) {
	/* DEF statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_object_t* obj = 0;
	_var_t* var = 0;
	_ls_node_t*  rnode = 0;
	_routine_t* routine = 0;

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	_using_jump_set_of_structured(s, ast, _exit, result);

	obj = (_object_t*)ast->data;
	if(!_IS_ROUTINE(obj)) {
		_handle_error_on_obj(s, SE_RN_ROUTINE_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	if(obj->data.routine->func.basic.entry) {
		_handle_error_on_obj(s, SE_RN_DUPLICATE_ROUTINE, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	routine = (_routine_t*)((_object_t*)ast->data)->data.routine;
	ast = ast->next;
	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_open_bracket)) {
		_handle_error_on_obj(s, SE_RN_OPEN_BRACKET_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	obj = (_object_t*)ast->data;
	while(!_IS_FUNC(obj, _core_close_bracket)) {
		if(obj->type == _DT_VAR) {
			var = obj->data.variable;
			rnode = _search_identifier_in_scope_chain(s, routine->func.basic.scope, var->name, 0, 0);
			if(rnode)
				var = ((_object_t*)rnode->data)->data.variable;
			if(!routine->func.basic.parameters)
				routine->func.basic.parameters = _ls_create();
			_ls_pushback(routine->func.basic.parameters, var);
		}

		ast = ast->next;
		obj = (_object_t*)ast->data;
	}
	ast = ast->next;
	routine->func.basic.entry = ast;

	_skip_to(s, &ast, _core_enddef, _DT_INVALID);

	ast = ast->next;

_exit:
	*l = ast;

	return result;
}

int _core_enddef(mb_interpreter_t* s, void** l) {
	/* ENDDEF statement */
	int result = MB_SUB_RETURN;
	_ls_node_t* ast = 0;
	_ls_node_t* sub_stack = 0;

	mb_assert(s && l);

	sub_stack = s->sub_stack;

	ast = (_ls_node_t*)_ls_popback(sub_stack);
	if(!ast) {
		_handle_error_on_obj(s, SE_RN_NO_RETURN_POINT, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	*l = ast;

_exit:
	return result;
}

#ifdef MB_ENABLE_CLASS
int _core_class(mb_interpreter_t* s, void** l) {
	/* CLASS statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_object_t* obj = 0;
	_class_t* instance = 0;
	_class_t* inherit = 0;
	_class_t* last_inst = 0;

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	_using_jump_set_of_structured(s, ast, _exit, result);

	obj = (_object_t*)ast->data;
	obj = _GET_CLASS(obj);
	if(!_IS_CLASS(obj)) {
		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	instance = obj->data.instance;
	ast = ast->next;
	obj = (_object_t*)ast->data;

	last_inst = s->last_instance;
	s->last_instance = instance;

	if(_IS_FUNC(obj, _core_open_bracket)) {
		/* Process meta_list */
		do {
			ast = ast->next;
			obj = (_object_t*)ast->data;
			if(obj && obj->type == _DT_VAR) {
				_ls_node_t* tmp =_search_identifier_in_scope_chain(s, _OUTTER_SCOPE(running), obj->data.variable->name, 0, 0);
				if(tmp && tmp->data)
					obj = (_object_t*)tmp->data;
			}
			obj = _GET_CLASS(obj);
			if(!_IS_CLASS(obj)) {
				_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
			}
			inherit = obj->data.instance;
			_link_meta_class(s, instance, inherit);
			ast = ast->next;
			obj = (_object_t*)ast->data;
		} while(_IS_CLASS(obj) || _IS_SEP(obj, ','));
		if(_IS_FUNC(obj, _core_close_bracket)) {
			ast = ast->next;
		} else {
			_handle_error_on_obj(s, SE_RN_CLOSE_BRACKET_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
	}

	*l = ast;

	running = _push_scope_by_class(s, instance->scope);

	do {
		result = _execute_statement(s, (_ls_node_t**)l);
		if(result != MB_FUNC_OK && s->error_handler) {
			if(result >= MB_EXTENDED_ABORT)
				s->last_error = SE_EA_EXTENDED_ABORT;
			_handle_error_now(s, s->last_error, s->last_error_func, result);

			goto _pop_exit;
		}
		ast = (_ls_node_t*)*l;
		if(!ast) break;
		obj = (_object_t*)ast->data;
	} while(ast && !_IS_FUNC(obj, _core_endclass));

	_pop_scope(s);

	if(ast) {
		_skip_to(s, &ast, _core_endclass, _DT_NIL);

		ast = ast->next;
	}

_pop_exit:
	if(result != MB_FUNC_OK)
		_pop_scope(s);

_exit:
	*l = ast;

	s->last_instance = last_inst;

	return result;
}

int _core_endclass(mb_interpreter_t* s, void** l) {
	/* ENDCLASS statement */
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

int _core_new(mb_interpreter_t* s, void** l) {
	/* NEW statement */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	_object_t obj;
	_object_t tgt;
	mb_value_t ret;
	_ls_node_t* cs = 0;
	_object_t* c = 0;

	mb_assert(s && l);

	mb_make_nil(ret);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_func_end(s, l));

	_MAKE_NIL(&obj);
	switch(arg.type) {
	case MB_DT_STRING:
		arg.value.string = mb_strupr(arg.value.string);
		cs = _search_identifier_in_scope_chain(s, 0, arg.value.string, 0, 0);
		if(!cs || !cs->data) goto _default;
		c = (_object_t*)cs->data;
		if(!c) goto _default;
		c = _GET_CLASS(c);
		if(!c) goto _default;
		_internal_object_to_public_value(c, &arg);
		_ref(&c->data.instance->ref, c->data.instance);
		/* Fall through */
	case MB_DT_CLASS:
		_public_value_to_internal_object(&arg, &obj);
		_clone_object(s, &obj, &tgt);
		ret.type = MB_DT_CLASS;
		ret.value.instance = tgt.data.instance;

		break;
	default:
_default:
		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_push_value(s, l, ret));
	_assign_public_value(&ret, 0);

_exit:
	_assign_public_value(&arg, 0);

	return result;
}

int _core_var(mb_interpreter_t* s, void** l) {
	/* VAR statement */
	int result = MB_FUNC_IGNORE;
	_ls_node_t* ast = 0;
	mb_unrefvar(s);

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	if(!s->last_instance) {
		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);
	}

_exit:
	*l = ast;

	return result;
}
#endif /* MB_ENABLE_CLASS */

#ifdef MB_ENABLE_LAMBDA
int _core_lambda(mb_interpreter_t* s, void** l) {
	/* LAMBDA statement */
	int result = MB_FUNC_OK;
	mb_value_t ret;
	bool_t err = false;
	_routine_t* routine = 0;
	_ls_node_t* ast = 0;
	int brackets = 0;

	mb_assert(s && l);

	routine = (_routine_t*)mb_malloc(sizeof(_routine_t));
	_init_routine(s, routine, 0, 0);

	/* Parameter list */
	_mb_check_mark(mb_attempt_open_bracket(s, l), err, _error);

	while(mb_has_arg(s, l)) {
		void* v = 0;
		_mb_check_mark(mb_get_var(s, l, &v), err, _error);

		if(!routine->func.lambda.parameters)
			routine->func.lambda.parameters = _ls_create();
		_ls_pushback(routine->func.lambda.parameters, v);

		ast = (_ls_node_t*)*l;
		if(_IS_FUNC(ast->data, _core_close_bracket))
			break;
		ast = ast->next;
		*l = ast;
	}

	_mb_check_mark(mb_attempt_close_bracket(s, l), err, _error);

	/* Lambda body */
	ast = (_ls_node_t*)*l;
	if(ast) ast = ast->prev;
	*l = ast;
	_mb_check_mark(mb_attempt_open_bracket(s, l), err, _error);

	ast = (_ls_node_t*)*l;
	routine->func.lambda.entry = ast;
	while(ast && (brackets || !_IS_FUNC(ast->data, _core_close_bracket))) {
		if(_IS_FUNC(ast->data, _core_open_bracket))
			brackets++;
		else if(_IS_FUNC(ast->data, _core_close_bracket))
			brackets--;
		ast = ast->next;
	}
	*l = ast;

	_mb_check_mark(mb_attempt_close_bracket(s, l), err, _error);

	/* Return the value */
	ret.type = MB_DT_ROUTINE;
	ret.value.routine = routine;

	_mb_check_mark(mb_push_value(s, l, ret), err, _error);

	/* Error processing */
	while(0) {
_error:
		if(routine)
			_destroy_routine(routine);
	}

	return result;
}
#endif /* MB_ENABLE_LAMBDA */

#ifdef MB_ENABLE_ALLOC_STAT
int _core_mem(mb_interpreter_t* s, void** l) {
	/* MEM statement */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	mb_check(mb_attempt_func_begin(s, l));
	mb_check(mb_attempt_func_end(s, l));

	mb_check(mb_push_int(s, l, (int_t)_mb_allocated));

	return result;
}
#endif /* MB_ENABLE_ALLOC_STAT */

int _core_type(mb_interpreter_t* s, void** l) {
	/* TYPE statement */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	int i = 0;
	unsigned e = 0;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	if(arg.type == MB_DT_STRING) {
		for(i = 0; i < sizeof(mb_data_e) * 8; i++) {
			e = 1 << i;
			if(!mb_stricmp(mb_get_type_string((mb_data_e)e), arg.value.string)) {
				arg.value.type = (mb_data_e)e;
				arg.type = MB_DT_TYPE;

				goto _found;
			}
		}
	}
	arg.value.type = arg.type;
	arg.type = MB_DT_TYPE;

_found:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

int _core_import(mb_interpreter_t* s, void** l) {
	/* IMPORT statement */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_attempt_func_end(s, l));

	return result;
}

int _core_end(mb_interpreter_t* s, void** l) {
	/* END statement */
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	result = MB_FUNC_END;

	return result;
}

/** Standard lib */
int _std_abs(mb_interpreter_t* s, void** l) {
	/* Get the absolute value of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = (int_t)abs(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.float_point = (real_t)fabs(arg.value.float_point);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);

		break;
	}

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_sgn(mb_interpreter_t* s, void** l) {
	/* Get the sign of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = sgn(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.integer = sgn(arg.value.float_point);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	mb_check(mb_push_int(s, l, arg.value.integer));

_exit:
	return result;
}

int _std_sqr(mb_interpreter_t* s, void** l) {
	/* Get the square root of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	_math_calculate_fun_real(s, l, arg, sqrt, _exit, result);

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_floor(mb_interpreter_t* s, void** l) {
	/* Get the greatest integer not greater than a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = (int_t)(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.integer = (int_t)floor(arg.value.float_point);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	mb_check(mb_push_int(s, l, arg.value.integer));

_exit:
	return result;
}

int _std_ceil(mb_interpreter_t* s, void** l) {
	/* Get the least integer not less than a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = (int_t)(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.integer = (int_t)ceil(arg.value.float_point);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	mb_check(mb_push_int(s, l, arg.value.integer));

_exit:
	return result;
}

int _std_fix(mb_interpreter_t* s, void** l) {
	/* Get the integer format of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = (int_t)(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.integer = (int_t)(arg.value.float_point);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	mb_check(mb_push_int(s, l, arg.value.integer));

_exit:
	return result;
}

int _std_round(mb_interpreter_t* s, void** l) {
	/* Get the rounded integer of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = (int_t)(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.integer = (int_t)(arg.value.float_point + (real_t)0.5f);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, 0, TON(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	mb_check(mb_push_int(s, l, arg.value.integer));

_exit:
	return result;
}

int _std_srnd(mb_interpreter_t* s, void** l) {
	/* Set a random seed */
	int result = MB_FUNC_OK;
	int_t seed = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_int(s, l, &seed));

	mb_check(mb_attempt_close_bracket(s, l));

	srand((unsigned int)seed);

	return result;
}

int _std_rnd(mb_interpreter_t* s, void** l) {
	/* Get a random value among 0 ~ 1 or among given bounds */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	real_t rnd = (real_t)0.0f;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;

	if(ast && ast->next && _IS_FUNC(ast->next->data, _core_open_bracket)) {
		int_t lw = 0;
		int_t hg = 0;
		mb_check(mb_attempt_open_bracket(s, l));
		if(mb_has_arg(s, l)) {
			mb_check(mb_pop_int(s, l, &hg));
		}
		if(mb_has_arg(s, l)) {
			lw = hg;
			mb_check(mb_pop_int(s, l, &hg));
		}
		mb_check(mb_attempt_close_bracket(s, l));

		if(lw >= hg) {
			_handle_error_on_obj(s, SE_RN_ILLEGAL_BOUND, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		rnd = (real_t)rand() / RAND_MAX * (hg - lw + (real_t)0.99999f) + lw; /* [low, high] */

		mb_check(mb_push_int(s, l, (int_t)rnd));
	} else {
		mb_check(mb_attempt_func_begin(s, l));
		mb_check(mb_attempt_func_end(s, l));

		rnd = (real_t)(((real_t)(rand() % 101)) / 100.0f); /* [0.0, 1.0] */

		mb_check(mb_push_real(s, l, rnd));
	}

_exit:
	return result;
}

int _std_sin(mb_interpreter_t* s, void** l) {
	/* Get the sin value of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	_math_calculate_fun_real(s, l, arg, sin, _exit, result);

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_cos(mb_interpreter_t* s, void** l) {
	/* Get the cos value of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	_math_calculate_fun_real(s, l, arg, cos, _exit, result);

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_tan(mb_interpreter_t* s, void** l) {
	/* Get the tan value of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	_math_calculate_fun_real(s, l, arg, tan, _exit, result);

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_asin(mb_interpreter_t* s, void** l) {
	/* Get the asin value of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	_math_calculate_fun_real(s, l, arg, asin, _exit, result);

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_acos(mb_interpreter_t* s, void** l) {
	/* Get the acos value of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	_math_calculate_fun_real(s, l, arg, acos, _exit, result);

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_atan(mb_interpreter_t* s, void** l) {
	/* Get the atan value of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	_math_calculate_fun_real(s, l, arg, atan, _exit, result);

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_exp(mb_interpreter_t* s, void** l) {
	/* Get the exp value of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	_math_calculate_fun_real(s, l, arg, exp, _exit, result);

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_log(mb_interpreter_t* s, void** l) {
	/* Get the log value of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	_math_calculate_fun_real(s, l, arg, log, _exit, result);

	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

int _std_asc(mb_interpreter_t* s, void** l) {
	/* Get the ASCII code of a character */
	int result = MB_FUNC_OK;
	char* arg = 0;
	int_t val = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_string(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	if(arg[0] == '\0') {
		result = MB_FUNC_ERR;

		goto _exit;
	}
	memcpy(&val, arg, strlen(arg));
	mb_check(mb_push_int(s, l, val));

_exit:
	return result;
}

int _std_chr(mb_interpreter_t* s, void** l) {
	/* Get the character of an ASCII code */
	int result = MB_FUNC_OK;
	int_t arg = 0;
	char* chr = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_int(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	chr = (char*)mb_malloc(sizeof(arg) + 1);
	memset(chr, 0, sizeof(arg) + 1);
	memcpy(chr, &arg, sizeof(arg));
	mb_check(mb_push_string(s, l, chr));

	return result;
}

int _std_left(mb_interpreter_t* s, void** l) {
	/* Get a number of characters from the left of a string */
	int result = MB_FUNC_OK;
	char* arg = 0;
	int_t count = 0;
	char* sub = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_string(s, l, &arg));
	mb_check(mb_pop_int(s, l, &count));

	mb_check(mb_attempt_close_bracket(s, l));

	if(count <= 0) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#ifdef MB_ENABLE_UNICODE
	switch(mb_uu_substr(arg, 0, count, &sub)) {
	case 0:
		_handle_error_on_obj(s, SE_RN_INVALID_STRING, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	case -1:
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_UNICODE */
	sub = (char*)mb_malloc(count + 1);
	memcpy(sub, arg, count);
	sub[count] = '\0';
#endif /* MB_ENABLE_UNICODE */
	mb_check(mb_push_string(s, l, sub));

_exit:
	return result;
}

int _std_mid(mb_interpreter_t* s, void** l) {
	/* Get a number of characters from a specific position of a string */
	int result = MB_FUNC_OK;
	char* arg = 0;
	int_t start = 0;
	int_t count = 0;
	char* sub = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_string(s, l, &arg));
	mb_check(mb_pop_int(s, l, &start));
	mb_check(mb_pop_int(s, l, &count));

	mb_check(mb_attempt_close_bracket(s, l));

	if(count <= 0 || start < 0 || start >= (int_t)strlen(arg)) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#ifdef MB_ENABLE_UNICODE
	switch(mb_uu_substr(arg, start, count, &sub)) {
	case 0:
		_handle_error_on_obj(s, SE_RN_INVALID_STRING, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	case -1:
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_UNICODE */
	sub = (char*)mb_malloc(count + 1);
	memcpy(sub, arg + start, count);
	sub[count] = '\0';
#endif /* MB_ENABLE_UNICODE */
	mb_check(mb_push_string(s, l, sub));

_exit:
	return result;
}

int _std_right(mb_interpreter_t* s, void** l) {
	/* Get a number of characters from the right of a string */
	int result = MB_FUNC_OK;
	char* arg = 0;
	int_t count = 0;
	char* sub = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_string(s, l, &arg));
	mb_check(mb_pop_int(s, l, &count));

	mb_check(mb_attempt_close_bracket(s, l));

	if(count <= 0) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#ifdef MB_ENABLE_UNICODE
	switch(mb_uu_substr(arg, mb_uu_strlen(arg) - count, count, &sub)) {
	case 0:
		_handle_error_on_obj(s, SE_RN_INVALID_STRING, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	case -1:
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_UNICODE */
	sub = (char*)mb_malloc(count + 1);
	memcpy(sub, arg + (strlen(arg) - count), count);
	sub[count] = '\0';
#endif /* MB_ENABLE_UNICODE */
	mb_check(mb_push_string(s, l, sub));

_exit:
	return result;
}

int _std_str(mb_interpreter_t* s, void** l) {
	/* Get the string format of a number */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	char* chr = 0;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	chr = (char*)mb_malloc(32);
	memset(chr, 0, 32);
	if(arg.type == MB_DT_INT) {
		sprintf(chr, MB_INT_FMT, arg.value.integer);
	} else if(arg.type == MB_DT_REAL) {
		sprintf(chr, MB_REAL_FMT, arg.value.float_point);
	} else {
		result = MB_FUNC_ERR;

		goto _exit;
	}
	mb_check(mb_push_string(s, l, chr));

_exit:
	return result;
}

int _std_val(mb_interpreter_t* s, void** l) {
	/* Get the number format of a string, or get the value of a dictionary iterator */
	int result = MB_FUNC_OK;
	char* conv_suc = 0;
	mb_value_t arg;
#ifdef MB_ENABLE_COLLECTION_LIB
	_object_t ocoi;
#endif /* MB_ENABLE_COLLECTION_LIB */
	mb_value_t ret;

	mb_assert(s && l);

	mb_make_nil(arg);
	mb_make_nil(ret);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	switch(arg.type) {
	case MB_DT_STRING:
		ret.value.integer = (int_t)mb_strtol(arg.value.string, &conv_suc, 0);
		if(*conv_suc == '\0') {
			ret.type = MB_DT_INT;
			mb_check(mb_push_value(s, l, ret));

			goto _exit;
		}
		ret.value.float_point = (real_t)mb_strtod(arg.value.string, &conv_suc);
		if(*conv_suc == '\0') {
			ret.type = MB_DT_REAL;
			mb_check(mb_push_value(s, l, ret));

			goto _exit;
		}
		result = MB_FUNC_ERR;

		break;
#ifdef MB_ENABLE_COLLECTION_LIB
	case MB_DT_DICT_IT:
		_MAKE_NIL(&ocoi);
		_public_value_to_internal_object(&arg, &ocoi);
		if(ocoi.data.dict_it && ocoi.data.dict_it->curr_node && ocoi.data.dict_it->curr_node->data) {
			_internal_object_to_public_value((_object_t*)ocoi.data.dict_it->curr_node->data, &ret);
			mb_check(mb_push_value(s, l, ret));
		} else {
			_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
	default:
		_handle_error_on_obj(s, SE_RN_TYPE_NOT_MATCH, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}

_exit:
	return result;
}

int _std_len(mb_interpreter_t* s, void** l) {
	/* Get the length of a string or an array */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	mb_value_t arg;
	_array_t* arr = 0;
#ifdef MB_ENABLE_COLLECTION_LIB
	_list_t* lst = 0;
	_dict_t* dct = 0;
#endif /* MB_ENABLE_COLLECTION_LIB */

	mb_assert(s && l);

	mb_make_nil(arg);

	ast = (_ls_node_t*)*l;

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	switch(arg.type) {
	case MB_DT_STRING:
#ifdef MB_ENABLE_UNICODE
		mb_check(mb_push_int(s, l, (int_t)mb_uu_strlen(arg.value.string)));
#else /* MB_ENABLE_UNICODE */
		mb_check(mb_push_int(s, l, (int_t)strlen(arg.value.string)));
#endif /* MB_ENABLE_UNICODE */

		break;
	case MB_DT_ARRAY:
		arr = (_array_t*)arg.value.array;
		mb_check(mb_push_int(s, l, (int_t)arr->count));

		break;
#ifdef MB_ENABLE_COLLECTION_LIB
	case MB_DT_LIST:
		lst = (_list_t*)arg.value.list;
		mb_check(mb_push_int(s, l, (int_t)lst->count));
		_assign_public_value(&arg, 0);

		break;
	case MB_DT_DICT:
		dct = (_dict_t*)arg.value.dict;
		mb_check(mb_push_int(s, l, (int_t)_ht_count(dct->dict)));
		_assign_public_value(&arg, 0);

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
	default:
		_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);

		break;
	}

_exit:
	return result;
}

int _std_print(mb_interpreter_t* s, void** l) {
	/* PRINT statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_object_t tmp;
	_object_t val_obj;
	_object_t* val_ptr = 0;

	mb_assert(s && l);

	val_ptr = &val_obj;
	_MAKE_NIL(val_ptr);

	++s->no_eat_comma_mark;
	ast = (_ls_node_t*)*l;
	ast = ast->next;
	if(!ast || !ast->data) {
		_handle_error_on_obj(s, SE_RN_SYNTAX, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	obj = (_object_t*)ast->data;
	do {
		switch(obj->type) {
		case _DT_VAR:
#ifdef MB_ENABLE_CLASS
			if(obj->data.variable->pathing) {
				_ls_node_t* pathed = _search_identifier_in_scope_chain(s, 0, obj->data.variable->name, obj->data.variable->pathing, 0);
				if(pathed && pathed->data) {
					if(obj != (_object_t*)pathed->data) {
						obj = (_object_t*)pathed->data;

						if(obj->type == _DT_ROUTINE) {
							_execute_statement(s, &ast);
							_MAKE_NIL(&tmp);
							_public_value_to_internal_object(&s->running_context->intermediate_value, &tmp);
							val_ptr = obj = &tmp;
							if(tmp.type == _DT_STRING)
								tmp.data.string = mb_strdup(tmp.data.string, strlen(tmp.data.string) + 1);
							if(ast) ast = ast->prev;
						} else if(obj->type == _DT_VAR) {
							val_ptr = obj = obj->data.variable->data;
							if(ast) ast = ast->next;
						} else {
							val_ptr = obj;
							if(ast) ast = ast->next;
						}
					}
				}

				goto _print;
			}
#else /* MB_ENABLE_CLASS */
			mb_unrefvar(tmp);
#endif /* MB_ENABLE_CLASS */
			/* Fall through */
		case _DT_TYPE: /* Fall through */
		case _DT_NIL: /* Fall through */
		case _DT_INT: /* Fall through */
		case _DT_REAL: /* Fall through */
		case _DT_STRING: /* Fall through */
		case _DT_ARRAY: /* Fall through */
#ifdef MB_ENABLE_CLASS
		case _DT_CLASS: /* Fall through */
#endif /* MB_ENABLE_CLASS */
		case _DT_FUNC: /* Fall through */
		case _DT_ROUTINE:
			result = _calc_expression(s, &ast, &val_ptr);
#ifdef MB_ENABLE_CLASS
_print:
#endif /* MB_ENABLE_CLASS */
			if(val_ptr->type == _DT_NIL) {
				_get_printer(s)(MB_NIL);
			} else if(val_ptr->type == _DT_INT) {
				_get_printer(s)(MB_INT_FMT, val_ptr->data.integer);
			} else if(val_ptr->type == _DT_REAL) {
				_get_printer(s)(MB_REAL_FMT, val_ptr->data.float_point);
			} else if(val_ptr->type == _DT_STRING) {
				_get_printer(s)(val_ptr->data.string ? val_ptr->data.string : MB_NULL_STRING);
				if(!val_ptr->ref && val_ptr->data.string) {
					safe_free(val_ptr->data.string);
				}
			} else if(val_ptr->type == _DT_USERTYPE_REF) {
				if(val_ptr->data.usertype_ref->fmt)
					val_ptr->data.usertype_ref->fmt(s, val_ptr->data.usertype_ref->usertype, _get_printer(s));
				else
					_get_printer(s)(mb_get_type_string(_internal_type_to_public_type(val_ptr->type)));
			} else if(val_ptr->type == _DT_TYPE) {
				_get_printer(s)(mb_get_type_string(val_ptr->data.type));
			} else {
				_get_printer(s)(mb_get_type_string(_internal_type_to_public_type(val_ptr->type)));
			}
			if(result != MB_FUNC_OK)
				goto _exit;
			/* Fall through */
		case _DT_SEP:
			if(!ast)
				break;
			obj = (_object_t*)ast->data;
#if _COMMA_AS_NEWLINE
			if(obj->data.separator == ',') {
#else /* _COMMA_AS_NEWLINE */
			if(obj->data.separator == ';') {
#endif /* _COMMA_AS_NEWLINE */
				_get_printer(s)("\n");
			}

			break;
		default:
			_handle_error_on_obj(s, SE_RN_NOT_SUPPORTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);

			break;
		}

		if(!ast)
			break;
		obj = (_object_t*)ast->data;
		if(_is_print_terminal(s, obj))
			break;
		if(_IS_SEP(obj, ',') || _IS_SEP(obj, ';')) {
			ast = ast->next;
			obj = (_object_t*)ast->data;
		} else {
			_handle_error_on_obj(s, SE_RN_COMMA_OR_SEMICOLON_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
		}
	} while(ast && !_IS_SEP(obj, ':') && (obj->type == _DT_SEP || !_is_expression_terminal(s, obj)));

_exit:
	--s->no_eat_comma_mark;

	*l = ast;
	if(result != MB_FUNC_OK)
		_get_printer(s)("\n");

	return result;
}

int _std_input(mb_interpreter_t* s, void** l) {
	/* INPUT statement */
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	char line[256];
	char* conv_suc = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_func_begin(s, l));
	mb_check(mb_attempt_func_end(s, l));

	ast = (_ls_node_t*)*l;
	obj = (_object_t*)ast->data;

	if(!obj || obj->type == _DT_EOS) {
#ifdef _MSC_VER
		getch();
#else /* _MSC_VER */
		_get_inputer(s)(line, sizeof(line));
#endif /* _MSC_VER */

		goto _exit;
	}
	if(obj->type != _DT_VAR) {
		_handle_error_on_obj(s, SE_RN_VARIABLE_EXPECTED, 0, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	if(obj->data.variable->data->type == _DT_INT || obj->data.variable->data->type == _DT_REAL) {
		_get_inputer(s)(line, sizeof(line));
		obj->data.variable->data->type = _DT_INT;
		obj->data.variable->data->data.integer = (int_t)mb_strtol(line, &conv_suc, 0);
		if(*conv_suc != '\0') {
			obj->data.variable->data->type = _DT_REAL;
			obj->data.variable->data->data.float_point = (real_t)mb_strtod(line, &conv_suc);
			if(*conv_suc != '\0') {
				result = MB_FUNC_ERR;

				goto _exit;
			}
		}
		ast = ast->next;
	} else if(obj->data.variable->data->type == _DT_STRING) {
		if(obj->data.variable->data->data.string) {
			safe_free(obj->data.variable->data->data.string);
		}
		obj->data.variable->data->data.string = (char*)mb_malloc(sizeof(line));
		memset(obj->data.variable->data->data.string, 0, sizeof(line));
		_get_inputer(s)(line, sizeof(line));
		strcpy(obj->data.variable->data->data.string, line);
		ast = ast->next;
	} else {
		result = MB_FUNC_ERR;

		goto _exit;
	}

_exit:
	*l = ast;

	return result;
}

/** Collection lib */
#ifdef MB_ENABLE_COLLECTION_LIB
int _coll_list(mb_interpreter_t* s, void** l) {
	/* LIST statement */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	_list_t* coll = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	coll = _create_list(s);

	while(mb_has_arg(s, l)) {
		mb_make_nil(arg);
		mb_check(mb_pop_value(s, l, &arg));
		_push_list(coll, &arg, 0);
	}

	mb_check(mb_attempt_close_bracket(s, l));

	arg.type = MB_DT_LIST;
	arg.value.list = coll;
	mb_check(mb_push_value(s, l, arg));

	return result;
}

int _coll_dict(mb_interpreter_t* s, void** l) {
	/* DICT statement */
	int result = MB_FUNC_OK;
	mb_value_t arg;
	mb_value_t val;
	_dict_t* coll = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	coll = _create_dict(s);

	while(mb_has_arg(s, l)) {
		mb_make_nil(arg);
		mb_make_nil(val);
		mb_check(mb_pop_value(s, l, &arg));
		mb_check(mb_pop_value(s, l, &val));
		_set_dict(coll, &arg, &val, 0, 0);
	}

	mb_check(mb_attempt_close_bracket(s, l));

	arg.type = MB_DT_DICT;
	arg.value.dict = coll;
	mb_check(mb_push_value(s, l, arg));

	return result;
}

int _coll_push(mb_interpreter_t* s, void** l) {
	/* PUSH statement */
	int result = MB_FUNC_OK;
	mb_value_t lst;
	mb_value_t arg;
	_object_t olst;

	mb_assert(s && l);

	mb_make_nil(lst);
	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &lst));
	if(lst.type != MB_DT_LIST) {
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}
	_MAKE_NIL(&olst);
	_public_value_to_internal_object(&lst, &olst);

	while(mb_has_arg(s, l)) {
		mb_make_nil(arg);
		mb_check(mb_pop_value(s, l, &arg));
		_push_list(olst.data.list, &arg, 0);
	}

	mb_check(mb_attempt_close_bracket(s, l));

	mb_check(mb_push_value(s, l, lst));

_exit:
	_assign_public_value(&lst, 0);

	return result;
}

int _coll_pop(mb_interpreter_t* s, void** l) {
	/* POP statement */
	int result = MB_FUNC_OK;
	mb_value_t lst;
	mb_value_t val;
	_object_t olst;
	_object_t ocoll;

	mb_assert(s && l);

	mb_make_nil(lst);
	mb_make_nil(val);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &lst));

	mb_check(mb_attempt_close_bracket(s, l));

	if(lst.type != MB_DT_LIST) {
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

	_MAKE_NIL(&olst);
	_public_value_to_internal_object(&lst, &olst);
	if(_pop_list(olst.data.list, &val, s)) {
		mb_check(mb_push_value(s, l, val));
		_MAKE_NIL(&ocoll);
		_public_value_to_internal_object(&val, &ocoll);
		_UNREF(&ocoll)

		_assign_public_value(&lst, 0);
	} else {
		mb_check(mb_push_value(s, l, val));

		_assign_public_value(&lst, 0);

		_handle_error_on_obj(s, SE_RN_EMPTY_COLLECTION, 0, TON(l), MB_FUNC_WARNING, _exit, result);
	}

_exit:
	return result;
}

int _coll_peek(mb_interpreter_t* s, void** l) {
	/* PEEK statement */
	int result = MB_FUNC_OK;
	mb_value_t lst;
	mb_value_t val;
	_object_t olst;
	_object_t* oval = 0;
	_ls_node_t* node = 0;

	mb_assert(s && l);

	mb_make_nil(lst);
	mb_make_nil(val);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &lst));

	mb_check(mb_attempt_close_bracket(s, l));

	if(lst.type != MB_DT_LIST) {
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

	_MAKE_NIL(&olst);
	_public_value_to_internal_object(&lst, &olst);
	node = _ls_back(olst.data.list->list);
	oval = node ? (_object_t*)node->data : 0;
	if(oval) {
		_internal_object_to_public_value(oval, &val);

		mb_check(mb_push_value(s, l, val));

		_assign_public_value(&lst, 0);
	} else {
		mb_check(mb_push_value(s, l, val));

		_assign_public_value(&lst, 0);

		_handle_error_on_obj(s, SE_RN_EMPTY_COLLECTION, 0, TON(l), MB_FUNC_WARNING, _exit, result);
	}

_exit:
	return result;
}

int _coll_insert(mb_interpreter_t* s, void** l) {
	/* INSERT statement */
	int result = MB_FUNC_OK;
	mb_value_t lst;
	int_t idx = 0;
	mb_value_t arg;
	_object_t olst;
	_object_t* oval = 0;

	mb_assert(s && l);

	mb_make_nil(lst);
	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &lst));
	mb_check(mb_pop_int(s, l, &idx));
	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	if(lst.type != MB_DT_LIST) {
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}
	_MAKE_NIL(&olst);
	_public_value_to_internal_object(&lst, &olst);

	if(!_insert_list(olst.data.list, idx, &arg, &oval)) {
		_destroy_object(oval, 0);

		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}

	mb_check(mb_push_value(s, l, lst));

_exit:
	_assign_public_value(&lst, 0);

	return result;
}

int _coll_sort(mb_interpreter_t* s, void** l) {
	/* SORT statement */
	int result = MB_FUNC_OK;
	mb_value_t lst;
	_object_t olst;

	mb_assert(s && l);

	mb_make_nil(lst);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &lst));

	mb_check(mb_attempt_close_bracket(s, l));

	if(lst.type != MB_DT_LIST) {
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);
	}
	_MAKE_NIL(&olst);
	_public_value_to_internal_object(&lst, &olst);

	_sort_list(olst.data.list);

	mb_check(mb_push_value(s, l, lst));

_exit:
	_assign_public_value(&lst, 0);

	return result;
}

int _coll_exist(mb_interpreter_t* s, void** l){
	/* EXIST statement */
	int result = MB_FUNC_OK;
	mb_value_t coll;
	mb_value_t arg;
	_object_t ocoll;
	mb_value_t ret;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(arg);
	mb_make_nil(ret);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &coll));
	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	ret.type = MB_DT_INT;
	_MAKE_NIL(&ocoll);
	switch(coll.type) {
	case MB_DT_LIST:
		_public_value_to_internal_object(&coll, &ocoll);
		ret.value.integer = !!_find_list(ocoll.data.list, &arg);

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		ret.value.integer = !!_find_dict(ocoll.data.dict, &arg, 0);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}
	mb_check(mb_push_value(s, l, ret));

_exit:
	_assign_public_value(&coll, 0);

	return result;
}

int _coll_get(mb_interpreter_t* s, void** l) {
	/* GET statement */
	int result = MB_FUNC_OK;
	mb_value_t coi;
	int_t index = 0;
	mb_value_t arg;
	_object_t ocoi;
#ifdef MB_ENABLE_CLASS
	char* field = 0;
	_ls_node_t* fnode = 0;
	_object_t* fobj = 0;
#endif /* MB_ENABLE_CLASS */
	mb_value_t ret;

	mb_assert(s && l);

	mb_make_nil(coi);
	mb_make_nil(arg);
	mb_make_nil(ret);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &coi));
	_MAKE_NIL(&ocoi);
	switch(coi.type) {
	case MB_DT_LIST:
		_public_value_to_internal_object(&coi, &ocoi);
		mb_check(mb_pop_int(s, l, &index));
		if(!_at_list(ocoi.data.list, index, &ret)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coi, &ocoi);
		mb_check(mb_pop_value(s, l, &arg));
		if(!_find_dict(ocoi.data.dict, &arg, &ret)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	case MB_DT_LIST_IT:
		_public_value_to_internal_object(&coi, &ocoi);
		if(ocoi.data.list_it && ocoi.data.list_it->curr && ocoi.data.list_it->curr->data) {
			_internal_object_to_public_value((_object_t*)ocoi.data.list_it->curr->data, &ret);
		} else {
			_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	case MB_DT_DICT_IT:
		_public_value_to_internal_object(&coi, &ocoi);
		if(ocoi.data.dict_it && ocoi.data.dict_it->curr_node && ocoi.data.dict_it->curr_node->extra) {
			_internal_object_to_public_value((_object_t*)ocoi.data.dict_it->curr_node->extra, &ret);
		} else {
			_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, 0, TON(l), MB_FUNC_ERR, _exit, result);
		}

		break;
#ifdef MB_ENABLE_CLASS
	case MB_DT_CLASS:
		_public_value_to_internal_object(&coi, &ocoi);
		mb_check(mb_pop_string(s, l, &field));
		fnode = _search_identifier_in_class(s, ocoi.data.instance, field, 0);
		if(fnode && fnode->data) {
			fobj = (_object_t*)fnode->data;
			_internal_object_to_public_value(fobj, &ret);
		}

		break;
#endif /* MB_ENABLE_CLASS */
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_OR_ITERATOR_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_attempt_close_bracket(s, l));

	mb_check(mb_push_value(s, l, ret));

_exit:
	_assign_public_value(&coi, 0);

	return result;
}

int _coll_set(mb_interpreter_t* s, void** l) {
	/* SET statement */
	int result = MB_FUNC_OK;
	mb_value_t coll;
	int_t idx = 0;
	mb_value_t key;
	mb_value_t val;
	_object_t ocoll;
	_object_t* oval = 0;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(key);
	mb_make_nil(val);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &coll));
	_MAKE_NIL(&ocoll);
	switch(coll.type) {
	case MB_DT_LIST:
		_public_value_to_internal_object(&coll, &ocoll);
		while(mb_has_arg(s, l)) {
			mb_make_nil(val);
			mb_check(mb_pop_int(s, l, &idx));
			mb_check(mb_pop_value(s, l, &val));
			if(!_set_list(ocoll.data.list, idx, &val, &oval)) {
				_destroy_object(oval, 0);

				_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
			}
		}

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		while(mb_has_arg(s, l)) {
			mb_make_nil(key);
			mb_make_nil(val);
			mb_check(mb_pop_value(s, l, &key));
			mb_check(mb_pop_value(s, l, &val));
			_set_dict(ocoll.data.dict, &key, &val, 0, 0);
		}

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_attempt_close_bracket(s, l));

	mb_check(mb_push_value(s, l, coll));

_exit:
	_assign_public_value(&coll, 0);

	return result;
}

int _coll_remove(mb_interpreter_t* s, void** l) {
	/* REMOVE statement */
	int result = MB_FUNC_OK;
	mb_value_t coll;
	int_t idx = 0;
	mb_value_t key;
	_object_t ocoll;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(key);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &coll));
	_MAKE_NIL(&ocoll);
	switch(coll.type) {
	case MB_DT_LIST:
		_public_value_to_internal_object(&coll, &ocoll);
		while(mb_has_arg(s, l)) {
			mb_check(mb_pop_int(s, l, &idx));

			if(!_remove_at_list(ocoll.data.list, idx)) {
				_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
			}
		}

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		while(mb_has_arg(s, l)) {
			mb_check(mb_pop_value(s, l, &key));

			if(!_remove_dict(ocoll.data.dict, &key)) {
				_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, 0, TON(l), MB_FUNC_ERR, _exit, result);
			}
		}

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_attempt_close_bracket(s, l));

	mb_check(mb_push_value(s, l, coll));

_exit:
	_assign_public_value(&coll, 0);

	return result;
}

int _coll_clear(mb_interpreter_t* s, void** l) {
	/* CLEAR statement */
	int result = MB_FUNC_OK;
	mb_value_t coll;
	_object_t ocoll;

	mb_assert(s && l);

	mb_make_nil(coll);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &coll));

	mb_check(mb_attempt_close_bracket(s, l));

	_MAKE_NIL(&ocoll);
	switch(coll.type) {
	case MB_DT_LIST:
		_public_value_to_internal_object(&coll, &ocoll);
		_clear_list(ocoll.data.list);

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		_clear_dict(ocoll.data.dict);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_push_value(s, l, coll));

_exit:
	_assign_public_value(&coll, 0);

	return result;
}

int _coll_clone(mb_interpreter_t* s, void** l) {
	/* CLONE statement */
	int result = MB_FUNC_OK;
	mb_value_t coll;
	_object_t ocoll;
	_object_t otgt;
	mb_value_t ret;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(ret);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &coll));

	mb_check(mb_attempt_close_bracket(s, l));

	_MAKE_NIL(&ocoll);
	switch(coll.type) {
	case MB_DT_LIST:
		_public_value_to_internal_object(&coll, &ocoll);
		_clone_object(s, &ocoll, &otgt);
		ret.type = MB_DT_LIST;
		ret.value.list = otgt.data.list;

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		_clone_object(s, &ocoll, &otgt);
		ret.type = MB_DT_DICT;
		ret.value.dict = otgt.data.dict;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_push_value(s, l, ret));

_exit:
	_assign_public_value(&coll, 0);

	return result;
}

int _coll_iterator(mb_interpreter_t* s, void** l) {
	/* ITERATOR statement */
	int result = MB_FUNC_OK;
	mb_value_t coll;
	_object_t ocoll;
	_list_it_t* lit = 0;
	_dict_it_t* dit = 0;
	mb_value_t ret;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(ret);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &coll));

	mb_check(mb_attempt_close_bracket(s, l));

	_MAKE_NIL(&ocoll);
	switch(coll.type) {
	case MB_DT_LIST:
		_public_value_to_internal_object(&coll, &ocoll);
		lit = _create_list_it(ocoll.data.list, false);
		ret.type = MB_DT_LIST_IT;
		ret.value.list_it = lit;

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		dit = _create_dict_it(ocoll.data.dict, false);
		ret.type = MB_DT_DICT_IT;
		ret.value.list_it = dit;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_push_value(s, l, ret));

_exit:
	_assign_public_value(&coll, 0);

	return result;
}

int _coll_move_next(mb_interpreter_t* s, void** l) {
	/* MOVE_NEXT statement */
	int result = MB_FUNC_OK;
	mb_value_t it;
	_object_t oit;
	mb_value_t ret;

	mb_assert(s && l);

	mb_make_nil(it);
	mb_make_nil(ret);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &it));

	mb_check(mb_attempt_close_bracket(s, l));

	_MAKE_NIL(&oit);
	switch(it.type) {
	case MB_DT_LIST_IT:
		_public_value_to_internal_object(&it, &oit);
		oit.data.list_it = _move_list_it_next(oit.data.list_it);
		if(_invalid_list_it(oit.data.list_it)) {
			_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, 0, TON(l), MB_FUNC_ERR, _exit, result);
		} else if(oit.data.list_it) {
			ret.type = MB_DT_INT;
			ret.value.integer = 1;
		} else {
			mb_make_nil(ret);
		}

		break;
	case MB_DT_DICT_IT:
		_public_value_to_internal_object(&it, &oit);
		oit.data.dict_it = _move_dict_it_next(oit.data.dict_it);
		if(_invalid_dict_it(oit.data.dict_it)) {
			_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, 0, TON(l), MB_FUNC_ERR, _exit, result);
		} else if(oit.data.dict_it) {
			ret.type = MB_DT_INT;
			ret.value.integer = 1;
		} else {
			mb_make_nil(ret);
		}

		break;
	default:
		_handle_error_on_obj(s, SE_RN_ITERATOR_EXPECTED, 0, TON(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_push_value(s, l, ret));

_exit:
	return result;
}
#endif /* MB_ENABLE_COLLECTION_LIB */

/* ========================================================} */

#ifdef MB_COMPACT_MODE
#	pragma pack()
#endif /* MB_COMPACT_MODE */

#ifdef __BORLANDC__
#	pragma warn .8004
#	pragma warn .8008
#	pragma warn .8012
#endif /* __BORLANDC__ */

#ifdef __APPLE__
#	pragma clang diagnostic pop
#endif /* __APPLE__ */

#ifdef _MSC_VER
#	pragma warning(pop)
#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif /* __cplusplus */
