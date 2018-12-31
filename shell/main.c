/*
** This source file is part of MY-BASIC
**
** For the latest info, see https://github.com/paladin-t/my_basic/
**
** Copyright (C) 2011 - 2019 Wang Renxin
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

#include "../core/my_basic.h"
#ifdef MB_CP_VC
#	include <conio.h>
#	include <crtdbg.h>
#	include <locale.h>
#	include <Windows.h>
#elif !defined MB_CP_BORLANDC && !defined MB_CP_TCC
#	include <unistd.h>
#endif /* MB_CP_VC */
#ifdef MB_CP_BORLANDC
#	include <Windows.h>
#endif /* MB_CP_BORLANDC */
#ifndef MB_CP_VC
#	include <stdint.h>
#endif /* MB_CP_VC */
#ifdef MB_CP_CLANG
#	include <sys/time.h>
#endif /* MB_CP_CLANG */
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MB_CP_VC
#	pragma warning(disable : 4127)
#	pragma warning(disable : 4706)
#	pragma warning(disable : 4996)
#endif /* MB_CP_VC */

#ifdef MB_CP_BORLANDC
#	pragma warn -8004
#	pragma warn -8008
#	pragma warn -8066
#endif /* MB_CP_BORLANDC */

#ifdef MB_CP_PELLESC
#	define unlink _unlink
#endif /* MB_CP_PELLESC */

/*
** {========================================================
** Common declarations
*/

#ifdef MB_OS_WIN
#	define _BIN_FILE_NAME "my_basic"
#elif defined MB_OS_MAC
#	define _BIN_FILE_NAME "my_basic_mac"
#else
#	define _BIN_FILE_NAME "my_basic_bin"
#endif

/* Define as 1 to use memory pool, 0 to disable */
#define _USE_MEM_POOL 1
#define _RESET_WHEN_PUSH_TO_POOL 1

#define _MAX_LINE_LENGTH 256
#define _str_eq(__str1, __str2) (mb_stricmp((__str1), (__str2)) == 0)

#define _REALLOC_INC_STEP 16

#define _NOT_FINISHED(s) ((s) == MB_FUNC_OK || (s) == MB_FUNC_SUSPEND || (s) == MB_FUNC_WARNING || (s) == MB_FUNC_ERR || (s) == MB_FUNC_END)

static struct mb_interpreter_t* bas = 0;

static jmp_buf mem_failure_point;

#define _CHECK_MEM(__p) do { if(!(__p)) { longjmp(mem_failure_point, 1); } } while(0)

/* ========================================================} */

/*
** {========================================================
** Common
*/

#ifndef countof
#	define countof(__a) (sizeof(__a) / sizeof(*(__a)))
#endif /* countof */

#ifndef _printf
#	define _printf printf
#endif /* _printf */

/* ========================================================} */

/*
** {========================================================
** Memory manipulation
*/

#if _USE_MEM_POOL
extern MBAPI const size_t MB_SIZEOF_4BYTES;
extern MBAPI const size_t MB_SIZEOF_8BYTES;
extern MBAPI const size_t MB_SIZEOF_32BYTES;
extern MBAPI const size_t MB_SIZEOF_64BYTES;
extern MBAPI const size_t MB_SIZEOF_128BYTES;
extern MBAPI const size_t MB_SIZEOF_256BYTES;
extern MBAPI const size_t MB_SIZEOF_512BYTES;
extern MBAPI const size_t MB_SIZEOF_INT;
extern MBAPI const size_t MB_SIZEOF_PTR;
extern MBAPI const size_t MB_SIZEOF_LSN;
extern MBAPI const size_t MB_SIZEOF_HTN;
extern MBAPI const size_t MB_SIZEOF_HTA;
extern MBAPI const size_t MB_SIZEOF_OBJ;
#ifdef MB_ENABLE_USERTYPE_REF
extern MBAPI const size_t MB_SIZEOF_UTR;
#else /* MB_ENABLE_USERTYPE_REF */
static const size_t MB_SIZEOF_UTR = 16;
#endif /* MB_ENABLE_USERTYPE_REF */
extern MBAPI const size_t MB_SIZEOF_FUN;
extern MBAPI const size_t MB_SIZEOF_VAR;
extern MBAPI const size_t MB_SIZEOF_ARR;
#ifdef MB_ENABLE_COLLECTION_LIB
extern MBAPI const size_t MB_SIZEOF_LST;
extern MBAPI const size_t MB_SIZEOF_DCT;
#else /* MB_ENABLE_COLLECTION_LIB */
static const size_t MB_SIZEOF_LST = 16;
static const size_t MB_SIZEOF_DCT = 16;
#endif /* MB_ENABLE_COLLECTION_LIB */
extern MBAPI const size_t MB_SIZEOF_LBL;
#ifdef MB_ENABLE_CLASS
extern MBAPI const size_t MB_SIZEOF_CLS;
#else /* MB_ENABLE_CLASS */
static const size_t MB_SIZEOF_CLS = 16;
#endif /* MB_ENABLE_CLASS */
extern MBAPI const size_t MB_SIZEOF_RTN;

typedef unsigned _pool_chunk_size_t;

typedef union _pool_tag_t {
	_pool_chunk_size_t size;
	void* ptr;
} _pool_tag_t;

typedef struct _pool_t {
	size_t size;
	char* stack;
} _pool_t;

static int pool_count = 0;

static _pool_t* pool = 0;

static long alloc_count = 0;
static long alloc_bytes = 0;
static long in_pool_count = 0;
static long in_pool_bytes = 0;

