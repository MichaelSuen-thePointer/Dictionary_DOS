#ifndef STREXT_H
#define STREXT_H

#include "basic.h"
#include "vector.h"
#include "set.h"
#include "atom.h"

enum enum_charClass {
	SPACE,
	DIGIT,
	ALPHA,
	HANZI,
	PUNCT
};

void		Str_freeFragments	(Vector fragments);
Vector		Str_splitByWord		(char* text);
Vector		Str_split			(char* text, char delimiter);
mint		Str_charClass		(char ch);
void		Str_strmov			(char* str, mint offset);
char*		Str_toLowerCase		(char* str);
char**		Str_extractWords	(char* text);
char*		Str_copy			(const char* text);
char*		Str_copyN			(const char* text, size_t count);
#endif
