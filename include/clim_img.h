#ifndef CLIM_IMG_H
#define CLIM_IMG_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include "clim_base.h"

#define CLIM_IMG_SET_PIXEL(ctx, x, y, r, g, b) img_set_pixel_argb(ctx, x, y, IMG_MAKE_RGB(r, g, b))
#define CLIM_IMG_GET_PIXEL(ctx, x, y) img_get_pixel_argb(ctx, x, y)

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
 * @return     if parameters and content is valid return true, otherwise false
 */
bool clim_img_set_pixel(img_ctx_t* pctx, size_t x, size_t y, 
				   		uint8_t a, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief      set pixel color on argb format in image
 *
 * @param      pctx   the image context
 * @param[in]  x      the x coordinate
 * @param[in]  y      the y coordinate
 * @param[in]  color  the new color applied in the pixel position [x, y]
 *
 * @return     if parameters and content is valid return true, otherwise false
 */
bool clim_img_set_pixel_argb(img_ctx_t* pctx, size_t x, size_t y, 
							 argb_t color);

/**
 * @brief      set pixel color on bgra format in image
 *
 * @param      pctx   the image context
 * @param[in]  x      the x coordinate
 * @param[in]  y      the y coordinate
 * @param[in]  color  the new color applied in the pixel position [x, y]
 *
 * @return     if parameters and content is valid return true, otherwise false
 */
bool clim_img_set_pixel_bgra(img_ctx_t* pctx, size_t x, size_t y, 
							 bgra_t color);

/**
 * @brief      get pixel color of image in ARGB format
 *
 * @param[in]  pctx   pointer to image context
 * @param[in]  x_pos  The x position
 * @param[in]  y_pos  The y position
 * @see        img_ctx_t
 *
 * @return     pixel color in the position [x, y]
 */
argb_t clim_img_get_pixel_argb(const img_ctx_t* restrict pctx, 
							   size_t x_pos, size_t y_pos);

/**
 * @brief      get pixel color of image in ARGB format
 *
 * @param[in]  pctx   pointer to image context
 * @param[in]  point  an point 2d in space represented by the point2_zu_t
 *                    structure
 * @see        img_ctx_t
 *
 * @return     pixel color in the position [point.x, point.y]
 */
argb_t clim_img_get_pixel_argb_pntzu(const img_ctx_t* restrict pctx, 
									 point2_zu_t point);

/**
 * @brief      get pixel color of image in BGRA format
 *
 * @param[in]  pctx   pointer to image context
 * @param[in]  x_pos  The x position
 * @param[in]  y_pos  The y position
 *
 * @return     pixel color in the position [point.x, point.y]
 */
bgra_t clim_img_get_pixel_bgra(const img_ctx_t* restrict pctx, 
							   size_t x_pos, size_t y_pos);

/**
 * @brief      get pixel color of image in BGRA format
 *
 * @param[in]  pctx   pointer to image context
 * @param[in]  point  an point 2d in space represented by the point2_zu_t
 *                    structure
 *
 * @return     pixel color in the position [point.x, point.y]
 */
bgra_t clim_img_get_pixel_bgra_pntzu(const img_ctx_t* restrict pctx, 
									 point2_zu_t point);

/**
 * @brief      load an image into image context
 * @see        img_ctx_t
 *
 * @param[in]  filepath  the image path
 * @param      ppctx     pointer to image context
 *
 * @return     EC_INVALID_PARAMATERS or EC_SUCCESS
 */
clim_err_code_t clim_img_load(const char* filepath, img_ctx_t** restrict ppctx);

#endif