static long POOL_THRESHOLD_COUNT = 0;
static long POOL_THRESHOLD_BYTES = 1024 * 1024 * 32;

#define _POOL_NODE_ALLOC(size) (((char*)malloc(sizeof(_pool_tag_t) + size)) + sizeof(_pool_tag_t))
#define _POOL_NODE_PTR(s) (s - sizeof(_pool_tag_t))
#define _POOL_NODE_NEXT(s) (*((void**)(s - sizeof(_pool_tag_t))))
#define _POOL_NODE_SIZE(s) (*((_pool_chunk_size_t*)(s - sizeof(_pool_tag_t))))
#define _POOL_NODE_FREE(s) do { free(_POOL_NODE_PTR(s)); } while(0)

static int _cmp_size_t(const void* l, const void* r) {
	size_t* pl = (size_t*)l;
	size_t* pr = (size_t*)r;

	if(*pl > *pr)
		return 1;
	else if(*pl < *pr)
		return -1;
	else
		return 0;
}

static void _tidy_mem_pool(bool_t force) {
	int i = 0;
	char* s = 0;

	if(!force) {
		if(POOL_THRESHOLD_COUNT > 0 && in_pool_count < POOL_THRESHOLD_COUNT)
			return;

		if(POOL_THRESHOLD_BYTES > 0 && in_pool_bytes < POOL_THRESHOLD_BYTES)
			return;
	}

	if(!pool_count)
		return;

	for(i = 0; i < pool_count; i++) {
		while((s = pool[i].stack) != 0) {
			pool[i].stack = (char*)_POOL_NODE_NEXT(s);
			_POOL_NODE_FREE(s);
		}
	}

	in_pool_count = 0;
	in_pool_bytes = 0;
}

static void _open_mem_pool(void) {
#	define N 22
	size_t szs[N];
	size_t lst[N];
	int i = 0;
	int j = 0;
	size_t s = 0;

	pool_count = 0;

	szs[i++] = MB_SIZEOF_4BYTES;
	szs[i++] = MB_SIZEOF_8BYTES;
	szs[i++] = MB_SIZEOF_32BYTES;
	szs[i++] = MB_SIZEOF_64BYTES;
	szs[i++] = MB_SIZEOF_128BYTES;
	szs[i++] = MB_SIZEOF_256BYTES;
	szs[i++] = MB_SIZEOF_512BYTES;
	szs[i++] = MB_SIZEOF_INT;
	szs[i++] = MB_SIZEOF_PTR;
	szs[i++] = MB_SIZEOF_LSN;
	szs[i++] = MB_SIZEOF_HTN;
	szs[i++] = MB_SIZEOF_HTA;
	szs[i++] = MB_SIZEOF_OBJ;
	szs[i++] = MB_SIZEOF_UTR;
	szs[i++] = MB_SIZEOF_FUN;
	szs[i++] = MB_SIZEOF_VAR;
	szs[i++] = MB_SIZEOF_ARR;
	szs[i++] = MB_SIZEOF_LST;
	szs[i++] = MB_SIZEOF_DCT;
	szs[i++] = MB_SIZEOF_LBL;
	szs[i++] = MB_SIZEOF_CLS;
	szs[i++] = MB_SIZEOF_RTN;

	mb_assert(i == N);

	memset(lst, 0, sizeof(lst));

	/* Find all unduplicated sizes */
	for(i = 0; i < N; i++) {
		s = szs[i];
		for(j = 0; j < N; j++) {
			if(!lst[j]) {
				lst[j] = s;
				pool_count++;

				break;
			} else if(lst[j] == s) {
				break;
			}
		}
	}
	qsort(lst, pool_count, sizeof(lst[0]), _cmp_size_t);

	pool = (_pool_t*)malloc(sizeof(_pool_t) * pool_count);
	_CHECK_MEM(pool);
	for(i = 0; i < pool_count; i++) {
		pool[i].size = lst[i];
		pool[i].stack = 0;
	}
#	undef N
}

static void _close_mem_pool(void) {
	int i = 0;
	char* s = 0;

	if(!pool_count)
		return;

	for(i = 0; i < pool_count; i++) {
		while((s = pool[i].stack) != 0) {
			pool[i].stack = (char*)_POOL_NODE_NEXT(s);
			_POOL_NODE_FREE(s);
		}
	}

	free((void*)pool);
	pool = 0;
	pool_count = 0;
}

static char* _pop_mem(unsigned s) {
	int i = 0;
	_pool_t* pl = 0;
	char* result = 0;

	++alloc_count;
	alloc_bytes += s;

	if(pool_count) {
		for(i = 0; i < pool_count; i++) {
			pl = &pool[i];
			if(s <= pl->size) {
				if(pl->stack) {
					in_pool_count--;
					in_pool_bytes -= (long)(_pool_chunk_size_t)s;

					/* Pop from stack */
					result = pl->stack;
					pl->stack = (char*)_POOL_NODE_NEXT(result);
					_POOL_NODE_SIZE(result) = (_pool_chunk_size_t)s;

					return result;
				} else {
					/* Create a new node */
					result = _POOL_NODE_ALLOC(pl->size);
					_CHECK_MEM(result);
					if((intptr_t)result == sizeof(_pool_tag_t)) {
						result = 0;
					} else {
						_POOL_NODE_SIZE(result) = (_pool_chunk_size_t)s;
					}

					return result;
				}
			}
		}
	}

	/* Allocate directly */
	result = _POOL_NODE_ALLOC(s);
	_CHECK_MEM(result);
	if((intptr_t)result == sizeof(_pool_tag_t)) {
		result = 0;
	} else {
		_POOL_NODE_SIZE(result) = (_pool_chunk_size_t)s;
	}

	return result;
}

