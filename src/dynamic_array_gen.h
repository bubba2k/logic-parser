#ifndef HANS_DYNA_H_INCLUDED
#define HANS_DYNA_H_INCLUDED

/*  Version 0.2

	A quick n dirty generic dynamic array implemented with macros.

USAGE:

 Include this file in your code, then use the macro 
	DYNAMIC_ARRAY_GEN_DECL(TYPE, NAME) 
 to generate the declaration for the struct and function prototypes 
 for a dynamic array containig type TYPE and with name DA_NAME.
 All function names are of the form 'DA_NAME_[some_func]'.

 Use the macro
	DYNAMIC_ARRAY_GEN_IMPL(TYPE, NAME)
 to generate the function declarations. Note that TYPE and NAME must
 match with corresponding call to DYNAMIC_ARRAY_GEN_DECL(TYPE, NAME).
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Quick min/max macros */
#define HANS_DYNA_MIN(A, B)  ((A) < (B) ? (A) : (B));
#define HANS_DYNA_MAX(A, B)  ((A) > (B) ? (A) : (B));



#define DYNAMIC_ARRAY_GEN_DECL(TYPE, NAME) \
	typedef struct DA_##NAME \
	{ \
		TYPE *data; \
		size_t allocated; \
		size_t size; \
	} DA_##NAME; \
\
	DA_##NAME *DA_##NAME##_create(unsigned prealloc); \
	void DA_##NAME##_destroy(DA_##NAME *da); \
	void DA_##NAME##_shrink(DA_##NAME *da); \
	void DA_##NAME##_push_back(DA_##NAME *da, TYPE *element); \
	void DA_##NAME##_pop_back(DA_##NAME *da); \
	TYPE *DA_##NAME##_get(DA_##NAME *da, unsigned index); \
\
	void DA_##NAME##_for_each(DA_##NAME *da, void (*func)(TYPE *,void *), void *usr_data); \
	int DA_##NAME##_any_of(DA_##NAME *da, int (*pred)(TYPE *,    void *), void *usr_data); \
	int DA_##NAME##_all_of(DA_##NAME *da, int (*pred)(TYPE *,    void *), void *usr_data); \
	int DA_##NAME##_none_of(DA_##NAME *da, int (*pred)(TYPE *,   void *), void *usr_data); \
	TYPE *DA_##NAME##_find_if(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data); \
\
	void DA_##NAME##_for_each_range(DA_##NAME *da, void (*func)(TYPE *,void *), void *usr_data,  \
										unsigned int begin, unsigned int end); \
	int DA_##NAME##_any_of_range(DA_##NAME *da, int (*pred)(TYPE *,    void *), void *usr_data,  \
										unsigned int begin, unsigned int end); \
	int DA_##NAME##_all_of_range(DA_##NAME *da, int (*pred)(TYPE *,    void *), void *usr_data,  \
										unsigned int begin, unsigned int end); \
	int DA_##NAME##_none_of_range(DA_##NAME *da, int (*pred)(TYPE *,   void *), void *usr_data,  \
										unsigned int begin, unsigned int end); \
	TYPE *DA_##NAME##_find_if_range(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data,  \
										unsigned int begin, unsigned int end); \
\
	size_t DA_##NAME##_memsize(DA_##NAME *da); \
	size_t DA_##NAME##_size(DA_##NAME *da);

#define DYNAMIC_ARRAY_GEN_IMPL(TYPE, NAME) \
	DA_##NAME *DA_##NAME##_create(unsigned prealloc) \
	{ \
		prealloc = (prealloc < 1) ? 1 : prealloc;  \
		DA_##NAME *da    = (DA_##NAME *) malloc(sizeof(DA_##NAME)); \
		da->data	   = (TYPE *)    malloc(sizeof(TYPE) * prealloc); \
		da->allocated = prealloc; \
		da->size       =        0; \
		\
		return da; \
	} \
	\
	void DA_##NAME##_destroy(DA_##NAME *da) \
	{ \
		free(da->data); \
		free(da); \
	} \
