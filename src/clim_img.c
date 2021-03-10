#include <string.h>

#include "../include/clim_img.h"
#include "../include/clim_utils.h"
#include "../include/clim_logger.h"
#include "../include/clim_filesystem.h"

static inline void clim_get_argb_from_pixels(
	const clim_img_ctx_t* pctx, size_t index, 
	size_t len, clim_pixelcolor_t* pout_color
)
{
	pout_color->a = pctx->data.pixels[(index * len)   ];
	pout_color->r = pctx->data.pixels[(index * len) + 1];
	pout_color->g = pctx->data.pixels[(index * len) + 2];
	pout_color->b = pctx->data.pixels[(index * len) + 3];
}

void clim_img_set_pixel(
	clim_img_ctx_t* pctx, 
	size_t xpos, size_t ypos, 
	clim_pixelcolor_t color
)
{
	CLIM_ASSERT(pctx && pctx->data.pixels);
	CLIM_ASSERT(xpos < CLIM_IMG_GET_WIDTH(pctx));
	CLIM_ASSERT(ypos < CLIM_IMG_GET_HEIGTH(pctx));

	const size_t width = CLIM_IMG_GET_WIDTH(pctx);
	const size_t index = ypos * width + xpos;
	const size_t len = sizeof(clim_pixelcolor_t);
	clim_pixelcolor_t target = color;

	uint8_t Alpha = target.a;

	if (Alpha > 0 && Alpha < 0xffu)
	{
		clim_pixelcolor_t pix_color = {0};
		clim_get_argb_from_pixels(pctx, index, len, &pix_color);

		uint32_t blend_color = clim_alpha_blend_pixels(
			clim_pixelcolor_to_uint32(pix_color), 
			clim_pixelcolor_to_uint32(color)
		);

		target = clim_uint32_to_argb(blend_color);
	}

	pctx->data.pixels[(index * len)    ]  = target.a;
	pctx->data.pixels[(index * len) + 1]  = target.r;
	pctx->data.pixels[(index * len) + 2]  = target.g;
	pctx->data.pixels[(index * len) + 3]  = target.b;
}

clim_pixelcolor_t clim_img_get_pixel(
	const clim_img_ctx_t* restrict pctx, 
	size_t xpos, size_t ypos
) 
{
	CLIM_ASSERT(pctx && pctx->data.pixels);

	const size_t width = CLIM_IMG_GET_WIDTH(pctx);
	CLIM_ASSERT(xpos < width && ypos < CLIM_IMG_GET_HEIGTH(pctx));

	const size_t index = xpos * width + ypos;
	const size_t len = sizeof(clim_pixelcolor_t);

	clim_pixelcolor_t result = {0};
	clim_get_argb_from_pixels(pctx, index, len, &result);

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

	FILE* fileptr = fopen(filepath, "rb");
	if (!fileptr)
		return CLIM_EC_CANNOT_OPEN_FILE;

	clim_img_format_t format = clim_get_img_format(fileptr);

	if (fseek(fileptr, 0, SEEK_SET) || ferror(fileptr))
	{
		clim_fclose(fileptr);
		return CLIM_EC_CANNOT_READ_FILE;
	}

	switch (format)
	{
		case CLIM_IMAGE_FORMAT_BITMAP:
			CLIM_LOG_INFO("File Format: [BMP]\n");
			CLIM_LOG_INFO("Not Implemented Yeat");
			break;
		case CLIM_IMAGE_FORMAT_PGM:
			CLIM_LOG_INFO("File Format: [PGM]\n");
			CLIM_LOG_INFO("Not Implemented Yeat");
			break;
		case CLIM_IMAGE_FORMAT_PNG:
			CLIM_LOG_INFO("File Format: [PNG]\n");
			CLIM_LOG_INFO("Not Implemented Yeat");
			break;
		case CLIM_IMAGE_FORMAT_JPEG:
			CLIM_LOG_INFO("File Format: [JPG]\n");
			CLIM_LOG_INFO("Not Implemented Yeat");
			break;
		default:
			CLIM_LOG_DEBUG("Unsupported Image Format");
			clim_fclose(fileptr);
			return CLIM_EC_UNSUPPORTED_IMAGE_FORMAT;
	}

	clim_fclose(fileptr);
	return CLIM_EC_SUCCESS;
}

clim_errcode_t clim_img_release(
	clim_img_ctx_t* restrict pctx
)
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

	if (filepath_len >= CLIM_MAX_OS_PATH)
	{
		CLIM_LOG_DEBUG(
			"this path: \"%.*s...\" is too long", 
			CLIM_MAX_OS_PATH, filename
		);
		return CLIM_EC_PATH_IS_TOO_LONG;
	}

	FILE* img_handle = fopen(filename, "wb");
	if (!img_handle)
		return CLIM_EC_CANNOT_OPEN_FILE;

	switch (format)
	{
		case CLIM_IMAGE_FORMAT_BITMAP:
		case CLIM_IMAGE_FORMAT_PGM:
		case CLIM_IMAGE_FORMAT_PNG:
		case CLIM_IMAGE_FORMAT_JPEG:
			CLIM_LOG_INFO("Not Implemented Yeat");
			break;
		default:
			CLIM_LOG_ERROR("Invalid Image Format");
			break;
	}

	clim_fclose(img_handle);
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