static void _push_mem(char* p) {
	int i = 0;
	_pool_t* pl = 0;

	if(--alloc_count < 0) {
		mb_assert(0 && "Multiple free.");
	}
	alloc_bytes -= _POOL_NODE_SIZE(p);

#if _RESET_WHEN_PUSH_TO_POOL
	memset(p, 0, _POOL_NODE_SIZE(p));
#endif /* _RESET_WHEN_PUSH_TO_POOL */
	if(pool_count) {
		for(i = 0; i < pool_count; i++) {
			pl = &pool[i];
			if(_POOL_NODE_SIZE(p) <= pl->size) {
				in_pool_count++;
				in_pool_bytes += _POOL_NODE_SIZE(p);

				/* Push to stack */
				_POOL_NODE_NEXT(p) = pl->stack;
				pl->stack = p;

				_tidy_mem_pool(false);

				return;
			}
		}
	}

	/* Free directly */
	_POOL_NODE_FREE(p);
}
#endif /* _USE_MEM_POOL */

/* ========================================================} */

/*
** {========================================================
** Code manipulation
*/

typedef struct _code_line_t {
	char** lines;
	int count;
	int size;
} _code_line_t;

static _code_line_t* code = 0;

static _code_line_t* _code(void) {
	return code;
}

static _code_line_t* _create_code(void) {
	_code_line_t* result = 0;

	result = (_code_line_t*)malloc(sizeof(_code_line_t));
	_CHECK_MEM(result);
	result->count = 0;
	result->size = _REALLOC_INC_STEP;
	result->lines = (char**)malloc(sizeof(char*) * result->size);
	_CHECK_MEM(result->lines);

	code = result;

	return result;
}

static void _destroy_code(void) {
	int i = 0;

	mb_assert(code);

	for(i = 0; i < code->count; ++i)
		free(code->lines[i]);
	free(code->lines);
	free(code);
	code = 0;
}

static void _clear_code(void) {
	int i = 0;

	mb_assert(code);

	for(i = 0; i < code->count; ++i)
		free(code->lines[i]);
	code->count = 0;
}

static int _append_line(const char* txt) {
	int result = 0;
	int l = 0;
	char* buf = 0;

	mb_assert(code && txt);

	if(code->count + 1 == code->size) {
		code->size += _REALLOC_INC_STEP;
		code->lines = (char**)realloc(code->lines, sizeof(char*) * code->size);
	}
	result = l = (int)strlen(txt);
	buf = (char*)malloc(l + 2);
	_CHECK_MEM(buf);
	memcpy(buf, txt, l);
	buf[l] = '\n';
	buf[l + 1] = '\0';
	code->lines[code->count++] = buf;

	return result;
}

static char* _get_code(void) {
	char* result = 0;
	int i = 0;

	mb_assert(code);

	result = (char*)malloc((_MAX_LINE_LENGTH + 2) * code->count + 1);
	_CHECK_MEM(result);
	result[0] = '\0';
	for(i = 0; i < code->count; ++i) {
		result = strcat(result, code->lines[i]);
		if(i != code->count - 1)
			result = strcat(result, "\n");
	}

	return result;
}

static void _set_code(char* txt) {
	char* cursor = 0;
	char _c = '\0';

	mb_assert(code);

	if(!txt)
		return;

	_clear_code();
	cursor = txt;
	do {
		_c = *cursor;
		if(_c == '\r' || _c == '\n' || _c == '\0') {
			cursor[0] = '\0';
			if(_c == '\r' && *(cursor + 1) == '\n')
				++cursor;
			_append_line(txt);
			txt = cursor + 1;
		}
		++cursor;
	} while(_c);
}

static char* _load_file(const char* path) {
	FILE* fp = 0;
	char* result = 0;
	long curpos = 0;
	long l = 0;

	mb_assert(path);

	fp = fopen(path, "rb");
	if(fp) {
		curpos = ftell(fp);
		fseek(fp, 0L, SEEK_END);
		l = ftell(fp);
		fseek(fp, curpos, SEEK_SET);
		result = (char*)malloc((size_t)(l + 1));
		_CHECK_MEM(result);
		fread(result, 1, l, fp);
		fclose(fp);
		result[l] = '\0';
	}

	return result;
}

static int _save_file(const char* path, const char* txt) {
	FILE* fp = 0;

	mb_assert(path && txt);

	fp = fopen(path, "wb");
	if(fp) {
		fwrite(txt, sizeof(char), strlen(txt), fp);
		fclose(fp);

		return 1;
	}

	return 0;
}

/* ========================================================} */

/*
** {========================================================
** Importing directories
*/

typedef struct _importing_dirs_t {
	char** dirs;
	int count;
	int size;
} _importing_dirs_t;

static _importing_dirs_t* importing_dirs = 0;

static void _destroy_importing_directories(void) {
	int i = 0;

	if(!importing_dirs)
		return;

	for(i = 0; i < importing_dirs->count; ++i)
		free(importing_dirs->dirs[i]);
	free(importing_dirs->dirs);
	free(importing_dirs);
	importing_dirs = 0;
}

