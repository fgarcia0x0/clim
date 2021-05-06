#ifndef CLIM_ARGS_PARSER_H
#define CLIM_ARGS_PARSER_H

#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "clim_base.h"
#include "clim_logger.h"
#include "clim_utils.h"
#include "clim_img.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

typedef struct
{
	const char* input_filepath;
	const char* output_filepath;

	uint32_t img_width, img_height;
	clim_pixelcolor_t img_color;
	clim_img_format_t img_fmt;

	uint8_t pixel_depth; //  (24 or 32) bpp
	uint8_t opacity;     //  0 - 255
	uint8_t blur_radius; //  0 - 255
	uint8_t quality;     //  0 - 100
	clim_img_flip_orientation_t flip_orientation; // horizontal - vertical

	unsigned flag_make			: 1U,
             flag_help			: 1U,
			 flag_input			: 1U,
			 flag_output		: 1U,
			 flag_quality		: 1U,
			 flag_overwrite		: 1U,
			 flag_format		: 1U,
			 flag_fillcolor		: 1U,
			 flag_pixel_depth	: 1U,
			 flag_negative		: 1U,
			 flag_opacity		: 1U,
			 flag_flip			: 1U,
			 flag_blur			: 1U,
			 flag_grayscale     : 1U,
			 flag_sepia			: 1U;

} clim_args_options_t;

typedef union
{
	uint8_t  u8[sizeof(uint32_t)];
	uint32_t u32;
} u8_to_u32;

#define CLIM_ARGS_NUM(...)  (sizeof((const char *[]){__VA_ARGS__}) / sizeof(const char*))
#define CLIM_ARGS_LIST(...) (const char*[]){__VA_ARGS__}

static bool clim_parse_quality_format(const char* arg, clim_args_options_t* popt)
{
	uint8_t quality = UINT8_C(0);

	if (strlen(arg) > 3U)
		return false;

	if (CLIM_FAILED(clim_to_u8(arg, 10, &quality)))
		return false;

	if (quality > UINT8_C(100))
		return false;

	popt->quality = quality;
	return true;
}

static bool clim_parse_img_format(const char* arg, clim_args_options_t* popt)
{
	clim_img_format_t fmt = CLIM_IMAGE_FORMAT_UNKNOWN;

	if (strlen(arg) > 3U)
		return false;

	if (!memcmp(arg, "jpg", 3U))
		fmt = CLIM_IMAGE_FORMAT_JPG;
	else if (!memcmp(arg, "png", 3U))
		fmt = CLIM_IMAGE_FORMAT_PNG;
	else if (!memcmp(arg, "bmp", 3U))
		fmt = CLIM_IMAGE_FORMAT_BMP;
	else if (!memcmp(arg, "pgm", 3U))
		fmt = CLIM_IMAGE_FORMAT_PGM;
	else if (!memcmp(arg, "ppm", 3U))
		fmt = CLIM_IMAGE_FORMAT_PPM;
	else if (!memcmp(arg, "pbm", 3U))
		fmt = CLIM_IMAGE_FORMAT_PBM;
	else
		return false;
	
	popt->img_fmt = fmt;
	return true;
}

