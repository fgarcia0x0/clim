#ifndef CLIM_FILESYSTEM_TYPES_H
#define CLIM_FILESYSTEM_TYPES_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#if CLIM_OS_WIN
	#define stat64 _stat64
#else
	#define __USE_LARGEFILE64
	#define _LARGEFILE_SOURCE
	#define _LARGEFILE64_SOURCE
#endif

#ifdef CLIM_OS_WIN
	#define CLIM_PATH_SEPERATOR '\\'
#else
	#define CLIM_PATH_SEPERATOR '/'
#endif


#include <stddef.h>
#include "clim_base.h"

#define CLIM_FS_BUFFER_INIT(buffer, ...) \
	do { buffer = (clim_buffer_t){__VA_ARGS__}; } while(0)

#define CLIM_FS_BUFFER_RELEASE(buffer) 		\
	do 										\
	{ 										\
		clim_mem_release(buffer.data); 		\
		memset(&buffer, 0, sizeof(buffer));  \
	} while(0)

typedef struct
{
	void* data;
	size_t size;
} clim_buffer_t;

#endif
