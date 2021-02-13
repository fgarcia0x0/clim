#include "../include/clim_utils.h"
#include "../include/clim_mem.h"

clim_errcode_t clim_utf16_to_utf8(
	const char16_t* restrict input, 
	char* restrict output,
	const size_t output_len, 
	size_t* restrict pout_utf8_bytes_written
)
{
	if (!input || !output || !output_len)
		return CLIM_EC_INVALID_PARAMETERS;

	const char16_t* pinput = input;
    size_t writted_utf8_bytes = 0;
	size_t srclen = clim_strlen16(input);
    char16_t code = 0;

    for (; srclen && (writted_utf8_bytes < output_len); --srclen, ++pinput)
    {
        code = *pinput;

        if (code <= CLIM_MAX_ONE_BYTE_UTF8) // ASCII
        {
            output[writted_utf8_bytes++] = (char)(code);
        }
        else if (code <= CLIM_MAX_TWO_BYTE_UTF8)
        {
            if ((writted_utf8_bytes + 1) >= output_len)
                break;

           /*
            * 							[2-byte UTF-8] 
            * 
            * Author: Felipe Garcia 
            * 0b = binary constant
            *  
            * char16_t = 0bxxxxxxxx 0bxxxxxxxx
            *                ^           ^
            *           first byte   second byte
            *           
            *  we want to turn this into a utf8 representation:
            *  utf8 = 0b110xxxxx 0b10xxxxxx
            *           
            *  Notes: [ISO C DOES NOT SUPPORT BINARY CONSTANTS]
            *  0x1F = 0b00011111
            *  0xC0 = 0b11000000
            *  0x80 = 0b10000000
            *  0x3F = 0b00111111
            *  
            *  code & 0b00011111 => preserve the fiver lower bits of code
            *  code & 0b00111111 => preserve the six lower bits of code
            *  
            *  0b10000000 | (code & 0b00111111)
            *  => set the first two high-order bits of the second 
            *     byte (lowest byte) and preserve the rest of the code bits
            *     
            *  θ = (code >> (char16_size_bits >> 1))
            *  => jump to first byte(high order byte) of char16_t
            *  
            *  θ | ((code & 0b00011111) | 0b11000000);
            *  => jump to first byte(high order byte) of char16_t
            *     and set the first three bits and preserve 
            *     the rest of the code bits
            */

            code = *pinput;

            // setup first byte of 'code' (highest byte)
            // ignore the eight first lower bits and set 3 high bits
            output[writted_utf8_bytes++] = (char)((code >> (CLIM_CHAR16_BITS_CNT >> 1)) | ((code & 0x1F) | 0xC0));

            // setup second byte of 'code' (lowest byte)
            output[writted_utf8_bytes++] = (char)(0x80 | (code & 0x3F));
        }
        else
        {
            if ((writted_utf8_bytes + 2) >= output_len)
                break;

            /*               		[3-byte UTF-8] 
             *               
             *  Author: Felipe Garcia
             *  0b = binary constant
             *  
             *  0b1110xxxx   0b10xxxxxx   0b10xxxxxx
             *      ^             ^           ^ 
             *  first byte   second byte  third byte
             *  
             *  Notes:
             *  0x3F = 0b00111111
             *  0x80 = 0b10000000
             *  0xE0 = 0b11100000
             */ 

            code = *pinput; // 16 bits

            // setup first byte (high byte)
            // ignore first four high order bits
            output[writted_utf8_bytes++] = (char)((code >> (CLIM_CHAR16_BITS_CNT - 4)) | 0xE0);
            
            // setup second byte (middle byte)
            // ignore first 8 bits and 2 first bits of second byte
            output[writted_utf8_bytes++] = (char)(((code >> (CLIM_CHAR16_BITS_CNT - 10)) & 0x3F) | 0x80);

            // setup third byte (low byte)
            output[writted_utf8_bytes++] = (char)((code & 0x3F) | 0x80);
        }
    }

	if (srclen > 0)
		return CLIM_EC_CONVERSION_ERROR;

	output[output_len] = '\0';

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
	char* psave = dest;
    
	if (!dest_size) 
        return (size_t)(dest - psave) + strlen(src);

	for (; dest_size && (*dest = *src) != '\0'; dest_size--, src++, dest++)
        continue;

	*dest = 0;
	return (size_t)(dest - psave) + strlen(src);
}
