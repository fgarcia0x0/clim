#ifndef CLIM_BASE_H
#define CLIM_BASE_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#ifdef CLIM_OS_LINUX
	#include <linux/limits.h>
	#define CLIM_MAX_OS_PATH PATH_MAX
#elif defined(CLIM_OS_WIN)
	#include <Windows.h>
	#define CLIM_MAX_OS_PATH MAX_PATH
#elif defined(CLIM_OS_MAC)
	#include <sys/syslimits.h>
	#define CLIM_MAX_OS_PATH PATH_MAX
#else
	#error "platform not supported"
#endif

#ifdef CLIM_COMPILER_MSVC
	#ifndef __func__
		#define __func__ __FUNCTION__
	#endif
	#define restrict __restrict
#endif

#if defined(CLIM_GNU_EXT)
	#define CLIM_HAVE_MEMMEM
#endif

// CLIM => COMMAND LINE IMAGE MANIPULATOR
#define CLIM_API

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "clim_mem.h"

#define CLIM_MAX_ERR_MSG_LEN 512
#define CLIM_INIT_ERR_H(msg_s, code_n) 		\
	(clim_err_handle_t){ 					\
		.msg = msg_s, .code = code_n, 		\
		.file = __FILE__,					\
		.func = __func__, .line = __LINE__	\
	}

typedef enum
{
	CLIM_EC_SUCCESS = 0x00000000,
	CLIM_EC_INVALID_PARAMETERS,
	CLIM_EC_CONVERSION_ERROR,
	CLIM_EC_CANNOT_CLOSE_FILE,
	CLIM_EC_CANNOT_OPEN_FILE,
	CLIM_EC_UNKNOWN
} clim_err_code_t;

typedef struct
{
	char msg[CLIM_MAX_ERR_MSG_LEN];
	clim_err_code_t code;
	const char* file;
	const char* func;
	unsigned line;
} clim_err_handle_t;

typedef enum
{
	CLIM_IMAGE_FORMAT_BITMAP = 0x000002f,
	CLIM_IMAGE_FORMAT_JPEG,
	CLIM_IMAGE_FORMAT_PNG,
	CLIM_IMAGE_FORMAT_PGM
} img_format_t;

#define CLIM_SUCCESS(errcode) ((errcode) == (CLIM_EC_SUCCESS))
#define CLIM_MAKE_POINT_STRUCT(name, type, ...)    \
	typedef struct  						  \
	{										  \
		type __VA_ARGS__;				  	  \
	} name;							          \

CLIM_MAKE_POINT_STRUCT(point2_zu_t, size_t, x, y)

typedef struct
{
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;
} argb_t;

typedef struct
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} bgra_t;

typedef struct
{
	uint32_t width;
	uint32_t heigth;
	uint8_t* pixels;
	uint8_t  bytes_per_pixel;
} img_data_t;

typedef struct 
{
	FILE* fptr;
	char filepath[CLIM_MAX_OS_PATH];
} img_file_info_t;

typedef struct
{
	img_file_info_t file;
	img_format_t format;
	img_data_t data;
} img_ctx_t;

#define CLIM_IMG_MAKE_RGB(r, g, b) ((argb_t){255, r, g, b})
#define CLIM_IMG_MAKE_ARGB(a, r, g, b) ((argb_t){a, r, g, b})
#define CLIM_IMG_MAKE_BGR(b, g, r) ((bgra_t){b, g, r, 255})
#define CLIM_IMG_MAKE_BGRA(b, g, r, a) ((bgra_t){b, g, r, a})

#define CLIM_ARGB_TO_UINT32(argb) 0
#define CLIM_UINT32_TO_ARGB(n) (argb_t){0}

#define CLIM_IMG_SET_SIZE(ctx, w, h) do { ctx->data.width = w, ctx->data.heigth = h; } while(0)
#define CLIM_IMG_SET_BPP(ctx, n) do { ctx->data.bytes_per_pixel = n; } while(0)
#define CLIM_IMG_GET_HEIGTH(ctx) (ctx->data.heigth)
#define CLIM_IMG_GET_WIDTH(ctx) (ctx->data.width)

#define CLIM_ENSURE(cond) assert(cond)

#endif