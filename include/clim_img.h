#ifndef CLIM_IMG_H
#define CLIM_IMG_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include "clim_base.h"

#define CLIM_IMG_MAKE_RGB(r, g, b) 	   (clim_pixelcolor_t){{{0xff, r, g, b}}}
#define CLIM_IMG_MAKE_ARGB(a, r, g, b) (clim_pixelcolor_t){{{a, r, g, b}}}
#define CLIM_IMG_MAKE_RGBA(r, g, b, a) (clim_pixelcolor_t){{{a, r, g, b}}}

#define CLIM_IMG_SET_SIZE(ctx, w, h) do { (ctx)->data.width = w, (ctx)->data.height = h; } while(0)
#define CLIM_IMG_SET_BPP(ctx, n) do { (ctx)->data.bytes_per_pixel = n; } while(0)
#define CLIM_IMG_GET_HEIGHT(ctx) ((ctx)->data.height)
#define CLIM_IMG_GET_WIDTH(ctx) ((ctx)->data.width)

#define CLIM_IMG_SET_PIXEL(ctx, x, y, r, g, b) clim_img_set_pixel_argb(ctx, x, y, CLIM_IMG_MAKE_RGB(r, g, b))
#define CLIM_IMG_GET_PIXEL(ctx, x, y) clim_img_get_pixel_argb(ctx, x, y)
#define CLIM_IMG_GET_FORMAT(ctx) ((ctx).format)

typedef enum
{
	CLIM_FLIP_ORIENTATION_HOR, // horizontal orientation
	CLIM_FLIP_ORIENTATION_VER, // vertical orientation
} clim_img_flip_orientation_t;

static inline clim_pixelcolor_t clim_uint32_to_argb(uint32_t color)
{
	clim_pixelcolor_t result = {0};
	result.a = (uint8_t)((color >> 24U) & UINT8_MAX);
	result.r = (uint8_t)((color >> 16U) & UINT8_MAX);
	result.g = (uint8_t)((color >> 8U)  & UINT8_MAX);
	result.b = (uint8_t)(color & UINT8_MAX);
	return result;
}

static inline uint32_t clim_pixelcolor_to_uint32(clim_pixelcolor_t color)
{
	uint32_t result = {0};
	result |= (uint32_t)((color.a & UINT8_MAX) << 24U);
	result |= (uint32_t)((color.r & UINT8_MAX) << 16U);
	result |= (uint32_t)((color.g & UINT8_MAX) << 8U);
	result |= (uint32_t)((color.b & UINT8_MAX));
	return result;
}

/**
 * @brief      set pixel color of the image in the specified position
 *
 * @param      pctx   the image context.
 * @param[in]  xpos   the x coordinate.
 * @param[in]  ypos   the y coordinate.
 * @param[in]  color  the new color applied in the pixel position [x, y].
 *
 */
void clim_img_set_pixel(
	clim_img_ctx_t* pctx, 
	size_t xpos, size_t ypos, 
	clim_pixelcolor_t color
);

/**
 * @brief      get pixel color of image in the specified position
 *
 * @param[in]  pctx   pointer to image context.
 * @param[in]  xpos   The x position.
 * @param[in]  ypos   The y position.
 * @see        clim_img_ctx_t
 *
 * @return     pixel color in the position [x, y].
 */
clim_pixelcolor_t clim_img_get_pixel(
	const clim_img_ctx_t* restrict pctx, 
	size_t xpos, size_t ypos
);

/**
 * @brief      load image file into image context
 * @see        clim_img_ctx_t
 *
 * @param[in]  filepath  the image path
 * @param      pctx      pointer to image context
 *
 * @return     EC_INVALID_PARAMATERS or EC_SUCCESS
 */
clim_errcode_t clim_img_load(
	const char* filepath, 
	clim_img_ctx_t* restrict pctx
);

/**
 * @brief release image from image context
 * 
 * @param pctx pointer to image context
 * @return EC_INVALID_PARAMATERS or EC_SUCCESS or CLIM_EC_CANNOT_CLOSE_FILE
 */
clim_errcode_t clim_img_release(
	clim_img_ctx_t* restrict pctx
);

