#include "basic.h"

void* _MakeHeap(void* value, size_t size)
{
	void* ptr = malloc(size);
	if (ptr)
	{
		memcpy(ptr, value, size);
	}
	return ptr;
}

bool iswchar(const char c)
{
#ifdef _MSC_VER
	return c < 0;
#else
	return c > '~';
#endif
}

bool binEqual(const void* a, const void* b, size_t size)
{
	return memcmp(a, b, size) == 0;
}
