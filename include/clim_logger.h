#ifndef CLIM_LOGGER_H
#define CLIM_LOGGER_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "clim_base.h"

#ifdef CLIM_OS_WIN
	#define CLIM_BACKSLASH_SYMBOL '\\'
#else
	#define CLIM_BACKSLASH_SYMBOL '/'
#endif

#define CLIM_CONSOLE_FG_COLOR_BLACK "\033[30m"
#define CLIM_CONSOLE_FG_COLOR_RED "\033[31m"
#define CLIM_CONSOLE_FG_COLOR_GREEN "\033[32m"
#define CLIM_CONSOLE_FG_COLOR_YELLOW "\033[33m"
#define CLIM_CONSOLE_FG_COLOR_BLUE "\033[34m"
#define CLIM_CONSOLE_FG_COLOR_MAGENTA "\033[35m"
#define CLIM_CONSOLE_FG_COLOR_CYAN "\033[36m"
#define CLIM_CONSOLE_FG_COLOR_WHITE "\033[37m"

#define CLIM_CONSOLE_BG_COLOR_BLACK "\033[40m"
#define CLIM_CONSOLE_BG_COLOR_RED "\033[41m"
#define CLIM_CONSOLE_BG_COLOR_GREEN "\033[42m"
#define CLIM_CONSOLE_BG_COLOR_YELLOW "\033[43m"
#define CLIM_CONSOLE_BG_COLOR_BLUE "\033[44m"
#define CLIM_CONSOLE_BG_COLOR_MAGENTA "\033[45m"
#define CLIM_CONSOLE_BG_COLOR_CYAN "\033[46m"
#define CLIM_CONSOLE_BG_COLOR_WHITE "\033[47m"

typedef enum
{
	CLIM_LOG_LEVEL_DEBUG = 0,
	CLIM_LOG_LEVEL_INFO,
	CLIM_LOG_LEVEL_WARN,
	CLIM_LOG_LEVEL_ERROR,
	CLIM_LOG_LEVEL_CRITICAL
} clim_log_level;

CLIM_API void clim_log(
	FILE* const stream, clim_log_level level, 
	const char* file, unsigned line, const char* fmt, ...
);

CLIM_API clim_err_code_t clim_log_init(const char* filepath, const char* extension);
CLIM_API clim_err_code_t clim_log_close();

#ifdef _DEBUG
	#define CLIM_LOG_DEBUG(...) clim_log(stdout, CLIM_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_INFO(...) clim_log(stdout, CLIM_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_WARN(...) clim_log(stderr, CLIM_LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_ERROR(...) clim_log(stderr, CLIM_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_CRITICAL(...) clim_log(stderr, CLIM_LOG_LEVEL_CRITICAL, __FILE__, __LINE__, __VA_ARGS__)
#else
	#define CLIM_LOG_DEBUG(...)
	#define CLIM_LOG_INFO(...) clim_log(stdout, CLIM_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_WARN(...) clim_log(stderr, CLIM_LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_ERROR(...) clim_log(stderr, CLIM_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_CRITICAL(...) clim_log(stderr, CLIM_LOG_LEVEL_CRITICAL, __FILE__, __LINE__, __VA_ARGS__)
#endif

#endif