#include "assert.h"
#ifndef NDEBUG
const Except_T Assert_Failed = {"Assertion failed"};
void(assert)(int e)
{
	assert(e);
}
#endif
