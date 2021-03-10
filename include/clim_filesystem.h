#ifndef CLIM_FILESYSTEM_H
#define CLIM_FILESYSTEM_H

#include "clim_filesystem_types.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "clim_utils.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static inline bool clim_fs_file_exists(const char* filepath)
{
	if (!filepath || !*filepath)
		return false;

	struct stat64 stat_buf;
	errno = 0;
	int errc = stat64(filepath, &stat_buf);
	return !errc && errno != ENOENT;
}

static inline clim_errcode_t clim_fs_get_file_size(
	const char* filepath, 
	uint64_t* restrict pout_length
)
{
	if (!filepath || !*filepath)
		return false;
		
	struct stat64 stat_buf;
	
	errno = 0;
    int errc = stat64(filepath, &stat_buf);

    if (!errc)
    {
    	if (pout_length)
    		*pout_length = (uint64_t)stat_buf.st_size;
    	return CLIM_EC_SUCCESS;
    }

    return (errno == ENOENT) ? CLIM_EC_FILE_NOT_FOUND : CLIM_EC_STAT_FAILED;
}

/**
 * @brief      read entire file into memory
 *
 * @param      filepath     the path of the file to be loaded into memory
 * @param      pout_buffer  a pointer to a structure in which the reading
 *                          results will be stored
 *
 * @return     CLIM_EC_INVALID_PARAMETERS OR CLIM_EC_CANNOT_OPEN_FILE OR
 *             CLIM_EC_CANNOT_CLOSE_FILE OR CLIM_EC_CANNOT_READ_FILE OR
 *             CLIM_EC_SUCCESS
 */
static clim_errcode_t clim_fs_read_entire_file(
	const char* filepath,
	clim_buffer_t* pout_buffer
)
{
	if (!filepath || !*filepath || !pout_buffer)
		return CLIM_EC_INVALID_PARAMETERS;

	clim_errcode_t errc = CLIM_EC_SUCCESS;
	uint64_t file_len = 0;

	errc = clim_fs_get_file_size(filepath, &file_len);
	if (CLIM_FAILED(errc))
		return errc;

	void* raw_buffer = clim_mem_alloc(file_len, false);
	FILE* fileptr = fopen(filepath, "rb");

	if (!fileptr)
		return CLIM_EC_CANNOT_OPEN_FILE;
	else if (ferror(fileptr))
	{
		if (fclose(fileptr) == EOF)
			return CLIM_EC_CANNOT_CLOSE_FILE;
		return CLIM_EC_CANNOT_OPEN_FILE;
	}

	// read buffer
	if (fread(raw_buffer, file_len, sizeof(uint8_t), fileptr) != sizeof(uint8_t))
	{
		if (fclose(fileptr) == EOF)
			return CLIM_EC_CANNOT_CLOSE_FILE;

		clim_mem_release(raw_buffer);
		return CLIM_EC_CANNOT_READ_FILE;
	}

	if (fclose(fileptr) == EOF)
	{
		clim_mem_release(raw_buffer);
		return CLIM_EC_CANNOT_CLOSE_FILE;
	}

	pout_buffer->data = raw_buffer;
	pout_buffer->size = file_len;

	return errc;
}

static CLIM_ALWAYS_INLINE char* clim_extract_file_extension(const char* filepath)
{
	CLIM_ASSERT(filepath);

	char* pext = strrchr(filepath, '.');
	if (pext && *(pext + 1))
		return pext + 1;

	return NULL;
}

static inline char* clim_fs_extract_filepath(
	const char* restrict path, char* restrict dest, size_t dest_size
)
{
	CLIM_ASSERT(path && dest && dest_size);

	const char* pslash = strrchr(path, CLIM_PATH_SEPERATOR);

	if (pslash)
	{
		size_t len = MIN((size_t)(pslash - path), dest_size - 1);
		(void) clim_strlcpy(dest, path, len);
		return dest;
	}

	return NULL;
}

static inline char* clim_fs_extract_filename(
	const char* restrict path, char* restrict dest, size_t dest_size
)
{
	CLIM_ASSERT(path && dest && dest_size);

	const char* pslash = strrchr(path, CLIM_PATH_SEPERATOR);

	if (pslash)
	{
		if (++pslash)
		{
			(void) clim_strlcpy(dest, pslash, dest_size - 1);
			return dest;
		}
	}

	return NULL;
}

#endif
