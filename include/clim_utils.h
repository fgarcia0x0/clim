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

#define CLIM_JPG_SIGNATURE U8("\xFF\xD8\xFF\xE0\xFF\xD8\xFF\xE1\xFF\xD8\xFF\xE2\xFF\xD8\xFF\xE3\xFF\xD8\xFF\xE8")
#define CLIM_PNG_SIGNATURE U8("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A")
#define CLIM_BMP_SIGNATURE U8("\x42\x4D")
#define CLIM_PGM_SIGNATURE U8("\x50\x32\x0A")
#define CLIM_PPM_SIGNATURE U8("\x50\x33\x0A")
#define CLIM_PBM_SIGNATURE U8("\x50\x31\x0A")

typedef int(*clim_compare_predicate)(const void*, const void*);
typedef int(*clim_compare_string_predicate)(const char*, const char*);
typedef char* (*clim_search_predicate)(const char* s1, const char* s2);

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
static clim_errcode_t clim_utf16_to_utf8(
	const char16_t* restrict input,
	char8_t* restrict output,
	const size_t output_len,
	size_t* restrict pout_utf8_bytes_written
)
{
	if (!input || !output)
		return CLIM_EC_INVALID_PARAMETERS;

	const char16_t* pinput = input;
	size_t inputlen = clim_strlen16(input);
	size_t writted_utf8_bytes = 0;

	for (; inputlen && (writted_utf8_bytes < output_len); --inputlen, ++pinput)
	{
		char16_t code = *pinput;

		if (code <= CLIM_MAX_ONE_BYTE_UTF8)
		{
			output[writted_utf8_bytes++] = (char8_t)(code & 0x7F);
		}
		else if (code <= CLIM_MAX_TWO_BYTE_UTF8)
		{
			if ((writted_utf8_bytes + 1) >= output_len)
				break;

			output[writted_utf8_bytes++] = (char8_t)(0xC0 | ((code >> 6) & 0x1F));
			output[writted_utf8_bytes++] = (char8_t)(0x80 | (code & 0x3F));
		}
		else
		{
			if ((writted_utf8_bytes + 2) >= output_len)
				break;

			output[writted_utf8_bytes++] = (char8_t)(0xE0 | ((code >> 12) & 0x0F));
			output[writted_utf8_bytes++] = (char8_t)(0x80 | ((code >> 6) & 0x3F));
			output[writted_utf8_bytes++] = (char8_t)(0x80 | (code & 0x3F));
		}
	}

	if (inputlen > 0)
		return CLIM_EC_CONVERSION_ERROR;

	if (pout_utf8_bytes_written)
		*pout_utf8_bytes_written = writted_utf8_bytes;

	return CLIM_EC_SUCCESS;
}

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
static clim_errcode_t clim_utf32_to_utf8(
	const char32_t* restrict input,
	char8_t* restrict output,
	const size_t output_len,
	size_t* restrict pout_utf8_bytes_written
)
{
	if (!input || !output)
		return CLIM_EC_INVALID_PARAMETERS;

	const char32_t* pinput = input;
	size_t inputlen = clim_strlen32(input);
	size_t writted_utf8_bytes = 0;

	for (; inputlen && (writted_utf8_bytes < output_len); --inputlen, ++pinput)
	{
		char32_t code = *pinput;

		if (code <= CLIM_MAX_ONE_BYTE_UTF8)
		{
			output[writted_utf8_bytes++] = (char8_t)(code & 0x7F);
		}
		else if (code <= CLIM_MAX_TWO_BYTE_UTF8)
		{
			if ((writted_utf8_bytes + 1) >= output_len)
				break;

			output[writted_utf8_bytes++] = (char8_t)(0xC0 | ((code >> 6) & 0x1F));
			output[writted_utf8_bytes++] = (char8_t)(0x80 | (code & 0x3F));
		}
		else if (code <= CLIM_MAX_THREE_BYTE_UTF8)
		{
			if ((writted_utf8_bytes + 2) >= output_len)
				break;

			output[writted_utf8_bytes++] = (char8_t)(0xE0 | ((code >> 12) & 0x0F));
			output[writted_utf8_bytes++] = (char8_t)(0x80 | ((code >> 6) & 0x3F));
			output[writted_utf8_bytes++] = (char8_t)(0x80 | (code & 0x3F));
		}
		else if (code <= CLIM_MAX_FOUR_BYTE_UTF8)
		{
			if ((writted_utf8_bytes + 3) >= output_len)
				break;

			output[writted_utf8_bytes++] = (char8_t)(0xF0 | ((code >> 18) & 0x07));
			output[writted_utf8_bytes++] = (char8_t)(0x80 | ((code >> 12) & 0x3F));
			output[writted_utf8_bytes++] = (char8_t)(0x80 | ((code >> 6) & 0x3F));
			output[writted_utf8_bytes++] = (char8_t)(0x80 | (code & 0x3F));
		}
		else
		{
			break;
		}
	}

	if (inputlen > 0)
		return CLIM_EC_CONVERSION_ERROR;

	if (pout_utf8_bytes_written)
		*pout_utf8_bytes_written = writted_utf8_bytes;

	return CLIM_EC_SUCCESS;
}

static const void* clim_mem_search(
	const void* haystack, size_t haystack_len,
	const void* needle, size_t needle_len
)
{
	CLIM_ASSERT(haystack && haystack_len && needle);

	if (!needle_len || haystack == needle)
		return haystack;

	const uint8_t* hptr = (const uint8_t*)haystack;
	const uint8_t* nptr = (const uint8_t*)needle;

	for (; haystack_len >= needle_len; ++hptr, --haystack_len)
	{
		if (!memcmp(hptr, nptr, needle_len))
		{
			return hptr;
		}
	}

	return NULL;
}

