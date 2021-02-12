#ifndef CLIM_IMG_H
#define CLIM_IMG_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include "clim_base.h"

#define CLIM_IMG_SET_PIXEL(ctx, x, y, r, g, b) clim_img_set_pixel_argb(ctx, x, y, CLIM_IMG_MAKE_RGB(r, g, b))
#define CLIM_IMG_GET_PIXEL(ctx, x, y) clim_img_get_pixel_argb(ctx, x, y)

static inline clim_argb_t clim_uint32_to_argb(uint32_t color)
{
	clim_argb_t result = {0};
	result.a = (uint8_t)((color >> 24U) & UINT8_MAX);
	result.r = (uint8_t)((color >> 16U) & UINT8_MAX);
	result.g = (uint8_t)((color >> 8U)  & UINT8_MAX);
	result.b = (uint8_t)(color & UINT8_MAX);
	return result;
}

static inline uint32_t clim_argb_to_uint32(clim_argb_t color)
{
	uint32_t result = {0};
	result |= (uint32_t)((color.a & UINT8_MAX) << 24U);
	result |= (uint32_t)((color.r & UINT8_MAX) << 16U);
	result |= (uint32_t)((color.g & UINT8_MAX) << 8U);
	result |= (uint32_t)((color.b & UINT8_MAX));
	return result;
}

/**
 * @brief      set pixel color in image
 *
 * @param      pctx  the image context
 * @param[in]  x     the x coordinate
 * @param[in]  y     the y coordinate
 * @param[in]  a     alpha amount [0, 255]
 * @param[in]  r     red amount [0, 255]
 * @param[in]  g     green amount [0, 255]
 * @param[in]  b     blur amount [0, 255]
 *
 */
void clim_img_set_pixel(
	clim_img_ctx_t* pctx, size_t x, size_t y, 
	uint8_t a, uint8_t r, uint8_t g, uint8_t b
);

/**
 * @brief      set pixel color on argb format in image
 *
 * @param      pctx   the image context
 * @param[in]  x      the x coordinate
 * @param[in]  y      the y coordinate
 * @param[in]  color  the new color applied in the pixel position [x, y]
 *
 */
void clim_img_set_pixel_argb(
	clim_img_ctx_t* pctx, 
	size_t x, size_t y, 
	clim_argb_t color
);

/**
 * @brief      get pixel color of image in ARGB format
 *
 * @param[in]  pctx   pointer to image context
 * @param[in]  x_pos  The x position
 * @param[in]  y_pos  The y position
 * @see        clim_img_ctx_t
 *
 * @return     pixel color in the position [x, y]
 */
clim_argb_t clim_img_get_pixel_argb(
	const clim_img_ctx_t* restrict pctx, 
	size_t x_pos, size_t y_pos
);

/**
 * @brief      get pixel color of image in ARGB format
 *
 * @param[in]  pctx   pointer to image context
 * @param[in]  point  an point 2d in space represented by the point2_zu_t
 *                    structure
 * @see        clim_img_ctx_t
 *
 * @return     pixel color in the position [point.x, point.y]
 */
clim_argb_t clim_img_get_pixel_argb_pntzu(
	const clim_img_ctx_t* restrict pctx, 
	clim_point2_zu_t point
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

#endif