/**
 * @brief creates a new image on the disk with the image context information
 * 
 * @param filepath the image file path
 * @param pctx pointer to image context
 * @param format the image format
 * @param quality the image quality
 * @return CLIM_EC_INVALID_PARAMETERS 
 */
clim_errcode_t clim_img_write(
	const char* filepath, const clim_img_ctx_t* restrict pctx,
	clim_img_format_t format, clim_img_quality_t quality
);

/**
 * @brief copy src image to dest image
 * 
 * @param pctx_src pointer to source image context
 * @param pctx_dest pointer to destination image context
 * @param method the copy method
 */
void clim_img_copy(
    const clim_img_ctx_t* restrict pctx_src, 
    clim_img_ctx_t* restrict pctx_dest,
	clim_img_copy_method method
);

/**
 * @brief Creates a new image in memory with the specified dimensions and format
 * 
 * @param img_width the image width
 * @param img_height the image height
 * @param bits_per_pixel The amount of bits per pixel in the image (24 or 32)
 * @param img_format The image format
 * @param pctx out pointer to image context
 * @return clim_errcode_t 
 */
clim_errcode_t clim_img_make(
	const uint32_t img_width,
	const uint32_t img_height,
	const uint8_t  bits_per_pixel,
	clim_img_format_t img_format,
	clim_img_ctx_t* pctx
);

/**
 * @brief Fills the entire image with the specified pixel color
 * 
 * @param pctx pointer to image context
 * @param color the pixel color
 * @return if pctx is NULL, return's CLIM_EC_INVALID_PARAMETERS, 
 * 		   otherwise return's CLIM_EC_SUCCESS
 */
clim_errcode_t clim_img_fillcolor(clim_img_ctx_t* pctx, clim_pixelcolor_t color);

/**
 * @brief Defines the opacity of all pixels of the image
 * 
 * @param pctx pointer to image context
 * @param opacity The value of the opacity [0, 255]
 * @return  if pctx is NULL, return's CLIM_EC_INVALID_PARAMETERS, 
 * 		    otherwise return's CLIM_EC_SUCCESS
 */
clim_errcode_t clim_img_set_opacity(clim_img_ctx_t* pctx, uint8_t opacity);

/**
 * @brief Transforms the image in grayscale
 * 
 * @param pctx pointer to image context
 * @return if pctx is NULL, return's CLIM_EC_INVALID_PARAMETERS, 
 * 		   otherwise return's CLIM_EC_SUCCESS
 */
clim_errcode_t clim_img_to_grayscale(clim_img_ctx_t* pctx);

/**
 * @brief Apply negative transformation in the image
 * 
 * @param pctx pointer to image context
 * @return if pctx is NULL, return's CLIM_EC_INVALID_PARAMETERS, 
 * 		   otherwise return's CLIM_EC_SUCCESS
 */
clim_errcode_t clim_img_to_negative(clim_img_ctx_t* pctx);

/**
 * @brief Reverses the image vertically or horizontally
 * 
 * @param pctx pointer to image context
 * @param orientation The orientation of the image
 * @return if pctx is NULL, return's CLIM_EC_INVALID_PARAMETERS, 
 * 		   otherwise return's CLIM_EC_SUCCESS
 */
clim_errcode_t clim_img_flip(clim_img_ctx_t* pctx, clim_img_flip_orientation_t orientation);

/**
 * @brief Applies the gaussian blur in the image using the "Stack Blur" algorithm
 * 
 * @param pctx pointer to image context
 * @param radius the blur radius [2, 254]
 * @return CLIM_EC_INVALID_PARAMETERS OR CLIM_EC_INVALID_VALUE_RANGE OR CLIM_EC_SUCCESS
 */
clim_errcode_t clim_img_blur(clim_img_ctx_t* pctx, uint8_t radius);

/**
 * @brief Applies the sepia effect in the image
 * 
 * @param pctx pointer to image context
 * @return if pctx is NULL, return's CLIM_EC_INVALID_PARAMETERS, 
 * 		   otherwise return's CLIM_EC_SUCCESS
 */
clim_errcode_t clim_img_to_sepia(clim_img_ctx_t* pctx);

#endif
