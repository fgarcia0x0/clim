#include <string.h>

#include "../include/clim_img.h"
#include "../include/clim_utils.h"
#include "../include/clim_logger.h"
#include "../include/clim_filesystem.h"
#include "../include/clim_img_bitmap.h"
#include "../include/clim_img_blur.h"

#define DIV_255_FAST(x) (((x) + (((x) + 257U) >> 8U)) >> 8U)
#define U32(expr) (uint32_t)(expr)

void clim_img_set_pixel(
	clim_img_ctx_t* pctx, 
	size_t xpos, size_t ypos, 
	clim_pixelcolor_t color
)
{
	CLIM_ASSERT(pctx && pctx->data.pixels);
	CLIM_ASSERT(xpos < CLIM_IMG_GET_WIDTH(pctx));
	CLIM_ASSERT(ypos < CLIM_IMG_GET_HEIGHT(pctx));

	const size_t width = CLIM_IMG_GET_WIDTH(pctx);
	const size_t index = ypos * width + xpos;
	const size_t len = sizeof(clim_pixelcolor_t);

	clim_pixelcolor_t target = color;
	uint8_t alpha = target.a;

	if (alpha > 0 && alpha < 0xffu)
	{
		clim_pixelcolor_t pix_color = {0};
		memcpy(&pix_color , &pctx->data.pixels[index * len], sizeof(pix_color));

		uint32_t blend_color = clim_alpha_blend_pixels(
			clim_pixelcolor_to_uint32(pix_color), 
			clim_pixelcolor_to_uint32(color)
		);

		target = clim_uint32_to_argb(blend_color);
	}

	memcpy(&pctx->data.pixels[index * len], &target, sizeof(target));
}

clim_pixelcolor_t clim_img_get_pixel(
	const clim_img_ctx_t* restrict pctx,
	size_t xpos, size_t ypos
) 
{
	CLIM_ASSERT(pctx && pctx->data.pixels);

	const size_t width = CLIM_IMG_GET_WIDTH(pctx);
	CLIM_ASSERT(xpos < width && ypos < CLIM_IMG_GET_HEIGHT(pctx));

	const size_t index = xpos * width + ypos;
	const size_t len = sizeof(clim_pixelcolor_t);

	clim_pixelcolor_t result = {0};
	memcpy(&result, &pctx->data.pixels[index * len], sizeof(result));

	return result;
}

clim_errcode_t clim_img_load(
	const char* filepath, 
	clim_img_ctx_t* restrict pctx
) 
{
	if (!pctx || !filepath || !*filepath) 
		return CLIM_EC_INVALID_PARAMETERS;

	if (!clim_fs_file_exists(filepath))
		return CLIM_EC_FILE_NOT_FOUND;

	clim_errcode_t errc = CLIM_EC_SUCCESS;
	clim_buffer_t file_buffer = { 0 };

	errc = clim_fs_read_entire_file(filepath, &file_buffer);

	if (CLIM_FAILED(errc))
		return errc;

	clim_img_format_t format = clim_get_img_format(file_buffer.data, file_buffer.size);
	if (format == CLIM_IMAGE_FORMAT_UNKNOWN)
	{
		clim_mem_release(file_buffer.data);
		return CLIM_EC_IMAGE_FORMAT_UNKNOWN;
	}

	switch (format)
	{
		case CLIM_IMAGE_FORMAT_BMP:
			errc = clim_img_bmp_load(file_buffer.data, file_buffer.size, pctx);
			break;
		case CLIM_IMAGE_FORMAT_PGM:
		case CLIM_IMAGE_FORMAT_PNG:
		case CLIM_IMAGE_FORMAT_JPG:
			errc = CLIM_EC_NOT_IMPLEMENTED;
			break;
		default:
			errc = CLIM_EC_UNSUPPORTED_IMAGE_FORMAT;
			break;
	}

	clim_mem_release(file_buffer.data);
	return errc;
}

