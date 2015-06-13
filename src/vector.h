#ifndef VECTOR_H
#define VECTOR_H
#include "basic.h"

#define VECTOR_BASIC_LENGTH 10
struct tag_vector {
	void**			m_vector;
	size_t			m_capacity;
	size_t			m_head;
};
typedef struct tag_vector* Vector;

Vector Vector_new(size_t size);
Vector Vector_push_back(Vector vec, void* ptr);
Vector Vector_reserve(Vector vec, size_t size);
Vector Vector_copy(Vector vector);
Vector Vector_erase(Vector vector, size_t index);
bool Vector_member(Vector vector, void* ptr, bool(*compare)(const void* a, const void* b));
void** Vector_getRawPointer(Vector vector);
#define Vector_foreach(elemtype, elemname, vector)							\
	do{																		\
		size_t _i;															\
		elemtype elemname;													\
		for(_i = 0; _i < (vector)->m_head &&									\
				   (elemname = (elemtype)((vector)->m_vector[_i]), true);	\
			_i++)											

/*end of size_t _i scope*/
#define Vector_loopend														\
		}while(0)
#define Vector_rforeach(elemtype, elemname, vector)							\
	do{																		\
		size_t _i;															\
		elemtype elemname;													\
		for(_i = (vector)->m_head - 1; _i != (size_t)-1 &&									\
				   (elemname = (elemtype)((vector)->m_vector[_i]), true);	\
			_i--)											

/*
#if defined __INVALID_INLINE__
#define Vector_getPointer(vector, index) ((vector)->m_vector[(index)])
#define Vector_getValue(vector, index, type) (*((type*)Vector_getPointer(vector, index)))
#define Vector_getLength(vector) (vector->m_head)
#define Vector_getCapacity(vector) (vector->m_capacity)
#define Vector_free(vector) do{free((vector)->m_vector); free(vector); vector = NULL;}while(0);

#else*/
void* Vector_getPointer(Vector vector, size_t index);
#define Vector_getValue(vector, index, type) (*((type*)Vector_getPointer(vector, index)))
size_t Vector_getLength(Vector vector);
size_t Vector_getCapacity(Vector vector);
void Vector_free(Vector* vector);
void* Vector_pop_back(Vector vector);

//#endif 

#endif // !VECTOR_H
