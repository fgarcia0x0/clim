#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../include/clim_base.h"
#include "../include/clim_img.h"
#include "../include/clim_logger.h"
#include "../include/clim_utils.h"
#include "../include/clim_args_parser.h"
#include "../include/clim_banner.h"

#ifdef CLIM_OS_WIN
	#include "../include/clim_win_utils.h"
#endif

static inline void clim_usage(void)
{
	printf("%s", s_banners_formatted[CLIM_BANNER_STYLE_3DFILLED]);

	printf("\n\t\t\t[CLIM - COMMAND LINE IMAGE MANIPULATOR]\n\n"
		"clim.exe [options] [transformations]\n\n"
		"[options]: \n"
		"  -m, --make          [width:height]              Created an image with dimensions specified.\n"
		"  -i, --input         [filepath]                  Loads an image from a path.\n"
		"  -o, --output        [filepath]                  Saves an image on a specified path\n"
		"  -f, --format        [image_format]              Specify image format of output image.\n"
		"  -d, --pixel-depth   [24 or 32]                  Specifies the depths (bits per pixel) of the image pixels. \n"  
		"  -q, --quality       [0 - 100]                   Specifies the quality of the output image.\n"
		"  -w, --overwrite                                 Instead of creating a copy, it makes the changes \n"
		"                                                  in the input image, overwriting it.\n"
		"  -h, --help                                      Show help menu.\n\n");

	printf("[transformations]: \n"
	    "  -fc, --fillcolor   [color_format]               Fills an entire image with a certain color format.\n"
	    "  -n,  --negative                                 Applies the negative effect in output image.\n"
	    "  -t,  --opacity     [0 - 255]                    Specifies the opacity of the output image.\n"
		"  -fl, --flip        [vertical or horizontal]     Turn the output image in a specified direction.\n" 
		"  -g,  --grayscale                                Applies grayscale effect in output image.\n"
		"  -b,  --blur        [2 - 254]                    Applies blur effect in output image.\n"
		"  -s,  --sepia                                    Applies sepia effect in output image.\n\n");
	
	// Color Format
	printf("┌─[color_format]:\n"); 
	printf("└─────► 0xaarrggbb (Hex ARGB format)\n");
	printf("└─────► 0xrrggbb   (Hex RGB format)\n"); 
	printf("└─────► alpha-red-green-blue (Decimal ARGB format)\n");
	printf("└─────► red-green-blue       (Decimal RGB format)\n\n");

	// Image Format
	printf("┌─[image_format]:\n");
	printf("└─────► [jpg, png, bmp, pgm, ppm, pbm]\n");

	// the stream  already discharges the flow after a '\ n'
	putchar('\n');
}

static inline void clim_init(void)
{
	#ifdef CLIM_OS_WIN
	
		CLIM_DO_IF(
			!clim_win_enable_utf8_console_output(),
			CLIM_LOG_WARN("Fail to configure console codepage to utf8")
		);

		CLIM_DO_IF(
			!clim_win_enable_virtual_terminal_sequences(),
			CLIM_LOG_WARN("Fail to configure windows virtual terminal sequences")
		);

	#endif

	CLIM_LOG_DEBUG("Running in Debug Mode");
	CLIM_LOG_DEBUG("clim has been initialized");
}

static bool clim_parse_options(clim_args_options_t* options_ptr)
{
	clim_img_ctx_t ctx  = { 0 };
	clim_errcode_t errc = CLIM_EC_SUCCESS;

	if (options_ptr->flag_help)
	{
		clim_usage();
		return true;
	}

	if (options_ptr->flag_input)
	{
		errc = clim_img_load(options_ptr->input_filepath, &ctx);

		if (CLIM_FAILED(errc))
		{
			CLIM_LOG_ERROR("Cannot load image in memory, reason: \"%s\"\n", clim_err_get_msg(errc));
			return false;
		}

		CLIM_LOG_DEBUG("[+] Create image in memory via \"--input\" (OK)");
	}

	if (options_ptr->flag_make)
	{
		errc = clim_img_make(options_ptr->img_width, options_ptr->img_height, 
							 sizeof(clim_pixelcolor_t) << 3U, CLIM_IMAGE_FORMAT_BMP, &ctx);

		if (CLIM_FAILED(errc))
		{
			CLIM_LOG_ERROR("Cannot make image in memory, reason: \"%s\"\n", clim_err_get_msg(errc));
			return false;
		}

		CLIM_LOG_DEBUG("[+] Create image in memory via \"--make\" (OK)");
	}

	if (options_ptr->flag_fillcolor)
		clim_img_fillcolor(&ctx, options_ptr->img_color);

	if (options_ptr->flag_opacity)
		clim_img_set_opacity(&ctx, options_ptr->opacity);

	if (options_ptr->flag_grayscale)
		clim_img_to_grayscale(&ctx);

	if (options_ptr->flag_negative)
		clim_img_to_negative(&ctx);

	if (options_ptr->flag_flip)
		clim_img_flip(&ctx, options_ptr->flip_orientation);
	
	if (options_ptr->flag_blur)
		clim_img_blur(&ctx, options_ptr->blur_radius);

	if (options_ptr->flag_sepia)
		clim_img_to_sepia(&ctx);

	if (options_ptr->flag_output)
	{
		clim_img_format_t fmt = options_ptr->flag_format ? options_ptr->img_fmt : ctx.format;
		clim_img_quality_t img_quality = options_ptr->flag_quality ? options_ptr->quality : 75;
		const char* img_path = (options_ptr->flag_overwrite) ? options_ptr->input_filepath : options_ptr->output_filepath;

		if (options_ptr->flag_pixel_depth)
			ctx.data.bytes_per_pixel = options_ptr->pixel_depth >> 3U;

		errc = clim_img_write(img_path, &ctx, fmt, img_quality);
		if (CLIM_FAILED(errc))
		{
			CLIM_LOG_ERROR("Cannot write image to output file, reason: \"%s\"\n", 
							clim_err_get_msg(errc));
			return false;
		}

		CLIM_LOG_DEBUG("[+] write image to output file (OK)");
	}

	clim_img_release(&ctx);
	return true;
}

int main(int argc, const char* argv[])
{
	clim_init();

	clim_args_options_t options = { 0 };
	bool parsed = clim_args_read(argv + 1U, (size_t)(argc) - 1U, &options);

	if (!parsed)
		return EXIT_FAILURE;

	parsed = clim_parse_options(&options);
	if (!parsed)
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}