\
	void _DA_##NAME##_realloc_expand(DA_##NAME *da) \
	{ \
		void *tmp = realloc(da->data, sizeof(TYPE) * da->allocated * 2); \
		if(!tmp) \
		{ \
			fprintf(stderr, "CRITICAL ERROR: Dynamic array (NAME): Failed to reallocate, aborting!\n"); \
			exit(EXIT_FAILURE); \
		} \
		else \
		{ \
			da->data = tmp; \
			da->allocated = da->allocated * 2; \
		} \
	} \
	void DA_##NAME##_shrink(DA_##NAME *da) \
	{ \
		if(da->size < (da->allocated / 2)) \
		{ \
			void *tmp = realloc(da->data, (da->allocated / 2) * sizeof(TYPE)); \
			if(!tmp) \
			{ \
				fprintf(stderr, "CRITICAL ERROR: Dynamic array (NAME): Failed to reallocate, aborting!\n"); \
				exit(EXIT_FAILURE); \
			} \
			else \
			{ \
				da->data = tmp; \
				da->allocated = da->allocated / 2; \
			} \
		} \
	} \
 \
	void DA_##NAME##_push_back(DA_##NAME *da, TYPE *element) \
	{ \
		if(da->size >= da->allocated) \
			_DA_##NAME##_realloc_expand(da); \
 \
		memcpy(da->data + (da->size++), element, sizeof(TYPE)); \
	} \
 \
	void DA_##NAME##_pop_back(DA_##NAME *da) \
	{ \
		if(da->size > 0) \
			da->size -= 1; \
	} \
 \
	TYPE *DA_##NAME##_get(DA_##NAME *da, unsigned index) \
	{ \
		return da->data + index; \
	} \
	size_t DA_##NAME##_memsize(DA_##NAME *da) \
	{ \
		return sizeof(TYPE) * da->allocated; \
	} \
	size_t DA_##NAME##_size(DA_##NAME *da) \
	{ \
		return da->size; \
	} \
	void DA_##NAME##_for_each(DA_##NAME *da, void (*func)(TYPE *, void *), void *usr_data) \
	{ \
		for(unsigned i = 0; i < da->size; i++) \
			func(da->data + i, usr_data); \
	} \
	void DA_##NAME##_for_each_range(DA_##NAME *da, void (*func)(TYPE *, void *), void *usr_data,  \
										unsigned int begin, unsigned int end) \
	{ \
		/* check the range */ \
		if(begin >= da->size || end > begin) return;		\
		end = HANS_DYNA_MIN((end), (da->size - 1)); \
\
		for(unsigned i = begin; i <= end; i++) \
			func(da->data + i, usr_data); \
	} \
	int DA_##NAME##_any_of(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data) \
	{ \
		for(unsigned i = 0; i < da->size; i++) \
		{ \
			if(pred(da->data + i, usr_data)) \
				return 1; \
		} \
		return 0; \
	} \
	int DA_##NAME##_any_of_range(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data,   \
										unsigned int begin, unsigned int end) \
	{ \
		/* check the range */ \
		if(begin >= da->size || end > begin) return 0;		\
		end = HANS_DYNA_MIN((end), (da->size - 1)); \
\
		for(unsigned i = begin; i <= end; i++) \
		{ \
			if(pred(da->data + i, usr_data)) \
				return 1; \
		} \
		return 0; \
	} \
	int DA_##NAME##_all_of(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data) \
	{ \
		for(unsigned i = 0; i < da->size; i++) \
		{ \
			if(!pred(da->data + i, usr_data)) \
				return 0; \
		} \
		return 1; \
	} \
	int DA_##NAME##_all_of_ranged(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data,  \
										unsigned int begin, unsigned int end) \
	{ \
		/* check the range */ \
		if(begin >= da->size || end > begin) return 0;		\
		end = HANS_DYNA_MIN((end), (da->size - 1)); \
\
		for(unsigned i = begin; i <= end; i++) \
		{ \
			if(!pred(da->data + i, usr_data)) \
				return 0; \
		} \
		return 1; \
	} \
	int DA_##NAME##_none_of(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data) \
	{ \
		for(unsigned i = 0; i < da->size; i++) \
		{ \
			if(pred(da->data + i, usr_data)) \
				return 0; \
		} \
		return 1; \
	} \
	int DA_##NAME##_none_of_ranged(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data,    \
										unsigned int begin, unsigned int end) \
	{ \
		/* check the range */ \
		if(begin >= da->size || end > begin) return 1;		\
		end = HANS_DYNA_MIN((end), (da->size - 1)); \
\
		for(unsigned i = begin; i <= end; i++) \
		{ \
			if(pred(da->data + i, usr_data)) \
				return 0; \
		} \
		return 1; \
	} \
	TYPE *DA_##NAME##_find_if(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data) \
	{ \
		for(unsigned i = 0; i < da->size; i++) \
		{ \
			if(pred(da->data + i, usr_data)) \
				return da->data + i; \
		} \
		return NULL; \
	} \
	TYPE *DA_##NAME##_find_if_ranged(DA_##NAME *da, int (*pred)(TYPE *, void *), void *usr_data,   \
										unsigned int begin, unsigned int end) \
	{ \
		/* check the range */ \
		if(begin >= da->size || end > begin) return NULL;		\
		end = HANS_DYNA_MIN((end), (da->size - 1)); \
\
		for(unsigned i = begin; i <= end; i++) \
		{ \
			if(pred(da->data + i, usr_data)) \
				return da->data + i; \
		} \
		return NULL; \
	} 

#endif
