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

void* clim_alloc_mem(const size_t nbytes, bool clear);
void clim_release_mem(void* mem);

#define CLIM_ALLOC_MEM(bytes) clim_alloc_mem(bytes, true)
#define CLIM_RELEASE_MEM(ptr) clim_release_mem(ptr)

#endif