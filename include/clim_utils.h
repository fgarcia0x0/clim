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
#define CLIM_CHAR16_BITS_CNT ((sizeof(char16_t)) * (CHAR_BIT))

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
        else
			count += 3;  
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

clim_errcode_t clim_utf16_to_utf8(
	const char16_t* restrict input, 
	char* restrict output, 
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

#endif
