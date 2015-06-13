#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"

static bool compareatom(const void* a, const void* b)
{
	return a == b ? 0 : -1;
}
Vector Vector_new(size_t size)
{
	Vector vector;
	NEW(vector);
	size = size ? size : VECTOR_BASIC_LENGTH;
	vector->m_capacity = size;
	vector->m_vector = malloc(size * sizeof(vector->m_vector[0]));
	vector->m_head = 0;
	return vector;
}

Vector Vector_push_back(Vector vector, void* ptr)
{
	if (vector->m_head == vector->m_capacity)
	{
		Vector_reserve(vector, vector->m_capacity + VECTOR_BASIC_LENGTH);
	}
	vector->m_vector[vector->m_head++] = ptr;
	return vector;
}

Vector Vector_reserve(Vector vector, size_t size)
{
	void* oldVec = vector->m_vector;
	assert(vector);
	vector->m_vector = malloc(size * sizeof(vector->m_vector[0]));
	if (vector->m_vector)
	{
		memcpy(vector->m_vector, oldVec, MIN(size, vector->m_capacity) * sizeof(vector->m_vector[0]));
		vector->m_capacity = size;
		vector->m_head = vector->m_head > size ? size : vector->m_head;
		free(oldVec);
	}
	else
	{
		fprintf(stderr, "error resizing vector");
		assert(0);
	}
	return vector;
}

Vector Vector_copy(Vector vector)
{
	Vector rvec = Vector_new(vector->m_head);
	if (rvec)
	{
		memcpy(rvec->m_vector, vector->m_vector, vector->m_head * sizeof(vector->m_vector[0]));
		rvec->m_capacity = vector->m_head;
		rvec->m_head = vector->m_head;
	}
	return rvec;
}

Vector Vector_erase(Vector vector, size_t index)
{
	size_t i;
	for (i = index; i + 1 < vector->m_head; i++)
	{
		vector->m_vector[i] = vector->m_vector[i + 1];
	}
	vector->m_head--;
	return vector;
}

bool Vector_member(Vector vector, void* ptr, bool(*compare)(const void* a, const void* b))
{
	size_t i;
	if (compare == NULL)
	{
		compare = compareatom;
	}
	for (i = 0; i < vector->m_head; i++)
	{
		if (compare(vector->m_vector[i], ptr) == 0)
		{
			return true;
		}
	}
	return false;
}

void* Vector_getPointer(Vector vector, size_t index)
{
	assert(vector);
	return vector->m_vector[index];
}
#define Vector_getValue(vector, index, type) (*((type*)Vector_getPointer(vector, index)))
size_t Vector_getLength(Vector vector)
{
	assert(vector);
	return vector->m_head;
}
size_t Vector_getCapacity(Vector vector)
{
	assert(vector);
	return vector->m_capacity;
}
void Vector_free(Vector* vector)
{
	assert(vector);
	free((*vector)->m_vector);
	free(*vector);
	*vector = NULL;
}

void** Vector_getRawPointer(Vector vector)
{
	assert(vector);
	return vector->m_vector;
}

void* Vector_pop_back(Vector vector)
{
	assert(vector);
	assert(vector->m_head);
	vector->m_head--;
	return vector->m_vector[vector->m_head];
}