static _importing_dirs_t* _set_importing_directories(const char* dirs) {
	_importing_dirs_t* result = 0;

	if(!dirs)
		return result;

	result = (_importing_dirs_t*)malloc(sizeof(_importing_dirs_t));
	_CHECK_MEM(result);
	result->count = 0;
	result->size = _REALLOC_INC_STEP;
	result->dirs = (char**)malloc(sizeof(char*) * result->size);
	_CHECK_MEM(result->dirs);

	while(dirs && *dirs) {
		size_t l = 0;
		char* buf = 0;
		bool_t as = false;
		const char* p = dirs;
		dirs = strchr(dirs, ';');
		if(dirs) {
			l = dirs - p;
			if(*dirs == ';') ++dirs;
		} else {
			l = strlen(p);
		}
		if(result->count + 1 == result->size) {
			result->size += _REALLOC_INC_STEP;
			result->dirs = (char**)realloc(result->dirs, sizeof(char*) * result->size);
		}
		as = p[l - 1] != '/' && p[l - 1] != '\\';
		buf = (char*)malloc(l + (as ? 2 : 1));
		_CHECK_MEM(buf);
		memcpy(buf, p, l);
		if(as) {
			buf[l] = '/';
			buf[l + 1] = '\0';
		} else {
			buf[l] = '\0';
		}
		result->dirs[result->count++] = buf;
		while(*buf) {
			if(*buf == '\\') *buf = '/';
			buf++;
		}
	}

	_destroy_importing_directories();
	importing_dirs = result;

	return result;
}

static bool_t _try_import(struct mb_interpreter_t* s, const char* p) {
	bool_t result = false;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < importing_dirs->count; i++) {
		char* t = 0;
		char* d = importing_dirs->dirs[i];
		int m = (int)strlen(d);
		int n = (int)strlen(p);
#if _USE_MEM_POOL
		char* buf = _pop_mem(m + n + 1);
#else /* _USE_MEM_POOL */
		char* buf = (char*)malloc(m + n + 1);
		_CHECK_MEM(buf);
#endif /* _USE_MEM_POOL */
		memcpy(buf, d, m);
		memcpy(buf + m, p, n);
		buf[m + n] = '\0';
		t = _load_file(buf);
		if(t) {
			if(mb_load_string(s, t, true) == MB_FUNC_OK)
				result = true;
			free(t);
		}
#if _USE_MEM_POOL
		_push_mem(buf);
#else /* _USE_MEM_POOL */
		free(buf);
#endif /* _USE_MEM_POOL */
		if(result)
			break;
	}

	return result;
}

/* ========================================================} */

/*
** {========================================================
** Interactive commands
*/

static int _get_unicode_bom(const char** ch) {
	if(!ch && !(*ch))
		return 0;

	if((*ch)[0] == -17 && (*ch)[1] == -69 && (*ch)[2] == -65) {
		*ch += 3;

		return 3;
	} else if((*ch)[0] == -2 && (*ch)[1] == -1) {
		*ch += 2;

		return 2;
	}

	return 0;
}

static void _clear_screen(void) {
#ifdef MB_OS_WIN
	system("cls");
#else /* MB_OS_WIN */
	system("clear");
#endif /* MB_OS_WIN */
}

static int _new_program(void) {
	int result = 0;

	_clear_code();

	result = mb_reset(&bas, false);

	mb_gc(bas, 0);

#if _USE_MEM_POOL
	_tidy_mem_pool(true);
#endif /* _USE_MEM_POOL */

	return result;
}

#if defined MB_CP_VC && defined MB_ENABLE_UNICODE
static int _bytes_to_wchar(const char* sz, wchar_t** out, size_t size) {
	int result = MultiByteToWideChar(CP_UTF8, 0, sz, -1, 0, 0);
	if((int)size < result) {
		*out = (wchar_t*)malloc(sizeof(wchar_t) * result);
		_CHECK_MEM(*out);
	}
	MultiByteToWideChar(CP_UTF8, 0, sz, -1, *out, result);

	return result;
}

static int _bytes_to_wchar_ansi(const char* sz, wchar_t** out, size_t size) {
	int result = MultiByteToWideChar(CP_ACP, 0, sz, -1, 0, 0);
	if((int)size < result) {
		*out = (wchar_t*)malloc(sizeof(wchar_t) * result);
		_CHECK_MEM(*out);
	}
	MultiByteToWideChar(CP_ACP, 0, sz, -1, *out, result);

	return result;
}

static int _wchar_to_bytes(const wchar_t* sz, char** out, size_t size) {
	int result = WideCharToMultiByte(CP_UTF8, 0, sz, -1, 0, 0, 0, 0);
	if((int)size < result) {
		*out = (char*)malloc(result);
		_CHECK_MEM(*out);
	}
	WideCharToMultiByte(CP_UTF8, 0, sz, -1, *out, result, 0, 0);

	return result;
}
#endif /* MB_CP_VC && MB_ENABLE_UNICODE */

static int _append_one_line(const char* line) {
	int result = 0;
#if defined MB_CP_VC && defined MB_ENABLE_UNICODE
	char str[16];
	char* strp = str;
	wchar_t wstr[16];
	wchar_t* wstrp = wstr;
	_bytes_to_wchar_ansi(line, &wstrp, countof(wstr));
	result = _wchar_to_bytes(wstrp, &strp, countof(str));
	if(wstrp != wstr)
		free(wstrp);
	_append_line(strp);
	if(strp != str)
		free(strp);
#else /* MB_CP_VC && MB_ENABLE_UNICODE */
	result = _append_line(line);
#endif /* MB_CP_VC && MB_ENABLE_UNICODE */

	return result;
}

