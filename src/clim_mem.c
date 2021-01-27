#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "..\include\clim_base.h"
#include "..\include\clim_mem.h"

void* clim_alloc_mem(const size_t nbytes, bool clear)
{
	void* raw_mem = CLIM_DEFAULT_ALLOCATOR(nbytes);

	if (!raw_mem)
	{
		fprintf(stderr, "[CRITICAL] [img_alloc_mem()] exausted memory");
		exit(EXIT_FAILURE);
	}

	if (clear) 
		memset(raw_mem, 0, nbytes);

	return raw_mem;
}

void clim_release_mem(void* mem)
{
	if (!mem) 
		fprintf(stderr, "[WARNING] [img_release_mem()] trying release null pointer");
	CLIM_DEFAULT_DEALLOCATOR(mem);
}