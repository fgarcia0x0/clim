#ifndef CLIM_IMG_BLUR_H
#define CLIM_IMG_BLUR_H 

#include <stdint.h>
#include "clim_base.h"

#define U32(expr) (uint32_t)(expr)

static uint16_t stackblur_mul[0xff] =
{
	512, 512, 456, 512, 328, 456, 335, 512, 405, 328, 271, 456, 388, 335, 292, 512,
	454, 405, 364, 328, 298, 271, 496, 456, 420, 388, 360, 335, 312, 292, 273, 512,
	482, 454, 428, 405, 383, 364, 345, 328, 312, 298, 284, 271, 259, 496, 475, 456,
	437, 420, 404, 388, 374, 360, 347, 335, 323, 312, 302, 292, 282, 273, 265, 512,
	497, 482, 468, 454, 441, 428, 417, 405, 394, 383, 373, 364, 354, 345, 337, 328,
	320, 312, 305, 298, 291, 284, 278, 271, 265, 259, 507, 496, 485, 475, 465, 456,
	446, 437, 428, 420, 412, 404, 396, 388, 381, 374, 367, 360, 354, 347, 341, 335,
	329, 323, 318, 312, 307, 302, 297, 292, 287, 282, 278, 273, 269, 265, 261, 512,
	505, 497, 489, 482, 475, 468, 461, 454, 447, 441, 435, 428, 422, 417, 411, 405,
	399, 394, 389, 383, 378, 373, 368, 364, 359, 354, 350, 345, 341, 337, 332, 328,
	324, 320, 316, 312, 309, 305, 301, 298, 294, 291, 287, 284, 281, 278, 274, 271,
	268, 265, 262, 259, 257, 507, 501, 496, 491, 485, 480, 475, 470, 465, 460, 456,
	451, 446, 442, 437, 433, 428, 424, 420, 416, 412, 408, 404, 400, 396, 392, 388,
	385, 381, 377, 374, 370, 367, 363, 360, 357, 354, 350, 347, 344, 341, 338, 335,
	332, 329, 326, 323, 320, 318, 315, 312, 310, 307, 304, 302, 299, 297, 294, 292,
	289, 287, 285, 282, 280, 278, 275, 273, 271, 269, 267, 265, 263, 261, 259
};

static uint8_t stackblur_shr[0xff] =
{
	9, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
	17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
	19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
	20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
	22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
};

