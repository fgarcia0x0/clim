#ifndef CLIM_BASE_H
#define CLIM_BASE_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#include "clim_platform_detector.h"

#ifndef CLIM_COMPILER_MSVC
	#include <stdnoreturn.h>
#endif

#define CLIM_ASSERT(cond) assert(cond)
#define CLIM_STATIC_ASSERT(pred) static_assert((pred), "Compile time assert constraint is not true: " #pred)

#define CLIM_UNREF_PARAM(param) \
	(void)(param)

#define CLIM_RET_IF(cond, ...) (cond) ? return __VA_ARGS__ : (void)(0)

#define CLIM_DO_IF(cond, ...) \
	do{ (cond) ? (__VA_ARGS__) : (void)0; } while (0)

#include "clim_mem.h"
#include "clim_err_handle.h"

#define CLIM_PRINT_ERRC(__errc) fprintf(stderr, "%s\n", clim_err_get_msg(__errc))
#define CLIM_GET_ERR_FMT(errc) "%s", clim_err_get_msg(errc)
#define CLIM_STRINGFY(x) #x

typedef unsigned char char8_t;
#define U8(str) (const char8_t *)(str)

typedef enum
{
	CLIM_IMAGE_FORMAT_UNKNOWN = 0x00000000,
	CLIM_IMAGE_FORMAT_BMP,
	CLIM_IMAGE_FORMAT_PGM,
	CLIM_IMAGE_FORMAT_PPM,
	CLIM_IMAGE_FORMAT_PBM,
	CLIM_IMAGE_FORMAT_JPG,
	CLIM_IMAGE_FORMAT_PNG,
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
	union
	{
		struct 
		{
			uint8_t a;
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};
		uint32_t native;
	};

} clim_pixelcolor_t;

typedef struct
{
	uint32_t width;
	uint32_t height;
	uint8_t* pixels;
	uint8_t  bytes_per_pixel;
	void* reserved;
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

#endif
