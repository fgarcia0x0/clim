#ifndef CLIM_UTILS_H
#define CLIM_UTILS_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include <stdlib.h>
#include <uchar.h>
#include <string.h>
#include <limits.h>

#include "clim_base.h"
#ifdef CLIM_DEBUG_MODE
	#include "clim_logger.h"
#endif

#ifdef CLIM_HAVE_MEMMEM
	#define CLIM_MEMMEM(h, hl, n, nl) (memmem((h), (hl), (n), (nl)))
#else
	#define CLIM_MEMMEM(h, hl, n, nl) (clim_mem_search((h), (hl), (n), (nl)))
#endif

#define CLIM_MEMMEM_REV(h, hl, n, nl) (clim_mem_search_rev((h), (hl), (n), (nl)))

#define CLIM_MAX_ONE_BYTE_UTF8   0x007f // ASCII
#define CLIM_MAX_TWO_BYTE_UTF8   0x07ff
#define CLIM_MAX_THREE_BYTE_UTF8 0xFFFF
#define CLIM_MAX_FOUR_BYTE_UTF8  0x10FFFF

static inline uint32_t clim_alpha_blend_pixels(
	const uint32_t p1, const uint32_t p2
)
{
	const uint32_t AMASK = 0xFF000000;
	const uint32_t RBMASK = 0x00FF00FF;
	const uint32_t GMASK = 0x0000FF00;
	const uint32_t AGMASK = AMASK | GMASK;
	const uint32_t ONEALPHA = 0x01000000;
	const uint32_t alpha = (p2 & AMASK) >> 24;
	const uint32_t na = 0xffu - alpha;
	const uint32_t rb = ((na * (p1 & RBMASK)) + (alpha * (p2 & RBMASK))) >> 8;
	const uint32_t ag = (na * ((p1 & AGMASK) >> 8)) + (alpha * (ONEALPHA | ((p2 & GMASK) >> 8)));
	return ((rb & RBMASK) | (ag & AGMASK));
}

static inline size_t clim_get_utf8_encode_len(const char16_t* pstr16)
{
	CLIM_ASSERT(pstr16);
    size_t count = 0;

    for (const char16_t* p = pstr16; *p; ++p)
    {
        if (*p <= CLIM_MAX_ONE_BYTE_UTF8)
            ++count;
        else if (*p <= CLIM_MAX_TWO_BYTE_UTF8)
            count += 2;
        else if (*p < CLIM_MAX_THREE_BYTE_UTF8)
			count += 3;
		else
			break;
    }

    return count;
}

static inline size_t clim_get_utf8_encode_len_32(const char32_t* pstr32)
{
	CLIM_ASSERT(pstr32);
    size_t count = 0;

    for (const char32_t* p = pstr32; *p; ++p)
    {
        if (*p <= CLIM_MAX_ONE_BYTE_UTF8)
            ++count;
        else if (*p <= CLIM_MAX_TWO_BYTE_UTF8)
            count += 2;
        else if (*p <= CLIM_MAX_THREE_BYTE_UTF8)
			count += 3;
		else if (*p <= CLIM_MAX_FOUR_BYTE_UTF8)
			count += 4;
		else
			break;
    }

    return count;
}

static inline size_t clim_strlen16(const char16_t* pstr16)
{
	CLIM_ASSERT(pstr16);
	const char16_t* ptracker = pstr16;
	for (; *ptracker; ++ptracker)
		;
	return (size_t)(ptracker - pstr16);
}

static inline size_t clim_strlen32(const char32_t* pstr32)
{
	CLIM_ASSERT(pstr32);
	const char32_t* ptracker = pstr32;
	for (; *ptracker; ++ptracker)
		;
	return (size_t)(ptracker - pstr32);
}

/**
 * @brief      Maps a UTF-16 character set to its UTF-8 counterpart
 *
 * @param[in]  input                    A pointer to a UTF-16 encoded character
 *                                      set
 * @param      output                   The destination buffer where UTF-8
 *                                      multi-byte characters will be written
 * @param[in]  output_len               The size of output buffer
 * @param      pout_utf8_bytes_written  The amount of bytes written in output
 *
 * @return     The clim errcode: CLIM_EC_INVALID_PARAMETERS or CLIM_EC_SUCCESS
 *             or CLIM_EC_CONVERSION_ERROR
 */
clim_errcode_t clim_utf16_to_utf8(
	const char16_t* restrict input, 
	char8_t* restrict output, 
	const size_t output_len,
	size_t* restrict pout_utf8_bytes_written
);

/**
 * @brief      Maps a UTF-32 character set to its UTF-8 counterpart
 *
 * @param[in]  input                    A pointer to a UTF-32 encoded character
 *                                      set
 * @param      output                   The destination buffer where UTF-8
 *                                      multi-byte characters will be written
 * @param[in]  output_len               The size of output buffer
 * @param      pout_utf8_bytes_written  The amount of bytes written in output
 *
 * @return     The clim errcode: CLIM_EC_INVALID_PARAMETERS or CLIM_EC_SUCCESS
 *             or CLIM_EC_CONVERSION_ERROR
 */