clim_errcode_t clim_img_release(clim_img_ctx_t* restrict pctx)
{
	if (!pctx) 
		return CLIM_EC_INVALID_PARAMETERS;

	clim_mem_release(pctx->data.pixels);

	(void) memset(pctx, 0, sizeof(*pctx));
	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_write(
	const char* filename, const clim_img_ctx_t* restrict pctx,
	clim_img_format_t format, clim_img_quality_t quality
)
{
	CLIM_UNREF_PARAM(quality);

	if (!filename || !*filename || !pctx)
		return CLIM_EC_INVALID_PARAMETERS;

	const size_t filepath_len = clim_strnlen(filename, CLIM_MAX_OS_PATH);
	clim_errcode_t errc = CLIM_EC_SUCCESS;

	if (filepath_len >= CLIM_MAX_OS_PATH)
	{
		CLIM_LOG_DEBUG(
			"this path: \"%.*s...\" is too long", 
			CLIM_MAX_OS_PATH, filename
		);
		return CLIM_EC_PATH_IS_TOO_LONG;
	}

	switch (format)
	{
		case CLIM_IMAGE_FORMAT_BMP:
			errc = clim_img_bmp_write(filename, pctx);
			break;
		case CLIM_IMAGE_FORMAT_PGM:
		case CLIM_IMAGE_FORMAT_PNG:
		case CLIM_IMAGE_FORMAT_JPG:
			errc = CLIM_EC_NOT_IMPLEMENTED;
			break;
		default:
			errc = CLIM_EC_UNSUPPORTED_IMAGE_BPP;
			break;
	}

	return errc;
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
		const size_t len = (size_t)pctx_src->data.width * pctx_src->data.height;
		pctx_dest->data.pixels = clim_mem_dup(
			psrc_pixels, len * pctx_src->data.bytes_per_pixel
		);
	}
}

