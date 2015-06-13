/*includes*/
#include <string.h>
#include <limits.h>
#include "assert.h"
#include "mem.h"
#include "atom.h"
/*macros*/
#define NELEMS(x) ((sizeof(x))/(sizeof((x)[0])))
/*data*/
static struct atom
{
	struct atom* link;
	int len;
	char* str;
}*buckets[2048];
/*functions*/
const char* Atom_string(const char* str)
{
	assert(str);
	return Atom_new(str, strlen(str));
}

const char* Atom_int(long n)
{
	char str[44];
	char* s = str + sizeof(str);
	unsigned long m;

	*--s = '\0';
	if (n == LONG_MIN)
		m = LONG_MAX + 1UL;
	else if (n < 0)
		m = -n;
	else
		m = n;
	do
		*--s = m % 10 + '0';
	while ((m /= 10) > 0);
	if (n < 0)
		*--s = '-';
	return Atom_new(s, (str + sizeof(str)) - s - 1);
}

const char* Atom_new(const char* str, int len)
{
	unsigned long h;
	struct atom* p;

	assert(str);
	assert(len >= 0);
	/*h = hash str*/
	p = Atom_find(str, len, &h);
	if (p)
	{
		return p->str;
	}
	/*allocate a new entry*/
	p = ALLOC(sizeof(*p) + len + 1);
	p->len = len;
	p->str = (char*)(p + 1);
	if (len > 0)
		memcpy(p->str, str, len);
	p->str[len] = '\0';
	p->link = buckets[h];
	buckets[h] = p;
	return p->str;
}

int Atom_length(const char* str)
{
	struct atom* p;
	int i;

	assert(str);
	for (i = 0; i < NELEMS(buckets); i++)
		for (p = buckets[i]; p != NULL; p = p->link)
			if (p->str == str)
				return p->len;
	assert(0);
	return 0;
}

void Atom_delete(const char* str)
{
	struct atom* p, *prev;
	unsigned long h;
	p = Atom_find(str, strlen(str), &h);
	if (p)
	{
		if (buckets[h] == p)
		{
			buckets[h] = p->link;
		}
		else
		{
			prev = buckets[h];
			while (prev && prev->link != p)
			{
				prev = prev->link;
			}
			assert(prev);
			prev->link = p->link;
		}
		FREE(p);
	}
}

extern void* Atom_find(const char* str, const int len, unsigned long* hash)
{
	int i;
	unsigned long h;
	struct atom* p;
	assert(str);
	for (h = 0, i = 0; i < len; i++)
		h = (h << 1) + scatter[(unsigned char)str[i]];
	h &= NELEMS(buckets) - 1;
	if (hash)
	{
		*hash = h;
	}
	for (p = buckets[h]; p != NULL; p = p->link)
	{
		if (len == p->len)
		{
			for (i = 0; i < len && p->str[i] == str[i];)
				i++;
			if (i == len)
				return p;
		}
	}
	assert(p == NULL);	//p must be NULL at the end of the loop
	return p;
}
