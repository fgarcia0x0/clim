#ifndef CLIM_IMG_UTILS_H
#define CLIM_IMG_UTILS_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include <uchar.h>
#include <string.h>
#include <limits.h>
#include "clim_base.h"

uint32_t clim_alpha_blend_pixels(
	const uint32_t p1, const uint32_t p2
);

#ifdef CLIM_OS_WIN
	clim_err_code_t clim_convert_wchar_to_utf8(
		const wchar_t* restrict input, 
		char** restrict ppdest, 
		size_t* restrict pout_bytes_written
	);
#endif

clim_err_code_t clim_utf16_to_utf8(
	const char16_t* restrict input, 
	char* restrict output, 
	const size_t output_len,
	size_t* restrict pout_bytes_written
);

#endif