clim_errcode_t clim_img_make(
	const uint32_t img_width,
	const uint32_t img_height,
	const uint8_t  bits_per_pixel,
	clim_img_format_t img_format,
	clim_img_ctx_t* pctx
)
{
	if (!img_width || !img_height || !bits_per_pixel)
		return CLIM_EC_INVALID_PARAMETERS;

	if (bits_per_pixel != 24 && bits_per_pixel != 32)
		return CLIM_EC_UNSUPPORTED_IMAGE_BPP;

	const size_t len = (size_t)(img_width) * img_height;
	const uint8_t bytes_per_pixel = (bits_per_pixel >> 3U);

	if (pctx)
	{
		*pctx = (clim_img_ctx_t)
		{
			.data.width  = img_width,
			.data.height = img_height,
			.data.bytes_per_pixel = bytes_per_pixel,
			.data.pixels = clim_mem_alloc(len * bytes_per_pixel, true),
			.format      = img_format,
		};
	}

	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_fillcolor(clim_img_ctx_t* pctx, clim_pixelcolor_t color)
{
	if (!pctx)
		return CLIM_EC_INVALID_PARAMETERS;

	const uint32_t width  = pctx->data.width;
	const uint32_t height = pctx->data.height;

	// #pragma omp parallel for
	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			clim_img_set_pixel(pctx, x, y, color);
		}
	}

	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_set_opacity(clim_img_ctx_t* pctx, uint8_t opacity)
{
	if (!pctx)
		return CLIM_EC_INVALID_PARAMETERS;
	
	if(opacity == UINT8_MAX)
		return CLIM_EC_SUCCESS;

	uint8_t  alpha  = opacity;
	uint32_t width  = pctx->data.width;
	uint32_t height = pctx->data.height;
	
	uint8_t* pixels_beg = pctx->data.pixels;
	uint8_t  bpp        = sizeof(clim_pixelcolor_t);
	uint8_t* pixels_end = pctx->data.pixels + (width * height * bpp);
	
	uint32_t rr = { 0 };
	uint32_t gg = { 0 };
	uint32_t bb = { 0 };

	clim_pixelcolor_t color = { 0 };

	for (; pixels_beg < pixels_end; pixels_beg += bpp)
	{
		memcpy(&color, pixels_beg, sizeof(color));

		rr = (uint32_t)(color.r * alpha);
		gg = (uint32_t)(color.g * alpha);
		bb = (uint32_t)(color.b * alpha);

		color.a = alpha;
		color.r = (uint8_t)(DIV_255_FAST(rr));
		color.g = (uint8_t)(DIV_255_FAST(gg));
		color.b = (uint8_t)(DIV_255_FAST(bb));

		memcpy(pixels_beg, &color, sizeof(color));
	}

	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_to_grayscale(clim_img_ctx_t* pctx)
{
	if (!pctx)
		return CLIM_EC_INVALID_PARAMETERS;

	const uint32_t width  = pctx->data.width;
	const uint32_t height = pctx->data.height;

	uint8_t* pixels = pctx->data.pixels;
	uint8_t bpp     = sizeof(clim_pixelcolor_t);
	clim_pixelcolor_t color = {0};
	uint8_t gs = { 0 };

	// #pragma omp parallel for
	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			memcpy(&color, &pixels[bpp * (y * width + x)], sizeof(color));
			
			gs = (uint8_t)((0.299 * color.r) + (0.587 * color.g) + (0.114 * color.b));		
			color.r = color.g = color.b = gs;

			memcpy(&pixels[bpp * (y * width + x)], &color, sizeof(color));
		}
	}

	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_to_negative(clim_img_ctx_t* pctx)
{
	if (!pctx)
		return CLIM_EC_INVALID_PARAMETERS;

	const uint32_t width  = pctx->data.width;
	const uint32_t height = pctx->data.height;

	uint8_t* pixels = pctx->data.pixels;
	uint8_t  bpp    = sizeof(clim_pixelcolor_t);
	clim_pixelcolor_t color = { 0 };

	// #pragma omp parallel for
	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			memcpy(&color, &pixels[bpp * (y * width + x)], sizeof(color));

			color.r = (uint8_t)(UINT8_MAX - color.r);
			color.g = (uint8_t)(UINT8_MAX - color.g);
			color.b = (uint8_t)(UINT8_MAX - color.b);

			memcpy(&pixels[bpp * (y * width + x)], &color, sizeof(color));
		}
	}

	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_flip(clim_img_ctx_t* pctx, 
							 clim_img_flip_orientation_t orientation)
{
	if (!pctx)
		return CLIM_EC_INVALID_PARAMETERS;

	const uint32_t width  = pctx->data.width;
	const uint32_t height = pctx->data.height;
	uint8_t bpp			  = sizeof(clim_pixelcolor_t);
	size_t rowsize = pctx->data.width * bpp;

	uint8_t* pixels = pctx->data.pixels;
	uint8_t* pixels_end = pctx->data.pixels + (width * height * bpp);

	if (orientation == CLIM_FLIP_ORIENTATION_VER)
	{
		// reverse all rows (swap last with the first)
		clim_mem_rev(pixels, pixels_end);

		// reverse line by line
		for (; pixels < pixels_end; pixels += rowsize)
			clim_mem_rev(pixels, pixels + rowsize);
	}
	else
	{
		// reverse line by line
		for (; pixels < pixels_end; pixels += rowsize)
			clim_mem_rev(pixels, pixels + rowsize);

		// reset pixels state
		pixels = pctx->data.pixels;

		// reverses the order of the pixels of the lines
		for (; pixels < pixels_end; pixels += bpp)
			clim_mem_rev(pixels, pixels + bpp);
	}

	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_to_sepia(clim_img_ctx_t* pctx)
{
	if (!pctx)
		return CLIM_EC_INVALID_PARAMETERS;

	const uint32_t width = pctx->data.width;
	const uint32_t height = pctx->data.height;

	uint8_t  bpp = sizeof(clim_pixelcolor_t);
	uint8_t* pixels = pctx->data.pixels;
	clim_pixelcolor_t color = { 0 };
	uint32_t r1, g1, b1;

	// #pragma omp parallel for
	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			memcpy(&color, &pixels[bpp * (y * width + x)], sizeof(color));
			
			r1 = U32((color.r * .393) + (color.g * .769) + (color.b * .189));
			g1 = U32((color.r * .349) + (color.g * .686) + (color.b * .168));
			b1 = U32((color.r * .272) + (color.g * .534) + (color.b * .131));

			color.r = (uint8_t)(min_u32(r1, 0xff));
			color.g = (uint8_t)(min_u32(g1, 0xff));
			color.b = (uint8_t)(min_u32(b1, 0xff));

			memcpy(&pixels[bpp * (y * width + x)], &color, sizeof(color));
		}
	}

	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_blur(clim_img_ctx_t* pctx, uint8_t radius)
{
	if(!pctx)
		return CLIM_EC_INVALID_PARAMETERS;

	bool ok = clim_stack_blur(pctx->data.pixels, pctx->data.width, 
							  pctx->data.height, radius, 1);
	if (!ok)
		return CLIM_EC_INVALID_VALUE_RANGE;

	return CLIM_EC_SUCCESS;
}
