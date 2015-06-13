/*
 *���ߣ�������
 *���ļ�Ϊtslib��C������չ��Ļ���ͷ�ļ���������������
 *
 *tslibΪ������Ϊ����C���Կ�����������һ��C���Ի�����ʩ��չ�⣬
 *ģ��C++STL��ʹ��
 *����vector��set��table(map)��hashtable��graph��tree�ȳ������ݽṹ
 *��ȫ�Է�װ���malloc��free �������� mem.h
 *setjump��longjumpʵ�ֵ��쳣��ת����except.h,assert.h
 *��չ�����㷨extendedpoint.h
 *λ����bitvector.h
 *�Լ������ڴ��allocator.h��memchk.h
 *��������δ�깤���̳߳أ�
 *
 *���ⲻ���κο�ԴЭ�鱣��������������ʹ�ã���ӭ����޸����
 *
 **/





/************************************************************************/
/* tslib���׼ͷ�ļ�                                                     */
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
/*��׼�������Ͷ��壬����MSVC��Watcom C��Clang��ICC��GCC���������*/
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

//�ɿ������ַ�����
struct tag_string {
	char words[255];
};
typedef struct tag_string string255;	//255 length copyable string

//bool���Ͷ���
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
/*����������ʶ����C99��׼�����C����֧��������������ʵ���ԣ���δ�ɹ�����ʱ����*/
/************************************************************************/
#ifdef INLINE
#undef INLINE
#endif

#define INLINE
#define __INVALID_INLINE__	//INLINE��ʶ����Чʱ�˱�ʶ����Ч
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
#define streq(str1, str2) (strcmp((str1), (str2)) == 0)	//�ַ����Ƚ�
#define UNUSED_PARAM(c) (c)
#define NEW(ptr) (ptr = malloc(sizeof(*ptr)), (void)0)	//ģ��C++��new
#define NEW0(ptr) (New(ptr), memset(ptr, 0, sizeof(*ptr)))	//new��ֵΪ0
#define MAX(a, b) ((a) > (b) ? (a) : (b))	//��С�Ƚ�
#define MIN(a, b) ((a) < (b) ? (a) : (b))	//��С�Ƚ�
#define SWAP(a, b) do{if((a) != (b)){(a) = (a) ^ (b); (b) = (a) ^ (b); (a) = (a) ^ (b);}}while(0)
#ifndef MakeHeap	//������תΪ���ڴ�洢�����tslib��ʩʹ��
#define MakeHeap(value) _MakeHeap(&value, sizeof(value))
#define bineq(a, b) (assert(sizeof(a) == sizeof(b)), binEqual(&(a), &(b), sizeof(a)))
#endif

bool	iswchar		(const char c);	//����Ƿ�Ϊ���ַ�
void*	_MakeHeap	(void* value, size_t size);
bool	binEqual	(const void* a, const void* b, size_t size);

#endif
