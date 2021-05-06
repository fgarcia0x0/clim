#ifndef CLIM_IMG_BITMAP_H
#define CLIM_IMG_BITMAP_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
    #pragma once
#endif

#include <stdlib.h>
#include <math.h>

#include "clim_base.h"
#include "clim_utils.h"

#if defined(CLIM_COMPILER_GCC) || defined(CLIM_COMPILER_CLANG) 
typedef struct 
{
    uint16_t signature;
    uint32_t filesize;
    uint32_t reserved;
    uint32_t pixel_data_offset;
} __attribute__((packed)) clim_bitmap_file_header_t;

typedef struct
{
    uint32_t size; 
    int32_t width; 
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t img_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} __attribute__((packed)) clim_bitmap_info_header_t;

#elif CLIM_COMPILER_MSVC

#pragma pack(push, 1)
typedef struct
{
    uint16_t signature;
    uint32_t filesize;
    uint32_t reserved;
    uint32_t pixel_data_offset;
} clim_bitmap_file_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    uint32_t size;
    int32_t width; 
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t img_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} clim_bitmap_info_header_t;
#pragma pack(pop)

#endif

#define CLIM_BMP_ABS(x) (((x) < 0) ? (-x) : (x))

clim_errcode_t clim_img_bmp_load(const uint8_t* buffer, 
                                 const size_t buffer_len, 
                                 clim_img_ctx_t* pctx)
{
    if (!buffer)
		return CLIM_EC_INVALID_PARAMETERS;

    // TODO(Garcia): Type Punning UB
	if (*(const uint16_t *)buffer != 0x4D42)
		return CLIM_EC_INVALID_BITMAP_FILE;

    pctx->format = CLIM_IMAGE_FORMAT_BMP;

    const uint8_t* save_point = buffer;
    buffer += offsetof(clim_bitmap_file_header_t, pixel_data_offset);
    
	uint8_t pixel_offset = *buffer;
    clim_bitmap_info_header_t info_header = { 0 };
	
    memcpy(&info_header, 
           save_point + sizeof(clim_bitmap_file_header_t),
       	   sizeof(clim_bitmap_info_header_t));

    pctx->data.width           = (uint32_t)(CLIM_BMP_ABS(info_header.width));
    pctx->data.height          = (uint32_t)(CLIM_BMP_ABS(info_header.height));
    pctx->data.bytes_per_pixel = (uint8_t) (info_header.bits_per_pixel >> 3U);

	const uint8_t bpp = pctx->data.bytes_per_pixel;
    if (bpp != 3 && bpp != 4)
		return CLIM_EC_UNSUPPORTED_BITMAP_BPP;

    buffer = save_point + pixel_offset;
    const size_t  rowsize = pctx->data.width * pctx->data.bytes_per_pixel;
    const uint8_t padd    = ((4U - (rowsize & 3U)) & 3U);

    const uint8_t mem_bpp  = sizeof(clim_pixelcolor_t);
    const size_t  mem_len  = pctx->data.height * pctx->data.width * mem_bpp;

    uint8_t* img_pixels  = (uint8_t *) clim_mem_alloc(mem_len, false);
    const bool is_32_bits_per_pixel = (bpp == 4U);

    uint32_t width  = pctx->data.width;
    uint32_t height = pctx->data.height;

    // #pragma omp parallel for
    for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
            img_pixels[mem_bpp * (y * width + x) + 3U] = *buffer++ & UINT8_MAX;
            img_pixels[mem_bpp * (y * width + x) + 2U] = *buffer++ & UINT8_MAX;
            img_pixels[mem_bpp * (y * width + x) + 1U] = *buffer++ & UINT8_MAX;
            img_pixels[mem_bpp * (y * width + x) + 0U] = is_32_bits_per_pixel ? (*buffer++ & UINT8_MAX) : (0xffu);
		}

		if (!is_32_bits_per_pixel)
			buffer += padd;
	}

    pctx->data.pixels = img_pixels;
    return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_bmp_write(const char* filepath, const clim_img_ctx_t* pctx)
{
	CLIM_ASSERT(pctx && filepath);
    
    if (pctx->data.bytes_per_pixel != 3 && pctx->data.bytes_per_pixel != 4)
        return CLIM_EC_UNSUPPORTED_BITMAP_BPP;

    const uint32_t rowsize = pctx->data.width * pctx->data.bytes_per_pixel;
    const uint8_t  padd    = ((4 - (rowsize & 3)) & 3);
    const size_t   len     = pctx->data.height * (rowsize + padd);

    clim_bitmap_file_header_t ctx_header_file = 
    {
        .signature = 0x4D42,
        .filesize  = (uint32_t)(len + sizeof(clim_bitmap_file_header_t) + sizeof(clim_bitmap_info_header_t)), 
        .pixel_data_offset = (uint32_t)(sizeof(clim_bitmap_file_header_t) + sizeof(clim_bitmap_info_header_t))
    };

    clim_bitmap_info_header_t ctx_header_info =
    {
        .size           = sizeof(clim_bitmap_info_header_t),
        .width          = (int32_t)(pctx->data.width),
        .height         = (int32_t)(pctx->data.height),
		.bits_per_pixel = (uint16_t)(pctx->data.bytes_per_pixel << 3),
		.img_size 		= (uint32_t)(len),
        .planes         = 1U
    };

    FILE* file_image = fopen(filepath, "wb");
    if (!file_image)
        return CLIM_EC_CANNOT_WRITE_FILE;
    
    fwrite(&ctx_header_file, sizeof(ctx_header_file), 1, file_image);
    fwrite(&ctx_header_info, sizeof(ctx_header_info), 1, file_image);
    
    const uint8_t  bpp     = pctx->data.bytes_per_pixel;
    const uint8_t  mem_bpp = sizeof(clim_pixelcolor_t);
    uint8_t*       pixels  = pctx->data.pixels;
    const uint32_t height  = pctx->data.height;
    const uint32_t width   = pctx->data.width;

    if (bpp == 4U)
    {
        clim_bgr2rgb(pixels, mem_bpp,
                     pixels + (pctx->data.width * pctx->data.height * mem_bpp));
        
        // TODO(Garcia): Ensure return write size
        fwrite(pctx->data.pixels, len, sizeof(uint8_t), file_image);
    }
    else
    {
        // #pragma omp parallel for
        for (uint32_t y = 0U; y < height; ++y)
        {
            for (uint32_t x = 0U; x < width; ++x)
            {
                fputc(pixels[mem_bpp * (y * width + x) + 3U], file_image); // B
                fputc(pixels[mem_bpp * (y * width + x) + 2U], file_image); // G
                fputc(pixels[mem_bpp * (y * width + x) + 1U], file_image); // R
            }
            
            fwrite(&(uint8_t){0x0}, sizeof(uint8_t), padd, file_image);
        }
    }
    
    clim_fclose(file_image);
    return CLIM_EC_SUCCESS;
}

#endif
