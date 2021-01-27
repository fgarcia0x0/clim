#include "..\include\clim_base.h"
#include "..\include\clim_img.h"
#include "..\include\clim_img_utils.h"
#include "..\include\clim_logger.h"

int main()
{
	// img_ctx_t* pctx = NULL;

	// if (clim_img_load("load.bmp", &pctx) == CLIM_EC_SUCCESS)
	// {
	// 	for (size_t i = 0; i < CLIM_IMG_GET_HEIGTH(pctx); ++i)
	// 	{
	// 		for (size_t j = 0; j < CLIM_IMG_GET_WIDTH(pctx); ++j)
	// 		{
	// 			argb_t pix_color = CLIM_IMG_GET_PIXEL(pctx, i, j);
	// 			img_set_pixel_argb(pctx, i, j, pix_color);
	// 		}
	// 	}
	// }
	
	///CLIM_LOG_INFO("Init Commit");
	///CLIM_LOG_WARN("Warn Ocurred");
	///CLIM_LOG_ERROR("Fail Ocurred");
	////CLIM_LOG_CRITICAL("Critical Error Ocurred");

	char16_t* tstr = u"عندما يريد العالم أن ‪يتكلّم ";
	size_t bytes = 0;
	char out[1024] = {0};

	if (CLIM_SUCCESS(clim_utf16_to_utf8(tstr, out, sizeof out, &bytes)))
	{
		if (bytes != SIZE_MAX)
		{
			printf("%u - %s\n", (unsigned) bytes, out);
		}
	}

	return 0;
}