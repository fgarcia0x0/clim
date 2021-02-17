#include "../include/clim_utils.h"
#include "../include/clim_mem.h"

clim_errcode_t clim_utf16_to_utf8(
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
            output[writted_utf8_bytes++] = (char8_t)(0x80 | ((code >> 6)  & 0x3F));
            output[writted_utf8_bytes++] = (char8_t)(0x80 | (code & 0x3F));
        } 
    }

	if (inputlen > 0)
		return CLIM_EC_CONVERSION_ERROR;

	if (pout_utf8_bytes_written)
		*pout_utf8_bytes_written = writted_utf8_bytes;

  	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_utf32_to_utf8(
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
            output[writted_utf8_bytes++] = (char8_t)(0x80 | ((code >> 6)  & 0x3F));
            output[writted_utf8_bytes++] = (char8_t)(0x80 | (code & 0x3F));
        } 
		else if (code <= CLIM_MAX_FOUR_BYTE_UTF8)
        {
            if ((writted_utf8_bytes + 3) >= output_len)
                break;

            output[writted_utf8_bytes++] = (char8_t)(0xF0 | ((code >> 18) & 0x07));
            output[writted_utf8_bytes++] = (char8_t)(0x80 | ((code >> 12) & 0x3F));
            output[writted_utf8_bytes++] = (char8_t)(0x80 | ((code >> 6)  & 0x3F));
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

void* clim_mem_search(
	void* haystack, size_t haystack_len,
	const void* needle, const size_t needle_len
) 
{
	CLIM_ASSERT(haystack && haystack_len && needle);
	
	if (!needle_len || haystack == needle)
		return haystack;

	uint8_t* hptr = (uint8_t *) haystack;
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

void* clim_mem_search_rev(
	void* haystack, size_t haystack_len,
	const void* needle, const size_t needle_len
)
{
	CLIM_ASSERT(haystack && haystack_len && needle);

	if (!needle_len || haystack == needle)
		return haystack;

	uint8_t* hptr = (uint8_t *)(haystack) + haystack_len - needle_len;
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

size_t clim_strlcpy(char* dest, const char* src, size_t dest_size)
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
