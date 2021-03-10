#ifndef CLIM_TEST_BASE_H
#define CLIM_TEST_BASE_H

#include "../include/clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uchar.h>

#define DEFAULT_IO_HANDLE stdout

#define INIT_MODULE(msg) MESSAGE("\n\r\t\t[" msg " MODULE TESTS]\n\n");
#define MESSAGE(...) fprintf(DEFAULT_IO_HANDLE, __VA_ARGS__)
#define GLUE(x) #x

#define MODULE_ALIAS ""

#define REQUIRE(cond)															 \
	do																			 \
	{																			 \
		if (!(cond))															 \
		{																		 \
			fprintf(DEFAULT_IO_HANDLE,											 \
				    "[" MODULE_ALIAS " - FAILED - %d] The condition \"%s\" was not satisfied\n", \
					__LINE__, GLUE(cond));										 \
			exit(EXIT_FAILURE);													 \
		}																		 \
	} while (0)

#endif
