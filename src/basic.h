/*
 *作者：孙明琦
 *本文件为tslib，C语言扩展库的基础头文件，包含基础定义
 *
 *tslib为孙明琦为方便C语言开发而开发的一套C语言基础设施扩展库，
 *模拟C++STL的使用
 *包含vector，set，table(map)，hashtable，graph，tree等常用数据结构
 *安全性封装后的malloc和free ———— mem.h
 *setjump，longjump实现的异常跳转机制except.h,assert.h
 *扩展精度算法extendedpoint.h
 *位向量bitvector.h
 *以及两个内存池allocator.h和memchk.h
 *（还有尚未完工的线程池）
 *
 *本库不受任何开源协议保护，您可以随意使用，欢迎提出修改意见
 *
 **/





/************************************************************************/
/* tslib库标准头文件                                                     */
/************************************************************************/

#ifndef BASIC_H
#define BASIC_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

//global type definition
/*标准长度类型定义，兼容MSVC，Watcom C，Clang，ICC，GCC五个编译器*/
#if defined _MSC_VER || defined __WATCOMC__ || defined __clang__ || defined __ICL
typedef __int8					mint8;
typedef __int16					mint16;
typedef __int32					mint32;
typedef __int64					mint64;
typedef unsigned __int8			muint8;
typedef unsigned __int16		muint16;
typedef unsigned __int32		muint32;
typedef unsigned __int64		muint64;
typedef mint32					mint;
typedef muint32					muint;
typedef mint32					msint;
typedef muint8					mbyte;
typedef muint16					mword;
typedef muint32					mdword;
typedef muint64					mqword;

#else //for GCC
#include <stdint.h>
typedef int8_t					mint8;
typedef int16_t					mint16;
typedef int32_t					mint32;
typedef int64_t					mint64;
typedef uint8_t					muint8;
typedef uint16_t				muint16;
typedef uint32_t				muint32;
typedef uint64_t				muint64;
typedef mint32					mint;
typedef mint32					msint;
typedef muint32					muint;
typedef muint8					mbyte;
typedef muint16					mword;
typedef muint32					mdword;
typedef muint64					mqword;

#endif

//可拷贝的字符数组
struct tag_string {
	char words[255];
};
typedef struct tag_string string255;	//255 length copyable string

//bool类型定义
#ifdef bool
#undef bool
#endif
typedef int bool;

#ifdef true
#undef true
#endif
#define true 1

#ifdef false
#undef false
#endif
#define false 0

/************************************************************************/
/*内联函数标识符（C99标准已提出C语言支持内联函数），实验性，尚未成功，暂时屏蔽*/
/************************************************************************/
#ifdef INLINE
#undef INLINE
#endif

#define INLINE
#define __INVALID_INLINE__	//INLINE标识符无效时此标识符生效
/*
#if defined _MSC_VER && _MSC_VER >= 1800
#define INLINE
#define __INVALID_INLINE__
#elif defined __GNUC__ && defined __GNUC_STDC_INLINE__
#define INLINE inline
#elif defined __WATCOMC__ && defined __INLINE_FUNCTIONS__
#define INLINE inline
#elif defined __clang__
#define INLINE inline
#elif defined __ICL && __STDC_VERSION__ >= 199901L
#define INLINE
#define __INVALID_INLINE__
#endif
*/

#define BREAK_IF(condition) if(condition) break
#define RETURN_IF(condition) if(condition) return
#define INFINITE_LOOP while(1)
#define streq(str1, str2) (strcmp((str1), (str2)) == 0)	//字符串比较
#define UNUSED_PARAM(c) (c)
#define NEW(ptr) (ptr = malloc(sizeof(*ptr)), (void)0)	//模仿C++的new
#define NEW0(ptr) (New(ptr), memset(ptr, 0, sizeof(*ptr)))	//new后赋值为0
#define MAX(a, b) ((a) > (b) ? (a) : (b))	//大小比较
#define MIN(a, b) ((a) < (b) ? (a) : (b))	//大小比较
#define SWAP(a, b) do{if((a) != (b)){(a) = (a) ^ (b); (b) = (a) ^ (b); (a) = (a) ^ (b);}}while(0)
#ifndef MakeHeap	//将变量转为堆内存存储，配合tslib设施使用
#define MakeHeap(value) _MakeHeap(&value, sizeof(value))
#define bineq(a, b) (assert(sizeof(a) == sizeof(b)), binEqual(&(a), &(b), sizeof(a)))
#endif

bool	iswchar		(const char c);	//检查是否为宽字符
void*	_MakeHeap	(void* value, size_t size);
bool	binEqual	(const void* a, const void* b, size_t size);

#endif
