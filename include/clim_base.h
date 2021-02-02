#ifndef CLIM_BASE_H
#define CLIM_BASE_H

#include "clim_platform_detector.h"

#if (__STDC_VERSION__ < 201112L)
	#error This program needs at least a C11 compliant compiler
#endif

#ifdef CLIM_COMPILER_MSVC
	#pragma once
	#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef CLIM_OS_LINUX
	#include <linux/limits.h>
	#define CLIM_MAX_OS_PATH PATH_MAX
#elif defined(CLIM_OS_WIN)
	#include <minwindef.h>
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

#if defined(_DEBUG) || !defined(NDEBUG)
	#define CLIM_DEBUG_MODE
#else
	#define CLIM_RELEASE_MODE
#endif

// CLIM => COMMAND LINE IMAGE MANIPULATOR
#define CLIM_API

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include "clim_mem.h"

typedef enum
{
	CLIM_EC_SUCCESS = 0x000000AF,
	CLIM_EC_INVALID_PARAMETERS,
	CLIM_EC_CONVERSION_ERROR,
	CLIM_EC_CANNOT_CLOSE_FILE,
	CLIM_EC_CANNOT_OPEN_FILE,
	CLIM_EC_UNKNOWN
} clim_errcode_t;

typedef enum
{
	CLIM_IMAGE_FORMAT_BITMAP = 0x000002f,
	CLIM_IMAGE_FORMAT_JPEG,
	CLIM_IMAGE_FORMAT_PNG,
	CLIM_IMAGE_FORMAT_PGM
} clim_img_format_t;

#define CLIM_SUCCESS(errcode) ((errcode) == (CLIM_EC_SUCCESS))
#define CLIM_FAILED(errcode) (!CLIM_SUCCESS(errcode))

#define CLIM_MAKE_POINT_STRUCT(name, type, ...)    \
	typedef struct  						  \
	{										  \
		type __VA_ARGS__;				  	  \
	} clim_##name;							          \

CLIM_MAKE_POINT_STRUCT(point2_zu_t, size_t, x, y)

typedef struct
{
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;
} clim_argb_t, clim_pixel_color;

typedef struct
{
	uint32_t width;
	uint32_t heigth;
	uint8_t* pixels;
	uint8_t  bytes_per_pixel;
} clim_img_data_t;

typedef struct 
{
	FILE* fptr;
	char filepath[CLIM_MAX_OS_PATH];
} clim_img_file_info_t;

typedef struct
{
	clim_img_file_info_t file;
	clim_img_format_t format;
	clim_img_data_t data;
} clim_img_ctx_t;

#define CLIM_IMG_MAKE_RGB(r, g, b) ((clim_argb_t){255, r, g, b})
#define CLIM_IMG_MAKE_ARGB(a, r, g, b) ((clim_argb_t){a, r, g, b})

#define CLIM_ARGB_TO_UINT32(argb) 0
#define CLIM_UINT32_TO_ARGB(n) (clim_argb_t){0}

#define CLIM_IMG_SET_SIZE(ctx, w, h) do { ctx->data.width = w, ctx->data.heigth = h; } while(0)
#define CLIM_IMG_SET_BPP(ctx, n) do { ctx->data.bytes_per_pixel = n; } while(0)
#define CLIM_IMG_GET_HEIGTH(ctx) (ctx->data.heigth)
#define CLIM_IMG_GET_WIDTH(ctx) (ctx->data.width)

#define CLIM_ASSERT(cond) assert(cond)
#define CLIM_STATIC_ASSERT(cond, msg) static_assert((cond), msg)

#endif