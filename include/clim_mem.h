#ifndef CLIM_MEM_H
#define CLIM_MEM_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "clim_logger.h"

#define CLIM_DEFAULT_ALLOCATOR(bytes) malloc(bytes)
#define CLIM_DEFAULT_DEALLOCATOR(ptr) free(ptr)

static void* clim_mem_alloc(const size_t nbytes, bool clear)
{
	CLIM_LOG_DEBUG("Alocating %zu bytes", nbytes);
	void* raw_mem = CLIM_DEFAULT_ALLOCATOR(nbytes);

	CLIM_PANIC_IF(
		!raw_mem,
		"cannot allocate %zu bytes, exausted memory",
		nbytes
	);

	#if CLIM_COMPILER_MSVC
		#pragma warning(suppress : 6387)
	#endif

	if (clear)
		memset(raw_mem, 0, nbytes);

	return raw_mem;
}

static void clim_mem_release(void* mem)
{
	if (!mem)
	{
		#ifdef CLIM_DEBUG_MODE
			CLIM_LOG_WARN("trying release null pointer");
		#endif
	}
	CLIM_DEFAULT_DEALLOCATOR(mem);
}

#define CLIM_ALLOC_MEM(bytes) clim_mem_alloc(bytes, true)
#define CLIM_RELEASE_MEM(ptr) clim_mem_release(ptr)

#endif