static void _list_one_line(bool_t nl, long l, const char* ln) {
#if defined MB_CP_VC && defined MB_ENABLE_UNICODE
	wchar_t wstr[16];
	wchar_t* wstrp = wstr;
	char* loc = setlocale(LC_ALL, "");
	_bytes_to_wchar(ln, &wstrp, countof(wstr));
	_printf(nl ? "%ld]%ls\n" : "%ld]%ls", l, wstrp);
	if(wstrp != wstr)
		free(wstrp);
	setlocale(LC_ALL, loc);
#else /* MB_CP_VC && MB_ENABLE_UNICODE */
	_printf(nl ? "%ld]%s\n" : "%ld]%s", l, ln);
#endif /* MB_CP_VC && MB_ENABLE_UNICODE */
}

static void _list_program(const char* sn, const char* cn) {
	long lsn = 0;
	long lcn = 0;
	char* p = 0;

	mb_assert(sn && cn);

	lsn = atoi(sn);
	lcn = atoi(cn);
	if(lsn == 0 && lcn == 0) {
		long i = 0;
		for(i = 0; i < _code()->count; ++i) {
			p = _code()->lines[i];
			_get_unicode_bom((const char**)&p);
			_list_one_line(false, i + 1, p);
		}
	} else {
		long i = 0;
		long e = 0;
		if(lsn < 1 || lsn > _code()->count) {
			_printf("Line number %ld out of bound.\n", lsn);

			return;
		}
		if(lcn < 0) {
			_printf("Invalid line count %ld.\n", lcn);

			return;
		}
		--lsn;
		e = lcn ? lsn + lcn : _code()->count;
		for(i = lsn; i < e; ++i) {
			if(i >= _code()->count)
				break;

			p = _code()->lines[i];
			_get_unicode_bom((const char**)&p);
			_list_one_line(true, i + 1, p);
		}
	}
}

static void _edit_program(const char* no) {
	char line[_MAX_LINE_LENGTH];
	long lno = 0;
	int l = 0;

	mb_assert(no);

	lno = atoi(no);
	if(lno < 1 || lno > _code()->count) {
		_printf("Line number %ld out of bound.\n", lno);

		return;
	}
	--lno;
	memset(line, 0, _MAX_LINE_LENGTH);
	_printf("%ld]", lno + 1);
	mb_gets(0, line, _MAX_LINE_LENGTH);
	l = (int)strlen(line);
	_code()->lines[lno] = (char*)realloc(_code()->lines[lno], l + 2);
	strcpy(_code()->lines[lno], line);
	_code()->lines[lno][l] = '\n';
	_code()->lines[lno][l + 1] = '\0';
}

static void _insert_program(const char* no) {
	char line[_MAX_LINE_LENGTH];
	long lno = 0;
	int l = 0;
	int i = 0;

	mb_assert(no);

	lno = atoi(no);
	if(lno < 1 || lno > _code()->count) {
		_printf("Line number %ld out of bound.\n", lno);

		return;
	}
	--lno;
	memset(line, 0, _MAX_LINE_LENGTH);
	_printf("%ld]", lno + 1);
	mb_gets(0, line, _MAX_LINE_LENGTH);
	if(_code()->count + 1 == _code()->size) {
		_code()->size += _REALLOC_INC_STEP;
		_code()->lines = (char**)realloc(_code()->lines, sizeof(char*) * _code()->size);
	}
	for(i = _code()->count; i > lno; i--)
		_code()->lines[i] = _code()->lines[i - 1];
	l = (int)strlen(line);
	_code()->lines[lno] = (char*)realloc(0, l + 2);
	strcpy(_code()->lines[lno], line);
	_code()->lines[lno][l] = '\n';
	_code()->lines[lno][l + 1] = '\0';
	_code()->count++;
}

static void _alter_program(const char* no) {
	long lno = 0;
	long i = 0;

	mb_assert(no);

	lno = atoi(no);
	if(lno < 1 || lno > _code()->count) {
		_printf("Line number %ld out of bound.\n", lno);

		return;
	}
	--lno;
	free(_code()->lines[lno]);
	for(i = lno; i < _code()->count - 1; i++)
		_code()->lines[i] = _code()->lines[i + 1];
	_code()->count--;
}

static void _load_program(const char* path) {
	char* txt = 0;

	mb_assert(path);

	txt = _load_file(path);
	if(txt) {
		_new_program();
		_set_code(txt);
		free(txt);
		if(_code()->count == 1) {
			_printf("Load done. %d line loaded.\n", _code()->count);
		} else {
			_printf("Load done. %d lines loaded.\n", _code()->count);
		}
	} else {
		_printf("Cannot load file \"%s\".\n", path);
	}
}

static void _save_program(const char* path) {
	char* txt = 0;

	mb_assert(path);

	txt = _get_code();
	if(!_save_file(path, txt)) {
		_printf("Cannot save file \"%s\".\n", path);
	} else {
		if(_code()->count == 1) {
			_printf("Save done. %d line saved.\n", _code()->count);
		} else {
			_printf("Save done. %d lines saved.\n", _code()->count);
		}
	}
	free(txt);
}

static void _kill_program(const char* path) {
	if(!unlink(path)) {
		_printf("Delete file \"%s\" successfully.\n", path);
	} else {
		FILE* fp = fopen(path, "rb");
		if(fp) {
			fclose(fp);
			_printf("Delete file \"%s\" failed.\n", path);
		} else {
			_printf("File \"%s\" not found.\n", path);
		}
	}
}

