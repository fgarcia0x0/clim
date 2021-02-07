#include <string.h>

#include "../include/clim_img.h"
#include "../include/clim_utils.h"
#include "../include/clim_logger.h"

static inline void clim_get_argb_from_pixels(
	const clim_img_ctx_t* pctx, size_t index, 
	size_t len, clim_argb_t* pout_color
)
{
	pout_color->a = pctx->data.pixels[index + len + 0];
	pout_color->r = pctx->data.pixels[index + len + 1];
	pout_color->g = pctx->data.pixels[index + len + 2];
	pout_color->b = pctx->data.pixels[index + len + 3];
}

void clim_img_set_pixel(
	clim_img_ctx_t* pctx, size_t x, size_t y, 
	uint8_t a, uint8_t r, uint8_t g, uint8_t b
) 
{
	CLIM_ASSERT((pctx) && (x < width) && (y < CLIM_IMG_GET_HEIGTH(pctx)));

	clim_argb_t color = (clim_argb_t){a, r, g, b};
	const size_t width = CLIM_IMG_GET_WIDTH(pctx);

	const size_t index = y * width + x;
	const size_t len = sizeof(clim_argb_t);
	clim_argb_t target = color;

	uint8_t Alpha = a;

	if (Alpha > 0 && Alpha < 0xffu)
	{
		clim_argb_t pix_color = {0};
		clim_get_argb_from_pixels(pctx, index, len, &pix_color);

		uint32_t blend_color = clim_alpha_blend_pixels(
			clim_argb_to_uint32(pix_color), clim_argb_to_uint32(color)
		);

		target = clim_uint32_to_argb(blend_color);
	}

	pctx->data.pixels[index + len 	 ] = target.a;
	pctx->data.pixels[index + len + 1] = target.r;
	pctx->data.pixels[index + len + 2] = target.g;
	pctx->data.pixels[index + len + 3] = target.b;
}

void clim_img_set_pixel_argb(
	clim_img_ctx_t* pctx, 
	size_t x, size_t y, 
	clim_argb_t color
)
{
	clim_img_set_pixel(pctx, x, y, color.a, color.r, color.g, color.b);
}

clim_argb_t clim_img_get_pixel_argb(
	const clim_img_ctx_t* restrict pctx, 
	size_t x_pos, size_t y_pos
) 
{
	CLIM_ASSERT(pctx && pctx->data.pixels);

	const size_t width = CLIM_IMG_GET_WIDTH(pctx);
	const size_t index = y_pos * width + x_pos;
	const size_t len = sizeof(clim_argb_t);

	clim_argb_t result = {0};
	CLIM_ASSERT(x_pos < width && y_pos < CLIM_IMG_GET_HEIGTH(pctx));
	
	clim_get_argb_from_pixels(pctx, index, len, &result);
	return result;
}

clim_argb_t clim_img_get_pixel_argb_pntzu(
	const clim_img_ctx_t* restrict pctx, 
	clim_point2_zu_t point
)
{
	return clim_img_get_pixel_argb(pctx, point.x, point.y);
}

clim_errcode_t clim_img_load(
	const char* filepath, 
	clim_img_ctx_t* restrict pctx
) 
{
	if (!pctx || !filepath || !*filepath) 
		return CLIM_EC_INVALID_PARAMETERS;

	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_release(
	clim_img_ctx_t* restrict pctx
)
{
	if (!pctx) 
		return CLIM_EC_INVALID_PARAMETERS;

	clim_mem_release(pctx->data.pixels);

	memset(pctx, 0, sizeof(*pctx));
	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_write(
	const char* filename, const clim_img_ctx_t* restrict pctx,
	clim_img_format_t format, clim_img_quality_t quality
)
{
	if (!filename || !*filename || !pctx)
		return CLIM_EC_INVALID_PARAMETERS;

	const size_t file_size = clim_strnlen(filename, CLIM_MAX_OS_PATH);

	if (file_size >= CLIM_MAX_OS_PATH)
	{
		CLIM_LOG_DEBUG(
			"this path: \"%.*s...\" is too long", 
			CLIM_MAX_OS_PATH, filename
		);
		return CLIM_EC_PATH_IS_TOO_LONG;
	}

	CLIM_UNREF_PARAM(format);
	CLIM_UNREF_PARAM(quality);

	return CLIM_EC_SUCCESS;
}

void clim_img_copy(
    const clim_img_ctx_t* restrict pctx_src, 
    clim_img_ctx_t* restrict pctx_dest,
	clim_img_copy_method method
)
{
	CLIM_ASSERT(pctx_src && pctx_dest && pctx_src != pctx_dest);
	memcpy(pctx_dest, pctx_src, sizeof(*pctx_src));

	const uint8_t* psrc_pixels = pctx_src->data.pixels;
	if ((method == CLIM_IMG_COPY_DEEP) && psrc_pixels)
    {
		const size_t len = (size_t)pctx_src->data.width * pctx_src->data.heigth;
		pctx_dest->data.pixels = clim_mem_dup(
			psrc_pixels, len * pctx_src->data.bytes_per_pixel
		);
	}
}