static bool clim_parse_fillcolor_format(const char* arg, clim_args_options_t* popt)
{
	CLIM_LOG_DEBUG("Option Arg: %s", arg);
	clim_pixelcolor_t* pixel_ptr = &popt->img_color;
	clim_errcode_t errc = CLIM_EC_SUCCESS;
	const char* keyword = "0x";

	if (!strncmp(arg, keyword, strlen(keyword)))
	{
		const char* parg = arg + strlen(keyword);
		size_t pair_cnt = parg ? strlen(parg) : 0U;

		if (!pair_cnt || (pair_cnt != 8U && pair_cnt != 6U))
			return false;

		uint32_t color = 0;
		
		errc = clim_to_u32(parg, 16, &color);
		if (CLIM_FAILED(errc))
		{
			CLIM_LOG_ERROR("%s", clim_err_get_msg(errc));
			return false;
		}

		pixel_ptr->a = (pair_cnt == 6U) ? (0xFFU) : (uint8_t)((color >> 24U) & UINT8_MAX);
		pixel_ptr->r = (uint8_t)((color >> 16U) & UINT8_MAX);
		pixel_ptr->g = (uint8_t)((color >> 8U)  & UINT8_MAX);
		pixel_ptr->b = (uint8_t)((color)		& UINT8_MAX);

		CLIM_LOG_DEBUG("Color Output: (%hhu, %hhu, %hhu, %hhu)\n",
					   pixel_ptr->a, pixel_ptr->r,
					   pixel_ptr->g, pixel_ptr->b);

		return true;
	}
	else
	{
		const char* txtptr = arg;

		char delim = '-';
		uint32_t num_cnt = 0U;

		const char* dig_track = txtptr;
		const char* save = txtptr;
		size_t diff = 0;

		while (true)
		{
			save = dig_track = txtptr;

			for (; isdigit((uint8_t)*txtptr); ++txtptr)
				++dig_track;

			diff = (size_t)(dig_track - save);
			if (diff == 0U || diff > 3U)
				return false;

			++num_cnt;

			if (*txtptr == '\0')
				break;

			if (*txtptr != delim || !isdigit(*(txtptr + 1U)))
				return false;

			++txtptr;
		}

		if (num_cnt < 3U || num_cnt > 4U)
			return false;

		// reset state
		const char* temp = NULL;
		txtptr = arg;

		u8_to_u32 u = { 0 };
		uint8_t* pargb = u.u8;
		uint8_t number = 0U;

		if (num_cnt == 3U)
			*pargb++ = 0xFFU;

		// extract digits
		for (uint32_t i = 0U; i < num_cnt; ++i)
		{
			errc = clim_to_u8(txtptr, 10, &number);
			if (CLIM_FAILED(errc))
			{
				CLIM_LOG_ERROR("%s", clim_err_get_msg(errc));
				return false;
			}

			*pargb++ = number;
			CLIM_LOG_DEBUG("extracted = %hhu", number);

			if (!txtptr)
				return false;

			if (!(temp = strchr(txtptr, delim)))
				break;

			txtptr = clim_next_digit(temp);
		}

		pixel_ptr->native = u.u32;

		CLIM_LOG_DEBUG("Color Output: (%hhu, %hhu, %hhu, %hhu)\n",
					   pixel_ptr->a, pixel_ptr->r, pixel_ptr->g, pixel_ptr->b);

		return true;
	}
}

static bool clim_parse_opacity_format(const char* arg, clim_args_options_t* popt)
{
	uint8_t opacity = UINT8_C(0);

	clim_errcode_t errc = clim_to_u8(arg, 10, &opacity);
	if (CLIM_FAILED(errc))
	{
		CLIM_LOG_ERROR("%s", clim_err_get_msg(errc));
		return false;
	}

	popt->opacity = opacity;
	return true;
}

static bool clim_parse_blur_format(const char* arg, clim_args_options_t* popt)
{
	uint8_t value = UINT8_C(0);

	clim_errcode_t errc = clim_to_u8(arg, 10, &value);
	if(CLIM_FAILED(errc))
	{
		CLIM_LOG_ERROR("%s", clim_err_get_msg(errc));
		return false;
	}

	if (value < 2U || value == UINT8_MAX)
		return false;

	popt->blur_radius = value;
	return true;
}

static bool clim_parse_pixel_depth_format(const char* arg, clim_args_options_t* popt)
{
	uint8_t pixel_depth = UINT8_C(0);

	clim_errcode_t errc = clim_to_u8(arg, 10, &pixel_depth);
	if (CLIM_FAILED(errc))
	{
		CLIM_LOG_ERROR("%s", clim_err_get_msg(errc));
		return false;
	}

	if (pixel_depth != UINT8_C(24) && pixel_depth != UINT8_C(32))
		return false;

	popt->pixel_depth = pixel_depth;
	return true;
}