static void _list_directory(const char* path) {
	char line[_MAX_LINE_LENGTH];

#ifdef MB_OS_WIN
	if(path && *path) sprintf(line, "dir %s", path);
	else sprintf(line, "dir");
#else /* MB_OS_WIN */
	if(path && *path) sprintf(line, "ls %s", path);
	else sprintf(line, "ls");
#endif /* MB_OS_WIN */
	system(line);
}

static void _show_tip(void) {
	_printf("MY-BASIC Interpreter Shell - %s\n", mb_ver_string());
	_printf("Copyright (C) 2011 - 2019 Wang Renxin. All Rights Reserved.\n");
	_printf("For more information, see https://github.com/paladin-t/my_basic/.\n");
	_printf("Input HELP and hint enter to view help information.\n");
}

static void _show_help(void) {
	_printf("Modes:\n");
	_printf("  %s           - Launch interactive mode\n", _BIN_FILE_NAME);
	_printf("  %s *.*       - Load and run a file\n", _BIN_FILE_NAME);
	_printf("  %s -e \"expr\" - Evaluate an expression\n", _BIN_FILE_NAME);
	_printf("\n");
	_printf("Options:\n");
	_printf("  -h         - Show help information\n");
#if _USE_MEM_POOL
	_printf("  -p n       - Set memory pool threashold, n is size in bytes\n");
#endif /* _USE_MEM_POOL */
	_printf("  -f \"dirs\"  - Set importing directories, separated by \";\" with more than one\n");
	_printf("\n");
	_printf("Interactive commands:\n");
	_printf("  HELP  - View help information\n");
	_printf("  CLS   - Clear screen\n");
	_printf("  NEW   - Clear current program\n");
	_printf("  RUN   - Run current program\n");
	_printf("  BYE   - Quit interpreter\n");
	_printf("  LIST  - List current program\n");
	_printf("          Usage: LIST [l [n]], l is start line number, n is line count\n");
	_printf("  EDIT  - Edit (modify/insert/remove) a line in current program\n");
	_printf("          Usage: EDIT n, n is line number\n");
	_printf("                 EDIT -i n, insert a line before a given line\n");
	_printf("                 EDIT -r n, remove a line\n");
	_printf("  LOAD  - Load a file as current program\n");
	_printf("          Usage: LOAD *.*\n");
	_printf("  SAVE  - Save current program to a file\n");
	_printf("          Usage: SAVE *.*\n");
	_printf("  KILL  - Delete a file\n");
	_printf("          Usage: KILL *.*\n");
	_printf("  DIR   - List all files in a directory\n");
	_printf("          Usage: DIR [p], p is a directory path\n");
}

static int _do_line(void) {
	int result = MB_FUNC_OK;
	char line[_MAX_LINE_LENGTH];
	char dup[_MAX_LINE_LENGTH];

	mb_assert(bas);

	memset(line, 0, _MAX_LINE_LENGTH);
	_printf("]");
	mb_gets(0, line, _MAX_LINE_LENGTH);

	memcpy(dup, line, _MAX_LINE_LENGTH);
	strtok(line, " ");

	if(_str_eq(line, "")) {
		/* Do nothing */
	} else if(_str_eq(line, "HELP")) {
		_show_help();
	} else if(_str_eq(line, "CLS")) {
		_clear_screen();
	} else if(_str_eq(line, "NEW")) {
		result = _new_program();
	} else if(_str_eq(line, "RUN")) {
		int i = 0;
		mb_assert(_code());
		result = mb_reset(&bas, false);
		for(i = 0; i < _code()->count; ++i) {
			if(result)
				break;

			result = mb_load_string(bas, _code()->lines[i], false);
		}
		if(result == MB_FUNC_OK)
			result = mb_run(bas, true);
		_printf("\n");
	} else if(_str_eq(line, "BYE")) {
		result = MB_FUNC_BYE;
	} else if(_str_eq(line, "LIST")) {
		char* sn = line + strlen(line) + 1;
		char* cn = 0;
		strtok(sn, " ");
		cn = sn + strlen(sn) + 1;
		_list_program(sn, cn);
	} else if(_str_eq(line, "EDIT")) {
		char* no = line + strlen(line) + 1;
		char* ne = 0;
		strtok(no, " ");
		ne = no + strlen(no) + 1;
		if(!(*ne))
			_edit_program(no);
		else if(_str_eq(no, "-I"))
			_insert_program(ne);
		else if(_str_eq(no, "-R"))
			_alter_program(ne);
	} else if(_str_eq(line, "LOAD")) {
		char* path = line + strlen(line) + 1;
		_load_program(path);
	} else if(_str_eq(line, "SAVE")) {
		char* path = line + strlen(line) + 1;
		_save_program(path);
	} else if(_str_eq(line, "KILL")) {
		char* path = line + strlen(line) + 1;
		_kill_program(path);
	} else if(_str_eq(line, "DIR")) {
		char* path = line + strlen(line) + 1;
		_list_directory(path);
	} else {
		_append_one_line(dup);
	}

	return result;
}

/* ========================================================} */

/*
** {========================================================
** Parameter processing
*/

#define _CHECK_ARG(__c, __i, __e) \
	do { \
		if(__c <= __i + 1) { \
			_printf(__e); \
			return true; \
		} \
	} while(0)

static void _run_file(char* path) {
	if(mb_load_file(bas, path) == MB_FUNC_OK) {
		mb_run(bas, true);
	} else {
		_printf("Invalid file or wrong program.\n");
	}
}