static const void* clim_mem_search_rev(
	const void* haystack, size_t haystack_len,
	const void* needle, size_t needle_len
)
{
	CLIM_ASSERT(haystack && haystack_len && needle);

	if (!needle_len || haystack == needle)
		return haystack;

	const uint8_t* hptr = (const uint8_t*)(haystack)+haystack_len - needle_len;
	const uint8_t* nptr = (const uint8_t*)needle;

	for (; haystack_len >= needle_len; --hptr, --haystack_len)
	{
		if (!memcmp(hptr, nptr, needle_len))
		{
			return hptr;
		}
	}

	return NULL;
}

static inline size_t clim_strnlen(const char* src, size_t n)
{
	CLIM_ASSERT(src && n);
	const char* p = memchr(src, 0, n);
	return p ? (size_t)(p - src) : n;
}

static inline size_t clim_strlcpy(char* dest, const char* src, size_t dest_size)
{
	CLIM_ASSERT(src && dest);
	char* psave = dest;

	if (!dest_size)
		return (size_t)(dest - psave) + strlen(src);

	for (; dest_size && (*dest = *src) != '\0'; dest_size--, src++, dest++)
		continue;

	*dest = 0;
	return (size_t)(dest - psave) + strlen(src);
}

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
		(void) fprintf(stream, lowercase ? "%#x " : "%#X ", *pbuffer);

	(void) putchar('\n');
}

static CLIM_ALWAYS_INLINE void clim_print_hex_bytes(const void* buffer, size_t len)
{
	clim_print_hex_bytes_ext(stdout, buffer, len, false);
}

static clim_img_format_t clim_get_img_format(FILE* img_handle)
{
	/*	
	 *					[MAGIC FILES SIGNATURE]
	 * 
	 * [PNG]: 89 50 4E 47 0D 0A 1A 0A
	 * [BMP]: 42 4D
	 * [PPM]: 50 33 0A
	 * [PGM]: 50 32 0A
	 * [PBM]: 50 31 0A
	 * [JPG]: FF D8 FF E0 or FF D8 FF E1 or FF D8 FF E2 or FF D8 FF E3 or FF D8 FF E8
	 * 
	 */

	enum { MAX_SIG_LEN = 9, JPEG_SIGS_LEN = 20 };

	uint8_t signature[MAX_SIG_LEN] = { 0 };
	if (fread(signature, MAX_SIG_LEN - 1, sizeof(uint8_t), img_handle) != sizeof(uint8_t))
		return CLIM_IMAGE_FORMAT_UNKNOWN;

	if (!memcmp(signature, CLIM_BMP_SIGNATURE, 2))
		return CLIM_IMAGE_FORMAT_BITMAP;
	else if (!memcmp(signature, CLIM_PGM_SIGNATURE, 3))
		return CLIM_IMAGE_FORMAT_PGM;
	else if (!memcmp(signature, CLIM_PPM_SIGNATURE, 3))
		return CLIM_IMAGE_FORMAT_PPM;
	else if (!memcmp(signature, CLIM_PBM_SIGNATURE, 3))
		return CLIM_IMAGE_FORMAT_PBM;
	else if (!memcmp(signature, CLIM_PNG_SIGNATURE, 8))
		return CLIM_IMAGE_FORMAT_PNG;
	else if (clim_mem_search(CLIM_JPG_SIGNATURE, JPEG_SIGS_LEN, signature, 4))
		return CLIM_IMAGE_FORMAT_JPEG;
	else
		return CLIM_IMAGE_FORMAT_UNKNOWN;
}

static inline void clim_fclose(FILE* restrict fileptr)
{
	#ifdef CLIM_DEBUG_MODE
		if (fclose(fileptr) == EOF)
		{
			CLIM_LOG_ERROR("Error to close file handle: %p", (void *)fileptr);
			exit(EXIT_FAILURE);
		}
	#else
		(void) fclose(fileptr);
	#endif
}


static char** clim_split_str_ex(
	const char* text, const char* delimiter, 
	size_t* pout_nelem, clim_search_predicate search_fn
)
{
	size_t delimiter_cnt = 0;
	size_t delimiter_len = strlen(delimiter);

	const char* temp = NULL;
	const char* txtptr = text;

	for (; (temp = search_fn(txtptr, delimiter)); txtptr = temp + delimiter_len)
		++delimiter_cnt;

	++delimiter_cnt;

	char** result = (char **) clim_mem_alloc(delimiter_cnt * sizeof(char *), false);
	char** ppresult = result;

	for (txtptr = text; true; txtptr = temp + delimiter_len)
	{
		if ((temp = search_fn(txtptr, delimiter)) == NULL)
		{
			*ppresult++ = clim_strdup(txtptr);
			break;
		}

		*ppresult++ = clim_strndup(txtptr, (size_t)(temp - txtptr));
	}

	if (pout_nelem)
		*pout_nelem = delimiter_cnt;

	return result;
}

static inline char** clim_split_str(
	const char* text, const char* delimiter,size_t* pout_nelem
)
{
	CLIM_ASSERT(text && delimiter && search_fn);
	CLIM_ASSERT(*text && *delimiter);
	return clim_split_str_ex(text, delimiter, pout_nelem, strstr);
}

#endif
