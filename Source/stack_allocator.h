#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define EUTS_STACK_ALIGNMENT 4

struct EUTS_StackAllocator
{
	unsigned char	*data;
	unsigned char	*top;
	int				size; // Total allocator size in bytes
	int				used; // Used memory in bytes 
};

void EUTS_StackAllocator_initialize(EUTS_StackAllocator *stack, int size)
{
	stack->data = (unsigned char*)malloc(size);
	stack->data = stack->top;
	stack->size = size;
	stack->used = 0;
}

// TODO: Add alignment!
void* EUTS_StackAllocator_alloc(EUTS_StackAllocator *stack, int size)
{
	stack->used += size;
	assert(stack->used <= stack->size);
	void *pointerToData = stack->top;
	stack->top = stack->top + size;
	return pointerToData;
}

void EUTS_StackAllocator_clear(EUTS_StackAllocator *stack)
{
	stack->top = stack->data;
	stack->used = 0;
}

void EUTS_StackAllocator_finalize(EUTS_StackAllocator *stack)
{
	free(stack->data);
}