static bool clim_parse_flip_format(const char* arg, clim_args_options_t* popt)
{
	if (!strcmp(arg, "horizontal"))
		popt->flip_orientation = CLIM_FLIP_ORIENTATION_HOR;
	else if (!strcmp(arg, "vertical"))
		popt->flip_orientation = CLIM_FLIP_ORIENTATION_VER;
	else
		return false;

	return true;
}

static bool clim_parse_make_format(const char* next_args, clim_args_options_t* popt)
{
	const size_t curr_arg_len = strlen(next_args);
	size_t num_digits = clim_str_count_if(next_args, next_args + curr_arg_len, isdigit);

	if ((num_digits != (curr_arg_len - 1)) || !strchr(next_args, ':'))
		return false;

	clim_errcode_t errc = CLIM_EC_SUCCESS;
	errc = clim_to_u32(next_args, 10, &popt->img_width);

	if (CLIM_FAILED(errc))
	{
		CLIM_LOG_ERROR("%s", clim_err_get_msg(errc));
		return false;
	}

	if (popt->img_width == 0U)
	{
		CLIM_LOG_ERROR("the width size must be greater than zero");
		return false;
	}

	// read next part
	errc = clim_to_u32(strchr(next_args, ':') + 1U, 10, &popt->img_height);
	
	if (CLIM_FAILED(errc))
	{
		CLIM_LOG_ERROR("%s", clim_err_get_msg(errc));
		return false;
	}

	if (popt->img_height == 0U)
	{
		CLIM_LOG_ERROR("the height size must be greater than zero\n");
		return false;
	}

	popt->flag_make = true;
	return true;
}