static void _evaluate_expression(char* p) {
	char pr[8];
	int l = 0;
	int k = 0;
	bool_t a = true;
	char* e = 0;

	const char* const print = "PRINT ";

	if(!p) {
		_printf("Invalid expression.\n");

		return;
	}

	l = (int)strlen(p);
	k = (int)strlen(print);
	if(l >= k) {
		memcpy(pr, p, k);
		pr[k] = '\0';
		if(_str_eq(pr, print))
			a = false;
	}
	if(a) {
		e = (char*)malloc(l + k + 1);
		_CHECK_MEM(e);
		memcpy(e, print, k);
		memcpy(e + k, p, l);
		e[l + k] = '\0';
		p = e;
	}
	if(mb_load_string(bas, p, true) == MB_FUNC_OK) {
		mb_run(bas, true);
	} else {
		_printf("Invalid expression.\n");
	}
	if(a)
		free(e);
}

static bool_t _process_parameters(int argc, char* argv[]) {
	int i = 1;
	char* prog = 0;
	bool_t eval = false;
	bool_t help = false;
	char* memp = 0;
	char* diri = 0;

	while(i < argc) {
		if(!memcmp(argv[i], "-", 1)) {
			if(!memcmp(argv[i] + 1, "e", 1)) {
				eval = true;
				_CHECK_ARG(argc, i, "-e: Expression expected.\n");
				prog = argv[++i];
			} else if(!memcmp(argv[i] + 1, "h", 1)) {
				help = true;
#if _USE_MEM_POOL
			} else if(!memcmp(argv[i] + 1, "p", 1)) {
				_CHECK_ARG(argc, i, "-p: Memory pool threashold size expected.\n");
				memp = argv[++i];
				if(argc > i + 1)
					prog = argv[++i];
#endif /* _USE_MEM_POOL */
			} else if(!memcmp(argv[i] + 1, "f", 1)) {
				_CHECK_ARG(argc, i, "-f: Importing directories expected.\n");
				diri = argv[++i];
			} else {
				_printf("Unknown argument: %s.\n", argv[i]);
			}
		} else {
			prog = argv[i];
		}

		i++;
	}

#if _USE_MEM_POOL
	if(memp)
		POOL_THRESHOLD_BYTES = atoi(memp);
#else /* _USE_MEM_POOL */
	mb_unrefvar(memp);
#endif /* _USE_MEM_POOL */
	if(diri)
		_set_importing_directories(diri);
	if(eval)
		_evaluate_expression(prog);
	else if(prog)
		_run_file(prog);
	else if(help)
		_show_help();
	else
		return false;

	return true;
}

/* ========================================================} */

/*
** {========================================================
** Scripting interfaces
*/

#ifdef MB_OS_WIN
#	define _OS "WINDOWS"
#elif defined MB_OS_IOS || MB_OS_IOS_SIM
#	define _OS "IOS"
#elif defined MB_OS_MAC
#	define _OS "MACOS"
#elif defined MB_OS_ANDROID
#	define _OS "ANDROID"
#elif defined MB_OS_LINUX
#	define _OS "LINUX"
#elif defined MB_OS_UNIX
#	define _OS "UNIX"
#else
#	define _OS "UNKNOWN"
#endif /* MB_OS_WIN */

#define _HAS_TICKS
#if defined MB_CP_VC || defined MB_CP_BORLANDC
static int_t _ticks(void) {
	LARGE_INTEGER li;
	double freq = 0.0;
	int_t ret = 0;

	QueryPerformanceFrequency(&li);
	freq = (double)li.QuadPart / 1000.0;
	QueryPerformanceCounter(&li);
	ret = (int_t)((double)li.QuadPart / freq);

	return ret;
}
#elif defined MB_CP_CLANG
static int_t _ticks(void) {
	struct timespec ts;
	struct timeval now;
	int rv = 0;

	rv = gettimeofday(&now, 0);
	if(rv)
		return 0;

	ts.tv_sec = now.tv_sec;
	ts.tv_nsec = now.tv_usec * 1000;

	return (int_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
#elif defined MB_CP_GCC
static int_t _ticks(void) {
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (int_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
#else /* MB_CP_VC || MB_CP_BORLANDC */
#	undef _HAS_TICKS
#endif /* MB_CP_VC || MB_CP_BORLANDC */

#ifdef _HAS_TICKS
static int ticks(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_attempt_close_bracket(s, l));

	mb_check(mb_push_int(s, l, _ticks()));

	return result;
}
#endif /* _HAS_TICKS */

static int now(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	time_t ct;
	struct tm* timeinfo = 0;
	char buf[80];
	char* arg = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	if(mb_has_arg(s, l)) {
		mb_check(mb_pop_string(s, l, &arg));
	}

	mb_check(mb_attempt_close_bracket(s, l));

	time(&ct);
	timeinfo = localtime(&ct);
	if(arg) {
		strftime(buf, sizeof(buf), arg, timeinfo);
		mb_check(mb_push_string(s, l, mb_memdup(buf, (unsigned)(strlen(buf) + 1))));
	} else {
		arg = asctime(timeinfo);
		mb_check(mb_push_string(s, l, mb_memdup(arg, (unsigned)(strlen(arg) + 1))));
	}

	return result;
}

static int os(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_attempt_close_bracket(s, l));

	mb_check(mb_push_string(s, l, mb_memdup(_OS, (unsigned)(strlen(_OS) + 1))));

	return result;
}

static int sys(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	char* arg = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_string(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	if(arg)
		system(arg);

	return result;
}

static int trace(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	char* frames[16];
	int i = 0;

	mb_assert(s && l);

	memset(frames, 0, sizeof(frames));

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_attempt_close_bracket(s, l));

	mb_check(mb_debug_get_stack_trace(s, l, frames, countof(frames)));

	for(i = 1; i < countof(frames); ) {
		if(frames[i]) {
			_printf("%s", frames[i]);
		}
		if(++i < countof(frames) && frames[i]) {
			_printf(" <- ");
		}
	}

	return result;
}

static int raise(struct mb_interpreter_t* s, void** l) {
	int result = MB_EXTENDED_ABORT;
	int_t err = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	if(mb_has_arg(s, l)) {
		mb_check(mb_pop_int(s, l, &err));
	}

	mb_check(mb_attempt_close_bracket(s, l));

	mb_check(mb_push_int(s, l, err));

	result = mb_raise_error(s, l, SE_EA_EXTENDED_ABORT, MB_EXTENDED_ABORT + err);

	return result;
}

static int gc(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	int_t collected = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_attempt_close_bracket(s, l));

	mb_check(mb_gc(s, &collected));

	mb_check(mb_push_int(s, l, collected));

	return result;
}

static int beep(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_attempt_func_end(s, l));

	putchar('\a');

	return result;
}

