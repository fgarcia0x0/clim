#ifndef CLIM_BASE_H
#define CLIM_BASE_H

#include "clim_platform_detector.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#ifndef CLIM_COMPILER_MSVC
	#include <stdnoreturn.h>
#endif

#include "clim_mem.h"
#include "clim_err_handle.h"

#ifdef CLIM_OS_WIN
	#define CLIM_WIN_UTF8_CODEPAGE 65001
    extern int __stdcall SetConsoleOutputCP(unsigned wCodePageID);
#endif

#define CLIM_DO_IF(cond, ...) \
	do{ (cond) ? (__VA_ARGS__) : (void)0; } while (0)

typedef unsigned char char8_t;
#define U8(str) (const char8_t *)(str)

typedef enum
{
	CLIM_IMAGE_FORMAT_UNKNOWN = 0x0,
	CLIM_IMAGE_FORMAT_BITMAP = 0x000002f,
	CLIM_IMAGE_FORMAT_JPEG,
	CLIM_IMAGE_FORMAT_PNG,
	CLIM_IMAGE_FORMAT_PGM
} clim_img_format_t;

typedef enum
{
	CLIM_IMAGE_QUALITY_MIN = 5,
	CLIM_IMAGE_QUALITY_LOW = 25,
	CLIM_IMAGE_QUALITY_MEDIUM = 55,
	CLIM_IMAGE_QUALITY_HIGH = 75,
	CLIM_IMAGE_QUALITY_MAX = 100
} clim_img_quality_t;

typedef enum
{
	CLIM_IMG_COPY_DEEP = 0x000000AA,
	CLIM_IMG_COPY_SHALLOW
} clim_img_copy_method;

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
	/*FILE* fptr;*/
	char filepath[CLIM_MAX_OS_PATH];
} clim_img_file_info_t;

typedef struct
{
	clim_img_file_info_t fileinfo;
	clim_img_format_t format;
	clim_img_data_t data;
} clim_img_ctx_t;

#define CLIM_IMG_MAKE_RGB(r, g, b) ((clim_argb_t){255, r, g, b})
#define CLIM_IMG_MAKE_ARGB(a, r, g, b) ((clim_argb_t){a, r, g, b})

#define CLIM_IMG_SET_SIZE(ctx, w, h) do { (ctx)->data.width = w, (ctx)->data.heigth = h; } while(0)
#define CLIM_IMG_SET_BPP(ctx, n) do { (ctx)->data.bytes_per_pixel = n; } while(0)
#define CLIM_IMG_GET_HEIGTH(ctx) ((ctx)->data.heigth)
#define CLIM_IMG_GET_WIDTH(ctx) ((ctx)->data.width)

#define CLIM_ASSERT(cond) assert(cond)
#define CLIM_STATIC_ASSERT(pred) static_assert((pred), "Compile time assert constraint is not true: " #pred)

#define CLIM_UNREF_PARAM(param) \
	(void)(param)

#endif
