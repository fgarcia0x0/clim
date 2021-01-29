#include "..\include\clim_utils.h"
#include "..\include\clim_mem.h"

uint32_t clim_alpha_blend_pixels(const uint32_t p1, const uint32_t p2)
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

clim_err_code_t clim_utf16_to_utf8(
	const char16_t* restrict input, 
	char* restrict output,
	const size_t output_len, 
	size_t* restrict pout_bytes_written
)
{
	if (!input || !output || !output_len)
		return CLIM_EC_INVALID_PARAMETERS;

	const char16_t* input_begin_ptr = input;
  	const char16_t* input_end_ptr = input_begin_ptr;

  	for (; *input_end_ptr; ++input_end_ptr)
  		;

  	mbstate_t state = {0};
  	char temp[MB_LEN_MAX] = {0};
  	size_t written_bytes = 0;

  	for (const char16_t* p = input_begin_ptr; p != input_end_ptr; ++p)
  	{
  		size_t chars_read = c16rtomb(temp, *p, &state);

  		if(chars_read != SIZE_MAX)
  		{
  			written_bytes += chars_read;

  			if (written_bytes >= output_len)
  			{
  				if (pout_bytes_written)
  					*pout_bytes_written = SIZE_MAX;

  				return CLIM_EC_CONVERSION_ERROR;
  			}
  			
  			for (size_t i = 0; i < chars_read; ++i)
  				*output++ = temp[i];
  		}
  	}

  	*output = '\0';

  	if (pout_bytes_written)
  		*pout_bytes_written = written_bytes;

  	return CLIM_EC_SUCCESS;
}

const void* clim_mem_search(
	const void* haystack, size_t haystack_len, 
	const void* needle, const size_t needle_len
) 
{
	if (!haystack || !haystack_len || !needle)
		return NULL;

	if (!needle_len || haystack == needle)
		return haystack;

	const uint8_t* hptr = (const uint8_t *) haystack;
	const uint8_t* nptr = (const uint8_t *) needle;

	for (; haystack_len >= needle_len; ++hptr, --haystack_len)
	{
		if (!memcmp(hptr, nptr, needle_len))
		{
			return hptr;
		}
	}

    return NULL;
}

const void* clim_mem_search_rev(
	const void* haystack, size_t haystack_len, 
	const void* needle, const size_t needle_len
)
{
	if (!haystack || !haystack_len || !needle)
		return NULL;

	if (!needle_len || haystack == needle)
		return haystack;

	const uint8_t* hptr = (const uint8_t *)(haystack) + haystack_len - 1;
	const uint8_t* nptr = (const uint8_t *) needle;

	for (; haystack_len >= needle_len; --hptr, --haystack_len)
	{
		if (!memcmp(hptr, nptr, needle_len))
		{
			return hptr;
		}
	}

    return NULL;
}