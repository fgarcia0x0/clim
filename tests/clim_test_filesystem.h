#ifndef CLIM_TEST_FILESYSTEM_H
#define CLIM_TEST_FILESYSTEM_H

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include "clim_test_base.h"
#include "../include/clim_filesystem.h"

#define FS_SUCCESS "[FS - SUCCESS]"
#define FS_FAILED  "[FS - FAILED]"

#ifdef MODULE_ALIAS
	#undef MODULE_ALIAS
#endif

#define MODULE_ALIAS "FS"
#define STR_EQUALS(s1, s2) (!strcmp((s1), (s2)))
#define STR_EQUALS_INSENSITIVE(s1, s2) (!strcmp((s1), (s2)))

static void fs_init_tests(void)
{
	INIT_MODULE("FILESYSTEM");

	MESSAGE("[clim_get_file_extension]\n");

	const char* filepath = "C:\\users\\test_user\\desktop\\test.exe";
	char* ext = clim_extract_file_extension(filepath);

	REQUIRE(ext != NULL);
	REQUIRE(STR_EQUALS(ext, "exe"));

	REQUIRE(clim_extract_file_extension("") == NULL);
	REQUIRE(clim_extract_file_extension("c://dir.") == NULL);

	MESSAGE(FS_SUCCESS " All tests were successfully met\n");
}
#endif