clim_errcode_t clim_utf32_to_utf8(
	const char32_t* restrict input,
	char8_t* restrict output,
	const size_t output_len,
	size_t* restrict pout_utf8_bytes_written
);

void* clim_mem_search(
	void* haystack, size_t haystack_len,
	const void* needle, const size_t needle_len
);

void* clim_mem_search_rev(
	void* haystack, size_t haystack_len,
	const void* needle, const size_t needle_len
);

static inline size_t clim_strnlen(const char* src, size_t n)
{
	CLIM_ASSERT(src && n);
	const char* p = memchr(src, 0, n);
	return p ? (size_t)(p - src) : n;
}

size_t clim_strlcpy(char* dest, const char* src, size_t dest_size);

static inline size_t clim_strlcat(
	char* dest, const char* src, size_t dest_size
)
{
	CLIM_ASSERT(dest && src && dest_size);
	size_t sz = clim_strnlen(dest, dest_size);
	if (sz == dest_size) 
        return sz + strlen(src);
	return sz + clim_strlcpy(dest + sz, src, dest_size - sz);
}

static inline void* clim_mem_dup(const void* ptr, size_t sz) 
{
	CLIM_ASSERT(ptr && sz);
    void* dest = clim_mem_alloc(sz, false);
    return memcpy(dest, ptr, sz);
}

static inline void* clim_mem_dup_n(
    const void* ptr, 
    size_t copy_sz, 
    size_t alloc_sz
)
{
	CLIM_ASSERT(ptr && copy_sz && alloc_sz);
    void* dest = clim_mem_alloc(alloc_sz, true);
    const size_t sz = (alloc_sz < copy_sz) ? alloc_sz : copy_sz;
    return memcpy(dest, ptr, sz);
}

static inline char* clim_strdup(const char* pstr)
{
	CLIM_ASSERT(pstr);
	const size_t len = strlen(pstr) + 1;
	return clim_mem_dup(pstr, len);
}

static inline char* clim_strndup(const char* pstr, const size_t size)
{
	CLIM_ASSERT(pstr && size);
	const size_t len = clim_strnlen(pstr, size);
	return clim_mem_dup_n(pstr, len, len + 1);
}

static inline char8_t* clim_utf8_dupl_from_utf16(
	const char16_t* pstr16, size_t* pout_length
)
{
	if (!pstr16)
		return NULL;

	const size_t needed_len = clim_get_utf8_encode_len(pstr16);
	if (!needed_len)
		return NULL;

	char8_t* pstr8 = (char8_t *) clim_mem_alloc(needed_len + 1, false);
	size_t writted = 0;
	
	clim_errcode_t errc = clim_utf16_to_utf8(
		pstr16, pstr8, needed_len, &writted
	);

	if (CLIM_FAILED(errc))
	{
		#ifdef CLIM_DEBUG_MODE
			CLIM_LOG_DEBUG("%s(): %s", __func__, clim_err_get_msg(errc));
		#endif

		clim_mem_release(pstr8);
		return NULL;
	}

	if (writted != needed_len)
	{
		clim_mem_release(pstr8);
		return NULL;
	}

	if (pout_length)
		*pout_length = writted;

	pstr8[needed_len] = 0;
	return pstr8;
}

static inline char8_t* clim_utf8_dupl_from_utf32(
	const char32_t* pstr32, size_t* pout_length
)
{
	if (!pstr32)
		return NULL;

	const size_t needed_len = clim_get_utf8_encode_len_32(pstr32);
	if (!needed_len)
		return NULL;

	char8_t* pstr8 = (char8_t *) clim_mem_alloc(needed_len + 1, false);
	size_t writted = 0;
	
	clim_errcode_t errc = clim_utf32_to_utf8(
		pstr32, pstr8, needed_len, &writted
	);

	if (CLIM_FAILED(errc))
	{
		#ifdef CLIM_DEBUG_MODE
			CLIM_LOG_DEBUG("%s(): %s", __func__, clim_err_get_msg(errc));
		#endif

		clim_mem_release(pstr8);
		return NULL;
	}

	if (writted != needed_len)
	{
		clim_mem_release(pstr8);
		return NULL;
	}

	if (pout_length)
		*pout_length = writted;

	pstr8[needed_len] = 0;
	return pstr8;
}

static inline void clim_print_hex_bytes_ext(
	FILE* stream, const void* buffer, 
	size_t len, bool lowercase
)
{
	CLIM_ASSERT(stream && buffer && len);

	// cast is necessary for c++ compatibility
	for (const uint8_t* pbuffer = (const uint8_t *)buffer; pbuffer && len--; ++pbuffer)
		fprintf(stream, lowercase ? "%#x " : "%#X ", *pbuffer);

	putchar('\n');
}

static CLIM_ALWAYS_INLINE void clim_print_hex_bytes(const void* buffer, size_t len)
{
	clim_print_hex_bytes_ext(stdout, buffer, len, false);
}

#endif
