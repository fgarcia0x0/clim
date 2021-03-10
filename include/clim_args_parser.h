#ifndef CLIM_ARGS_PARSER_H
#define CLIM_ARGS_PARSER_H

#include "clim_base.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

typedef int(*clim_compare_string_predicate)(const char* s1, const char* s2);

static bool clim_args_has_option(char** args, const size_t args_size,
								 const char** opt_list, const size_t opt_list_size,
						         char*** ppprest, clim_compare_string_predicate compare_fn)
	
{
	if (!args || !args_size)
		return false;

	CLIM_ASSERT(opt_list && ppprest);

	char** args_begin = args;
	char** args_end = args + args_size;

	for (; args_begin < args_end; ++args_begin)
	{
		if (!*args_begin || !**args_begin)
			break;

		const char** optlst_beg = opt_list;
		const char** optlst_end = opt_list + opt_list_size;

		for (; optlst_beg != optlst_end; ++optlst_beg)
		{
			if (!compare_fn(*args_begin, *optlst_beg))
			{
				if (ppprest) *ppprest = args_begin + 1;
				return true;
			}
		}
	}

	return false;
}

#define CLIM_ARGS_NUM(...)  (sizeof((const char *[]){__VA_ARGS__}) / sizeof(const char*))
#define CLIM_ARGS_LIST(...) (const char*[]){__VA_ARGS__}
#define CLIM_ARGS_HAS_OPT(ppprest, ...) clim_args_has_option(argv, argc, CLIM_ARGS_LIST(__VA_ARGS__), CLIM_ARGS_NUM(__VA_ARGS__), ppprest, strcmp)

#endif