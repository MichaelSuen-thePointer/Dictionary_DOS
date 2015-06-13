#include <limits.h>
#include <stddef.h>
#include "mem.h"
#include "assert.h"
#include "arith.h"
#include "set.h"

#define T Set
#define TRUE 1
#define FALSE 0
/*types*/
typedef int(*pfcompare)(const void*, const void*);
typedef unsigned(*pfhash)(const void*);
struct T {
	size_t					m_length;
	unsigned				m_timestamp;
	pfcompare				m_compare;
	pfhash					m_hash;
	size_t					m_size;
	struct element {
		struct element*	m_next;
		const void*		m_value;
	}**						m_buckets;
};
/*\types*/
/*static functions*/
static int cmpatom(const void* x, const void* y)
{
	return x != y;
}
static unsigned hashatom(const void* x)
{
	return (unsigned long)x >> 2;
}
static T copy(T t, size_t hint)
{
	T set;

	assert(t);
	set = Set_new(hint, t->m_compare, t->m_hash);
	{
		/*for each member q in t*/
		size_t i;
		struct element* q;
		for (i = 0; i < t->m_size; i++)
		{
			for (q = t->m_buckets[i]; q != NULL; q = q->m_next)
			{
				/*for each member q in t*/
				/*add q->member to set*/

				struct element* p;
				const void* member = q->m_value;
				int i = (*set->m_hash)(member) % set->m_size;
				/*add member to set*/
				NEW(p);
				p->m_value = member;
				p->m_next = set->m_buckets[i];
				set->m_buckets[i] = p;
				set->m_length++;
				/*\add member to set*/
				/*\add q->member to set*/
			}
		}
	}
	return set;
}
/*\static functions*/
/*functions*/
T combine_if_match(T t, T s, int match)
{
	T set = Set_new(Arith_min(s->m_size, t->m_size), s->m_compare, s->m_hash);
	/*for each member q in t*/
	size_t i;
	struct element* q;
	for (i = 0; i < t->m_size; i++)
	{
		for (q = t->m_buckets[i]; q != NULL; q = q->m_next)
		{
			/*\for each member q in t*/
			if (Set_member(s, q->m_value) == match)
			{
				/*add q->member to set*/
				struct element* p;
				const void* member = q->m_value;
				size_t i = (*set->m_hash)(member) % set->m_size;
				/*add member to set*/
				NEW(p);
				p->m_value = member;
				p->m_next = set->m_buckets[i];
				set->m_buckets[i] = p;
				set->m_length++;
				/*\add member to set*/
				/*\add q->member to set*/
			}
		}
	}
	return set;
}
T Set_new(int hint,
		  int(*cmp)(const void* x, const void* y),
		  unsigned(*hash)(const void* x))
{
	T set;
	size_t i;
	static int primes[] = {509, 509, 1021, 2053,
		4093, 8191, 16381, 32771, 65521, INT_MAX};
	assert(hint >= 0);
	for (i = 1; primes[i] < hint; i++)
		/*do nothing*/;
	set = ALLOC(sizeof(*set) + primes[i - 1] * sizeof(set->m_buckets[0]));
	set->m_size = primes[i - 1];
	set->m_compare = cmp ? cmp : cmpatom;
	set->m_hash = hash ? hash : hashatom;
	set->m_buckets = (struct element**)(set + 1);
	for (i = 0; i < set->m_size; i++)
	{
		set->m_buckets[i] = NULL;
	}
	set->m_length = 0;
	set->m_timestamp = 0;
	return set;
}
int Set_member(T set, const void* member)
{
	int i;
	struct element* p;

	assert(set);
	assert(member);
	/*search set for member*/
	i = (*set->m_hash)(member) % set->m_size;
	for (p = set->m_buckets[i]; p != NULL; p = p->m_next)
	{
		if ((*set->m_compare)(member, p->m_value) == 0)
		{
			break;
		}
	}
	/*\search set for member*/
	return p != NULL;
}
void Set_put(T set, const void* member)
{
	size_t i;
	struct element* p;

	assert(set);
	assert(member);
	/*search set for member*/
	i = (*set->m_hash)(member) % set->m_size;
	for (p = set->m_buckets[i]; p != NULL; p = p->m_next)
	{
		if ((*set->m_compare)(member, p->m_value) == 0)
		{
			break;
		}
	}
	/*\search set for member*/
	if (p == NULL)
	{
		/*add member to set*/
		NEW(p);
		p->m_value = member;
		p->m_next = set->m_buckets[i];
		set->m_buckets[i] = p;
		set->m_length++;
		/*\add member to set*/
	}
	else
	{
		p->m_value = member;
	}
	set->m_timestamp++;
}
void* Set_remove(T set, const void* member)
{
	size_t i;
	struct element** pp;

	assert(set);
	assert(member);
	set->m_timestamp++;
	i = (*set->m_hash)(member) % set->m_size;
	for (pp = &set->m_buckets[i]; *pp != NULL; pp = &(*pp)->m_next)
	{
		if ((*set->m_compare)(member, (*pp)->m_value) == 0)
		{
			struct element* p = *pp;
			*pp = p->m_next;
			member = p->m_value;
			FREE(p);
			set->m_length--;
			return (void*)member;
		}
	}
	return NULL;
}
int Set_length(T set)
{
	assert(set);
	return set->m_length;
}
void Set_free(T* set)
{
	assert(set && *set);
	if ((*set)->m_length > 0)
	{
		size_t i;
		struct element* p, *q;
		for (i = 0; i < (*set)->m_size; i++)
		{
			for (p = (*set)->m_buckets[i]; p != NULL; p = q)
			{
				q = p->m_next;
				FREE(p);
			}
		}
	}
	FREE(*set);
}
void Set_map(T set, void apply(const void* m_value, void* cl), void* cl)
{
	size_t i;
	unsigned stamp;
	struct element* p;

	assert(set);
	assert(apply);
	stamp = set->m_timestamp;
	for (i = 0; i < set->m_size; i++)
	{
		for (p = set->m_buckets[i]; p != NULL; p = p->m_next)
		{
			apply(p->m_value, cl);
			assert(set->m_timestamp == stamp);
		}
	}
}
void** Set_toArray(T set, void* end)
{
	size_t i, j = 0;
	void** array;
	struct element* p;

	assert(set);
	array = CALLOC((set->m_length + 1), sizeof(*array));
	for (i = 0; i < set->m_size; i++)
	{
		for (p = set->m_buckets[i]; p != NULL; p = p->m_next)
		{
			array[j++] = (void*)p->m_value;
		}
	}
	array[j] = end;
	return array;
}
T Set_union(T s, T t)
{
	if (s == NULL)
	{
		assert(t);
		return copy(t, t->m_size);
	}
	else if (t == NULL)
	{
		return copy(s, s->m_size);
	}
	else
	{
		T set = copy(s, Arith_max(s->m_size, t->m_size));
		assert(s->m_compare == t->m_compare && s->m_hash == t->m_hash);
		{
			/*for each member q in t*/
			size_t i;
			struct element* q;
			for (i = 0; i < t->m_size; i++)
			{
				for (q = t->m_buckets[i]; q != NULL; q = q->m_next)
				{
					/*\for each member q in t*/
					Set_put(set, q->m_value);
				}
			}
		}
		return set;
	}
}
T Set_inter(T s, T t)
{
	if (s == NULL)
	{
		assert(t);
		return Set_new(t->m_size, t->m_compare, t->m_hash);
	}
	else if (t == NULL)
	{
		return Set_new(s->m_size, s->m_compare, s->m_hash);
	}
	else if (s->m_length < t->m_length)
	{
		return Set_inter(t, s);
	}
	else
	{
		//T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
		T set;
		assert(s->m_compare == t->m_compare && s->m_hash == t->m_hash);
		set = combine_if_match(t, s, TRUE);
		return set;
	}
}
T Set_minus(T s, T t)
{
	if (t == NULL)
	{
		assert(s);
		return Set_new(s->m_size, s->m_compare, s->m_hash);
	}
	else if (s == NULL)
	{
		return copy(t, t->m_size);
	}
	else
	{
		//T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
		T set;
		assert(s->m_compare == t->m_compare && s->m_hash == t->m_hash);
		set = combine_if_match(t, s, FALSE);
		return set;
	}
}
T Set_diff(T s, T t)
{
	if (s == NULL)
	{
		assert(t);
		return copy(t, t->m_size);
	}
	else if (t == NULL)
	{
		return copy(s, s->m_size);
	}
	else
	{
		//T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
		T set;
		assert(s->m_compare == t->m_compare && s->m_hash == t->m_hash);
		set = combine_if_match(s, t, FALSE);
		set = combine_if_match(t, s, FALSE);
		return set;
	}
}
#undef T
/*\functions*/
