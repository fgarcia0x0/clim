#ifndef CLIM_MEM_H
#define CLIM_MEM_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include <stddef.h>
#include <stdbool.h>

#define CLIM_DEFAULT_ALLOCATOR(bytes) malloc(bytes)
#define CLIM_DEFAULT_DEALLOCATOR(ptr) free(ptr)

void* clim_mem_alloc(const size_t nbytes, bool clear);
void clim_mem_release(void* mem);

#define CLIM_ALLOC_MEM(bytes) clim_mem_alloc(bytes, true)
#define CLIM_RELEASE_MEM(ptr) clim_mem_release(ptr)

#endif
