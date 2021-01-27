#include "..\include\clim_img_utils.h"
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

#ifdef CLIM_OS_WIN
clim_err_code_t clim_convert_wchar_to_utf8(
	const wchar_t* restrict input, 
	char** restrict ppdest, 
	size_t* pout_bytes_written
) 
{
	if (!input || !ppdest)
		return CLIM_EC_INVALID_PARAMETERS;
	
	const size_t input_len = wcslen(input);
	int size_needed = WideCharToMultiByte(
		CP_UTF8, 0, input, (int) input_len, NULL, 0, NULL, NULL
	);

	if (size_needed < 1)
	{
		//CLIM_LOG_ERROR("Error to get wide string needed size from conversion");
		*ppdest = NULL;
		if (pout_bytes_written) 
			*pout_bytes_written = 0;
		return CLIM_EC_UNKNOWN;
	}

	char* utf8_str = clim_alloc_mem((size_t)size_needed, false);

	int nbytes = WideCharToMultiByte(
		CP_UTF8, 0, input, (int) input_len, 
		utf8_str, size_needed, NULL, NULL
	);

	if (nbytes != size_needed)
	{
		clim_release_mem(utf8_str);
		*ppdest = NULL;

		if (pout_bytes_written) 
			*pout_bytes_written = (size_t)nbytes;

		//CLIM_LOG_ERROR("[clim_img_utils.c]: error in function \"clim_convert_wchar_to_utf8\", at line: 12 => Error during conversion");
		// WinGetErrMsg(GetLastError())

		return CLIM_EC_UNKNOWN;
	}

	*ppdest = utf8_str;
	if (pout_bytes_written) 
		*pout_bytes_written = (size_t)nbytes;

	return CLIM_EC_SUCCESS;
}
#endif

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