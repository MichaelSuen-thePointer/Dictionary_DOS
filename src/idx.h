#ifndef IDX_H
#define IDX_H

#include "basic.h"
#include "regex.h"
#include "vector.h"
#include "strext.h"

#define DICT_OPENFAILED -1
#define DICT_ALLOCATIONFAILED -2
#define DICT_BUFFERLEN 2048

typedef struct tag_IDX{
	char m_entry[20];
	long m_libOffset;
	short xlat_len;
	short exam_len;
} IDX;


mint			Dict_readdic			(void);
mint			Dict_openLib			(void);
mint			Dict_closeLib			(void);
char*			Dict_getExplanation		(void);
char*			Dict_getExample			(void);
size_t			Dict_getIndex			(const char* word);
char*			Dict_fixnewline			(char*explanation);
void			Dict_getInfo			(IDX* pidx);
const Vector	Dict_getWordList		(void);
size_t			Dict_getWordCount		(void);
void			Dict_freewordlist		(void);
Vector			Dict_search				(FSM fsm);
IDX*			Dict_getIDX				(size_t index);

#endif