static bool clim_args_read(const char* argv[], size_t argc, clim_args_options_t* pout_options)
{
	if (!argv || !argc)
	{
		CLIM_LOG_ERROR("no input passed");
		CLIM_LOG_WARN("try the \"--help\" command to get the list of possible commands\n");
		return false;
	}

	const char** arg_tracker = argv;
	const char** arg_end = argv + argc;
	const char* curr_arg = NULL, **next_arg = NULL;

	bool b1 = clim_contains(arg_tracker, (size_t)(arg_end - arg_tracker),
							"--help", strcmp);
	
	bool b2 = clim_contains(arg_tracker, (size_t)(arg_end - arg_tracker),
							"-h", strcmp);

	if (b1 || b2)
	{
		pout_options->flag_help = true;
		return true;
	}

	for (; arg_tracker != arg_end; ++arg_tracker)
	{
		curr_arg = *arg_tracker;
		next_arg = arg_tracker + 1;

		if (CLIM_CONTAINS(curr_arg, "-i", "--input"))
		{
			if (pout_options->flag_input)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (pout_options->flag_make)
			{
				CLIM_LOG_ERROR("The past arguments are ambiguous, you should only choose a mode of operation, or you use the \"%s\" or \"%s\"",
					"--make", "-i/--input");
				CLIM_LOG_WARN("Use the \"--help\" command for more details");
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("The option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			pout_options->flag_input = true;
			pout_options->input_filepath = *next_arg;
			
			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-o", "--output"))
		{
			if (pout_options->flag_output)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			pout_options->flag_output = true;
			pout_options->output_filepath = *next_arg;

			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-m", "--make"))
		{
			if (pout_options->flag_make)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (pout_options->flag_input)
			{
				CLIM_LOG_ERROR("The past arguments are ambiguous, you should only choose a mode of operation, or you use the \"%s\" or \"%s\"",
							   "-m/--make", "-i/--input");

				CLIM_LOG_WARN("Use the \"--help\" command for more details\n");
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			if (!clim_parse_make_format(*next_arg, pout_options))
			{
				CLIM_LOG_ERROR("invalid format for \"%s\" command\n", curr_arg);
				return false;
			}

			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-fc", "--fillcolor"))
		{
			if (pout_options->flag_fillcolor)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			pout_options->flag_fillcolor = true;

			if (!clim_parse_fillcolor_format(*next_arg, pout_options))
			{
				CLIM_LOG_ERROR("invalid color format");
				return false;
			}

			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-f", "--format"))
		{
			if (pout_options->flag_format)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			pout_options->flag_format = true;

			if (!clim_parse_img_format(*next_arg, pout_options))
			{
				CLIM_LOG_ERROR("invalid image format");
				return false;
			}

			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-q", "--quality"))
		{
			if (pout_options->flag_quality)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			pout_options->flag_quality = true;

			if (!clim_parse_quality_format(*next_arg, pout_options))
			{
				CLIM_LOG_ERROR("invalid quality range");
				return false;
			}

			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-t", "--opacity"))
		{
			if (pout_options->flag_opacity)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			if (!clim_parse_opacity_format(*next_arg, pout_options))
			{
				CLIM_LOG_ERROR("invalid opacity range");
				return false;
			}

			pout_options->flag_opacity = true;
			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-b", "--blur"))
		{
			if (pout_options->flag_blur)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			if (!clim_parse_blur_format(*next_arg, pout_options))
			{
				CLIM_LOG_ERROR("invalid blur range");
				return false;
			}

			pout_options->flag_blur = true;
			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-fl", "--flip"))
		{
			if (pout_options->flag_flip)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			pout_options->flag_flip = true;

			if (!clim_parse_flip_format(*next_arg, pout_options))
			{
				CLIM_LOG_ERROR("invalid orientation argument");
				return false;
			}

			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-d", "--pixel-depth"))
		{
			if (pout_options->flag_pixel_depth)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			if (!*next_arg || !strlen(*next_arg))
			{
				CLIM_LOG_ERROR("option \"%s\" requires at least one argument", curr_arg);
				return false;
			}

			pout_options->flag_pixel_depth = true;

			if (!clim_parse_pixel_depth_format(*next_arg, pout_options))
			{
				CLIM_LOG_ERROR("invalid pixel depth range argument");
				return false;
			}

			++arg_tracker;
		}
		else if (CLIM_CONTAINS(curr_arg, "-g", "--grayscale"))
		{
			if (pout_options->flag_grayscale)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			pout_options->flag_grayscale = true;
		}
		else if (CLIM_CONTAINS(curr_arg, "-n", "--negative"))
		{
			if (pout_options->flag_negative)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			pout_options->flag_negative = true;
		}
		else if (CLIM_CONTAINS(curr_arg, "-w", "--overwrite"))
		{
			if (pout_options->flag_overwrite)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			pout_options->flag_overwrite  = true;
			pout_options->flag_output     = true;
			pout_options->output_filepath = pout_options->input_filepath;
		}
		else if (CLIM_CONTAINS(curr_arg, "-s", "--sepia"))
		{
			if (pout_options->flag_sepia)
			{
				CLIM_LOG_ERROR("The option \"%s\" has already been set previously", curr_arg);
				return false;
			}

			pout_options->flag_sepia = true;
		}
		else
		{
			CLIM_LOG_ERROR("The option \"%s\" is not recognized by the CLIM.", curr_arg);
			return false;
		}
	}

	if (!pout_options->flag_input && !pout_options->flag_make)
	{
		CLIM_LOG_ERROR("you need to specify how the image will be loaded (-i/--input or "
					   "-m/--make).");
		
		CLIM_LOG_WARN("type \"--help\" command to see details about it.\n");
		
		return false;
	}

	if (!pout_options->flag_output)
	{
		CLIM_LOG_ERROR("It is necessary that the output path has been specified\n");
		return false;
	}

	return true;
}

#endif