static void clim_stack_blur_job(uint8_t* pixels, uint32_t width, uint32_t height, 
							    uint8_t radius, uint32_t cores, uint32_t core, 
							    uint32_t step, uint8_t* stack)
{
	uint32_t xp = 0, yp = 0;
	uint32_t sp = 0;
	uint32_t stack_start = 0;

	uint8_t* stack_ptr = (uint8_t *)0x0;
	uint8_t* src_ptr   = (uint8_t *)0x0;
	uint8_t* dst_ptr   = (uint8_t *)0x0;

	uint32_t sum_r	   = 0;
	uint32_t sum_g	   = 0;
	uint32_t sum_b	   = 0;
	uint32_t sum_a	   = 0;
	uint32_t sum_in_r  = 0;
	uint32_t sum_in_g  = 0;
	uint32_t sum_in_b  = 0;
	uint32_t sum_in_a  = 0;
	uint32_t sum_out_r = 0;
	uint32_t sum_out_g = 0;
	uint32_t sum_out_b = 0;
	uint32_t sum_out_a = 0;

	const uint32_t wm      = U32(width - 1);
	const uint32_t hm      = U32(height - 1);
	const uint32_t w4      = U32(width << 2);
	const uint32_t div     = U32((radius << 1) + 1);
	const uint32_t mul_sum = U32(stackblur_mul[radius]);
	const uint8_t shr_sum  = U32(stackblur_shr[radius]);

	if (step == 1)
	{
		const uint32_t minY = core * height / cores;
		const uint32_t maxY = (core + 1) * height / cores;

		for (uint32_t y = minY; y < maxY; y++)
		{
			sum_r     = sum_g     = sum_b     = sum_a     =
			sum_in_r  = sum_in_g  = sum_in_b  = sum_in_a  = 
			sum_out_r = sum_out_g = sum_out_b = sum_out_a = 0;

			src_ptr = pixels + w4 * y; // start of line (0,y)

			for (uint32_t i = 0; i <= radius; i++)
			{
				stack_ptr    = &stack[sizeof(clim_pixelcolor_t) * i];
				stack_ptr[0] = src_ptr[0];
				stack_ptr[1] = src_ptr[1];
				stack_ptr[2] = src_ptr[2];
				stack_ptr[3] = src_ptr[3];
				sum_r       += src_ptr[0] * (i + 1);
				sum_g       += src_ptr[1] * (i + 1);
				sum_b       += src_ptr[2] * (i + 1);
				sum_a       += src_ptr[3] * (i + 1);
				sum_out_r   += src_ptr[0];
				sum_out_g   += src_ptr[1];
				sum_out_b   += src_ptr[2];
				sum_out_a   += src_ptr[3];
			}

			for (uint32_t i = 1; i <= radius; i++)
			{
				if (i <= wm) src_ptr += sizeof(clim_pixelcolor_t);
				stack_ptr    = &stack[sizeof(clim_pixelcolor_t) * (i + radius)];
				stack_ptr[0] = src_ptr[0];
				stack_ptr[1] = src_ptr[1];
				stack_ptr[2] = src_ptr[2];
				stack_ptr[3] = src_ptr[3];
				sum_r       += src_ptr[0] * (radius + 1 - i);
				sum_g       += src_ptr[1] * (radius + 1 - i);
				sum_b       += src_ptr[2] * (radius + 1 - i);
				sum_a       += src_ptr[3] * (radius + 1 - i);
				sum_in_r    += src_ptr[0];
				sum_in_g    += src_ptr[1];
				sum_in_b    += src_ptr[2];
				sum_in_a    += src_ptr[3];
			}

			sp = radius;
			xp = radius;

			if (xp > wm) 
				xp = wm;

			src_ptr = pixels + sizeof(clim_pixelcolor_t) * (xp + y * width);
			dst_ptr = pixels + y * w4;

			for (uint32_t x = 0; x < width; x++)
			{
				dst_ptr[0] = (uint8_t)((sum_r * mul_sum) >> shr_sum);
				dst_ptr[1] = (uint8_t)((sum_g * mul_sum) >> shr_sum);
				dst_ptr[2] = (uint8_t)((sum_b * mul_sum) >> shr_sum);
				dst_ptr[3] = (uint8_t)((sum_a * mul_sum) >> shr_sum);
				dst_ptr += sizeof(clim_pixelcolor_t);

				sum_r -= sum_out_r;
				sum_g -= sum_out_g;
				sum_b -= sum_out_b;
				sum_a -= sum_out_a;

				stack_start = sp + div - radius;
				if (stack_start >= div) 
					stack_start -= div;

				stack_ptr = &stack[sizeof(clim_pixelcolor_t) * stack_start];

				sum_out_r -= stack_ptr[0];
				sum_out_g -= stack_ptr[1];
				sum_out_b -= stack_ptr[2];
				sum_out_a -= stack_ptr[3];

				if (xp < wm)
				{
					src_ptr += sizeof(clim_pixelcolor_t);
					++xp;
				}

				stack_ptr[0] = src_ptr[0];
				stack_ptr[1] = src_ptr[1];
				stack_ptr[2] = src_ptr[2];
				stack_ptr[3] = src_ptr[3];

				sum_in_r += src_ptr[0];
				sum_in_g += src_ptr[1];
				sum_in_b += src_ptr[2];
				sum_in_a += src_ptr[3];
				sum_r    += sum_in_r;
				sum_g    += sum_in_g;
				sum_b    += sum_in_b;
				sum_a    += sum_in_a;

				if (++sp >= div)
					sp = 0;

				stack_ptr = &stack[sp * sizeof(clim_pixelcolor_t)];

				sum_out_r += stack_ptr[0];
				sum_out_g += stack_ptr[1];
				sum_out_b += stack_ptr[2];
				sum_out_a += stack_ptr[3];
				sum_in_r  -= stack_ptr[0];
				sum_in_g  -= stack_ptr[1];
				sum_in_b  -= stack_ptr[2];
				sum_in_a  -= stack_ptr[3];

			}
		}
	}

	// step 2
	if (step == 2)
	{
		uint32_t minX = core * width / cores;
		uint32_t maxX = (core + 1) * width / cores;

		for (uint32_t x = minX; x < maxX; x++)
		{
			sum_r     = sum_g     = sum_b     = sum_a     =
			sum_in_r  = sum_in_g  = sum_in_b  = sum_in_a  =
			sum_out_r = sum_out_g = sum_out_b = sum_out_a = 0;

			src_ptr = pixels + sizeof(clim_pixelcolor_t) * x; // x,0
			for (uint32_t i = 0; i <= radius; i++)
			{
				stack_ptr = &stack[i * sizeof(clim_pixelcolor_t)];
				stack_ptr[0] = src_ptr[0];
				stack_ptr[1] = src_ptr[1];
				stack_ptr[2] = src_ptr[2];
				stack_ptr[3] = src_ptr[3];
				sum_r       += src_ptr[0] * (i + 1);
				sum_g       += src_ptr[1] * (i + 1);
				sum_b       += src_ptr[2] * (i + 1);
				sum_a       += src_ptr[3] * (i + 1);
				sum_out_r   += src_ptr[0];
				sum_out_g   += src_ptr[1];
				sum_out_b   += src_ptr[2];
				sum_out_a   += src_ptr[3];
			}
			for (uint32_t i = 1; i <= radius; i++)
			{
				if (i <= hm) 
					src_ptr += w4;

				stack_ptr = &stack[sizeof(clim_pixelcolor_t) * (i + radius)];
				stack_ptr[0] = src_ptr[0];
				stack_ptr[1] = src_ptr[1];
				stack_ptr[2] = src_ptr[2];
				stack_ptr[3] = src_ptr[3];

				sum_r       += src_ptr[0] * (radius + 1 - i);
				sum_g       += src_ptr[1] * (radius + 1 - i);
				sum_b       += src_ptr[2] * (radius + 1 - i);
				sum_a       += src_ptr[3] * (radius + 1 - i);
				
				sum_in_r    += src_ptr[0];
				sum_in_g    += src_ptr[1];
				sum_in_b    += src_ptr[2];
				sum_in_a    += src_ptr[3];
			}

			sp = radius;
			yp = radius;

			if (yp > hm)
				yp = hm;

			src_ptr = pixels + sizeof(clim_pixelcolor_t) * (x + yp * width);
			dst_ptr = pixels + sizeof(clim_pixelcolor_t) * x;

			for (uint32_t y = 0; y < height; y++)
			{
				dst_ptr[0] = (uint8_t)((sum_r * mul_sum) >> shr_sum);
				dst_ptr[1] = (uint8_t)((sum_g * mul_sum) >> shr_sum);
				dst_ptr[2] = (uint8_t)((sum_b * mul_sum) >> shr_sum);
				dst_ptr[3] = (uint8_t)((sum_a * mul_sum) >> shr_sum);
				dst_ptr   += w4;

				sum_r -= sum_out_r;
				sum_g -= sum_out_g;
				sum_b -= sum_out_b;
				sum_a -= sum_out_a;

				stack_start = sp + div - radius;

				if (stack_start >= div) 
					stack_start -= div;

				stack_ptr = &stack[sizeof(clim_pixelcolor_t) * stack_start];

				sum_out_r -= stack_ptr[0];
				sum_out_g -= stack_ptr[1];
				sum_out_b -= stack_ptr[2];
				sum_out_a -= stack_ptr[3];

				if (yp < hm)
				{
					src_ptr += w4; // stride
					++yp;
				}

				stack_ptr[0] = src_ptr[0];
				stack_ptr[1] = src_ptr[1];
				stack_ptr[2] = src_ptr[2];
				stack_ptr[3] = src_ptr[3];

				sum_in_r += src_ptr[0];
				sum_in_g += src_ptr[1];
				sum_in_b += src_ptr[2];
				sum_in_a += src_ptr[3];
				sum_r    += sum_in_r;
				sum_g    += sum_in_g;
				sum_b    += sum_in_b;
				sum_a    += sum_in_a;

				if (++sp >= div) 
					sp = 0;

				stack_ptr = &stack[sp * sizeof(clim_pixelcolor_t)];

				sum_out_r += stack_ptr[0];
				sum_out_g += stack_ptr[1];
				sum_out_b += stack_ptr[2];
				sum_out_a += stack_ptr[3];
				sum_in_r  -= stack_ptr[0];
				sum_in_g  -= stack_ptr[1];
				sum_in_b  -= stack_ptr[2];
				sum_in_a  -= stack_ptr[3];
			}
		}
	}
}

