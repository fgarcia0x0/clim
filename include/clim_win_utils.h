#ifndef CLIM_WIN_UTILS_H
#define CLIM_WIN_UTILS_H

#include <windows.h>
#include <stdbool.h>

#include "clim_platform_detector.h"
#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

static bool clim_win_enable_utf8_console_output(void)
{
	return SetConsoleOutputCP(CP_UTF8);
}

static bool clim_win_enable_virtual_terminal_sequences(void)
{
	HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (output_handle == INVALID_HANDLE_VALUE)
		return false;

    DWORD original_output_mode = 0;

    if (!GetConsoleMode(output_handle, &original_output_mode))
		return false;

    DWORD modes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    DWORD curr_output_mode = original_output_mode | modes;
	
    if (!SetConsoleMode(output_handle, curr_output_mode))
    {
        modes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        curr_output_mode = original_output_mode | modes;

        if (!SetConsoleMode(output_handle, curr_output_mode))
			return false;
    }

	return true;
}

#endif // CLIM_WIN_UTILS_H