/* ========================================================} */

/*
** {========================================================
** Callbacks and handlers
*/

static int _on_stepped(struct mb_interpreter_t* s, void** l, const char* f, int p, unsigned short row, unsigned short col) {
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(f);
	mb_unrefvar(p);
	mb_unrefvar(row);
	mb_unrefvar(col);

	return MB_FUNC_OK;
}

static void _on_error(struct mb_interpreter_t* s, mb_error_e e, const char* m, const char* f, int p, unsigned short row, unsigned short col, int abort_code) {
	mb_unrefvar(s);
	mb_unrefvar(p);

	if(e != SE_NO_ERR) {
		if(f) {
			if(e == SE_RN_WRONG_FUNCTION_REACHED) {
				_printf(
					"Error:\n    Ln %d, Col %d in Func: %s\n    Code %d, Abort Code %d\n    Message: %s.\n",
					row, col, f,
					e, abort_code,
					m
				);
			} else {
				_printf(
					"Error:\n    Ln %d, Col %d in File: %s\n    Code %d, Abort Code %d\n    Message: %s.\n",
					row, col, f,
					e, e == SE_EA_EXTENDED_ABORT ? abort_code - MB_EXTENDED_ABORT : abort_code,
					m
				);
			}
		} else {
			_printf(
				"Error:\n    Ln %d, Col %d\n    Code %d, Abort Code %d\n    Message: %s.\n",
				row, col,
				e, e == SE_EA_EXTENDED_ABORT ? abort_code - MB_EXTENDED_ABORT : abort_code,
				m
			);
		}
	}
}

static int _on_import(struct mb_interpreter_t* s, const char* p) {
	if(!importing_dirs)
		return MB_FUNC_ERR;

	if(!_try_import(s, p))
		return MB_FUNC_ERR;

	return MB_FUNC_OK;
}

/* ========================================================} */

/*
** {========================================================
** Initialization and disposing
*/

static void _on_startup(void) {
#if _USE_MEM_POOL
	_open_mem_pool();

	mb_set_memory_manager(_pop_mem, _push_mem);
#endif /* _USE_MEM_POOL */

	_create_code();

#ifdef _HAS_TICKS
	srand((unsigned)_ticks());
#endif /* _HAS_TICKS */

	mb_init();

	mb_open(&bas);

	mb_debug_set_stepped_handler(bas, _on_stepped);
	mb_set_error_handler(bas, _on_error);
	mb_set_import_handler(bas, _on_import);

#ifdef _HAS_TICKS
	mb_reg_fun(bas, ticks);
#endif /* _HAS_TICKS */
	mb_reg_fun(bas, now);
	mb_reg_fun(bas, os);
	mb_reg_fun(bas, sys);
	mb_reg_fun(bas, trace);
	mb_reg_fun(bas, raise);
	mb_reg_fun(bas, gc);
	mb_reg_fun(bas, beep);
}

static void _on_exit(void) {
	_destroy_importing_directories();

	if(bas)
		mb_close(&bas);

	mb_dispose();

	_destroy_code();

#if _USE_MEM_POOL
	_close_mem_pool();
#endif /* _USE_MEM_POOL */

#ifdef MB_CP_VC
	if(!!_CrtDumpMemoryLeaks()) { _CrtDbgBreak(); }
#elif _USE_MEM_POOL
	if(alloc_count > 0 || alloc_bytes > 0) { mb_assert(0 && "Memory leak."); }
#endif /* MB_CP_VC */
}

/* ========================================================} */

/*
** {========================================================
** Entry
*/

int main(int argc, char* argv[]) {
	int status = 0;

#ifdef MB_CP_VC
	_CrtSetBreakAlloc(0);
#endif /* MB_CP_VC */

	atexit(_on_exit);

	if(setjmp(mem_failure_point)) {
		_printf("Error: out of memory.\n");

		exit(1);
	}

	_on_startup();

	if(argc >= 2) {
		if(!_process_parameters(argc, argv))
			argc = 1;
	}
	if(argc == 1) {
		_show_tip();
		do {
			status = _do_line();
		} while(_NOT_FINISHED(status));
	}

	return 0;
}

/* ========================================================} */

#ifdef __cplusplus
}
#endif /* __cplusplus */
