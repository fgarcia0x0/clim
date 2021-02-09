#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/clim_base.h"
#include "../include/clim_mem.h"
#include "../include/clim_logger.h"

void* clim_mem_alloc(const size_t nbytes, bool clear)
{
	CLIM_LOG_DEBUG("Alocating %zu bytes", nbytes);
	void* raw_mem = CLIM_DEFAULT_ALLOCATOR(nbytes);

	CLIM_PANIC_IF(
		!raw_mem, 
		"cannot allocate %zu bytes, exausted memory", 
		nbytes
	);

	if (clear) 
		memset(raw_mem, 0, nbytes);

	return raw_mem;
}

void clim_mem_release(void* mem)
{
	if (!mem) 
		CLIM_LOG_WARN("trying release null pointer");
	CLIM_DEFAULT_DEALLOCATOR(mem);
}