bool clim_stack_blur(uint8_t* pixels, uint32_t width, uint32_t height, 
					 uint8_t radius, uint8_t ncores)
{
	if (radius == UINT8_MAX || radius < UINT8_C(2))
		return false;

	uint16_t div = (uint16_t)((radius << 1) + 1);
	uint8_t* stack = (uint8_t *) clim_mem_alloc((div * sizeof(clim_pixelcolor_t)) * ncores, true);

	// no multithreading
	if (ncores == 1)
	{
		clim_stack_blur_job(pixels, width, height, radius, 1, 0, 1, stack);
		clim_stack_blur_job(pixels, width, height, radius, 1, 0, 2, stack);
	}
	/*
	else
	{
		// TODO(Garcia): Implementing Multi-Threading Here

		StackBlurTask** workers = new StackBlurTask * [ncores];

		for (int i = 0; i < ncores; i++)
		{
			workers[i] = new StackBlurTask(pixels, width, height, radius, ncores, i, 1, stack + div * 4 * i);
			workers[i]->Start();
		}

		for (int i = 0; i < ncores; i++)
			workers[i]->Wait();

		for (int i = 0; i < ncores; i++)
		{
			workers[i]->step = 2;
			workers[i]->Start();
		}

		for (int i = 0; i < ncores; i++)
		{
			workers[i]->Wait();
			delete workers[i];
		}

		delete[] workers;
		delete[] stack;

		
	}*/

	clim_mem_release(stack);
	return true;
}

#endif
