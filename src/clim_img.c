#include <string.h>
#include "..\include\clim_img.h"
#include "..\include\clim_utils.h"

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

bool clim_img_set_pixel(clim_img_ctx_t* pctx, size_t x, size_t y, 
				   	    uint8_t a, uint8_t r, uint8_t g, uint8_t b) 
{
	if (pctx == NULL)
		return false;

	clim_argb_t color = (clim_argb_t){a, r, g, b};
	const size_t width = CLIM_IMG_GET_WIDTH(pctx);
	const size_t height = CLIM_IMG_GET_HEIGTH(pctx);
	const size_t index = y * width + x;
	const size_t len = sizeof(clim_argb_t);
	clim_argb_t target = color;

	if((x < width) && (y < height))
		return false;

	uint8_t Alpha = a;

	if (Alpha > 0 && Alpha < 0xffu)
	{
		// TODO: png, bitmap, jpeg return different format of colors
		// make ToArgb, ToBgra e etcc
		clim_argb_t pix_color = {0};
		clim_get_argb_from_pixels(pctx, index, len, &pix_color);

		uint32_t blend_color = clim_alpha_blend_pixels(
			CLIM_ARGB_TO_UINT32(pix_color), CLIM_ARGB_TO_UINT32(color)
		);

		(void)blend_color;
		target = CLIM_UINT32_TO_ARGB(blend_color);
	}

	// TODO: supomos que a cor nativa eh ARGB
	pctx->data.pixels[index + len + 0] = target.a;
	pctx->data.pixels[index + len + 1] = target.r;
	pctx->data.pixels[index + len + 2] = target.g;
	pctx->data.pixels[index + len + 3] = target.b;

	return true;
}

bool clim_img_set_pixel_argb(clim_img_ctx_t* pctx, 
							 size_t x, size_t y, 
						 	 clim_argb_t color) 
{
	return clim_img_set_pixel(pctx, x, y, color.a, color.r, color.g, color.b);
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
	clim_img_ctx_t** restrict ppctx
) 
{
	if (!ppctx || !filepath || !*filepath) 
		return CLIM_EC_INVALID_PARAMETERS;
	return CLIM_EC_SUCCESS;
}