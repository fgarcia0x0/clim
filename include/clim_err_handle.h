#ifndef CLIM_ERR_HANDLE
#define CLIM_ERR_HANDLE

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include <stddef.h>

typedef enum
{
	CLIM_EC_SUCCESS = 0x000000AF,
	CLIM_EC_INVALID_PARAMETERS,
	CLIM_EC_CONVERSION_ERROR,
	CLIM_EC_CANNOT_CLOSE_FILE,
	CLIM_EC_CANNOT_OPEN_FILE,
	CLIM_EC_CANNOT_READ_FILE,
	CLIM_EC_CANNOT_WRITE_FILE,
	CLIM_EC_PATH_IS_TOO_LONG,
	CLIM_EC_WAS_ALREADY_INIT,
	CLIM_EC_STAT_FAILED,
	CLIM_EC_FILE_NOT_FOUND,
	CLIM_EC_UNSUPPORTED_IMAGE_FORMAT,
	CLIM_EC_IMAGE_FORMAT_UNKNOWN,
	CLIM_EC_NOT_IMPLEMENTED,
	CLIM_EC_UNSUPPORTED_IMAGE_BPP,
	CLIM_EC_UNSUPPORTED_BITMAP_BPP,
	CLIM_EC_INVALID_BITMAP_FILE,
	CLIM_EC_NUMBER_OVERFLOW,
	CLIM_EC_NUMBER_CONVERSION_ERROR,
	CLIM_EC_INVALID_VALUE_RANGE,
	CLIM_EC_UNKNOWN
} clim_errcode_t;

#define MAX_ERRCODE_CODES (CLIM_EC_UNKNOWN - CLIM_EC_SUCCESS + 1)

typedef struct
{
	const char* msg; 
	clim_errcode_t code;
} clim_err_pair;

static const clim_err_pair s_error_messages[MAX_ERRCODE_CODES] =
{
	{ .msg = "No errors occurred (success)",  .code = CLIM_EC_SUCCESS },
	{ .msg = "Parameters passed are invalid", .code = CLIM_EC_INVALID_PARAMETERS },
	{ .msg = "Error when maping a character of a certain set to another", .code = CLIM_EC_CONVERSION_ERROR},
	{ .msg = "An error occurred while trying to close the file identifier", .code = CLIM_EC_CANNOT_CLOSE_FILE},
	{ .msg = "An error occurred while trying to open the past file (more common reason: invalid file path)", .code = CLIM_EC_CANNOT_OPEN_FILE},
	{ .msg = "An error occurred while trying to read the file data", .code = CLIM_EC_CANNOT_READ_FILE},
	{ .msg = "An error occurred while trying to write the data in the file", .code = CLIM_EC_CANNOT_WRITE_FILE},
	{ .msg = "The path of the past file is very long", .code = CLIM_EC_PATH_IS_TOO_LONG},
	{ .msg = "Attention is not possible to initialize an instance more than once", .code = CLIM_EC_WAS_ALREADY_INIT},
	{ .msg = "Failed to pick up the file information via \"stat\"", .code = CLIM_EC_STAT_FAILED},
	{ .msg = "It was not possible to find the specified file (file not found)", .code = CLIM_EC_FILE_NOT_FOUND},
	{ .msg = "The image format is not supported", .code = CLIM_EC_UNSUPPORTED_IMAGE_FORMAT},
	{ .msg = "The program does not recognize the image format", .code = CLIM_EC_IMAGE_FORMAT_UNKNOWN},
	{ .msg = "The specified image format has not yet been implemented", .code = CLIM_EC_NOT_IMPLEMENTED},
	{ .msg = "The specified BPP(Bits Per Pixel) is not supported", .code = CLIM_EC_UNSUPPORTED_IMAGE_BPP},
	{ .msg = "Bitmap format BPP(Bits Per Pixel) value is not supported", .code = CLIM_EC_UNSUPPORTED_BITMAP_BPP},
	{ .msg = "The not Bitmap type image file", .code = CLIM_EC_INVALID_BITMAP_FILE},
	{ .msg = "It was not possible to extract the number of the given text", .code = CLIM_EC_NUMBER_CONVERSION_ERROR},
	{ .msg = "You can not extract a number greater than the fully allowed type of integer (overflow).", .code = CLIM_EC_NUMBER_OVERFLOW},
	{ .msg = "The specified range is invalid" , .code = CLIM_EC_INVALID_VALUE_RANGE},
	{ .msg = "Unknown Error", .code = CLIM_EC_UNKNOWN }
};

static inline const char* clim_err_get_msg(clim_errcode_t errcode)
{
	return s_error_messages[(errcode - CLIM_EC_SUCCESS) % MAX_ERRCODE_CODES].msg;
}

#endif
