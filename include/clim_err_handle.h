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
	CLIM_EC_PATH_IS_TOO_LONG,
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
};

static inline const char* clim_err_get_msg(clim_errcode_t errcode)
{
	const size_t index = (unsigned)errcode - (unsigned)CLIM_EC_SUCCESS;
	return s_error_messages[index].msg;
}

#endif