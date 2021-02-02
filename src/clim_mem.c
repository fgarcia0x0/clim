#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "..\include\clim_base.h"
#include "..\include\clim_mem.h"
#include "..\include\clim_logger.h"

void* clim_alloc_mem(const size_t nbytes, bool clear)
{
	void* raw_mem = CLIM_DEFAULT_ALLOCATOR(nbytes);

	CLIM_PANIC_IF(!raw_mem, "cannot allocate %zu bytes, exausted memory", 
				  nbytes);

	if (clear) 
		memset(raw_mem, 0, nbytes);

	return raw_mem;
}

void clim_release_mem(void* mem)
{
	if (!mem) 
		CLIM_LOG_WARN("trying release null pointer");
	CLIM_DEFAULT_DEALLOCATOR(mem);
}