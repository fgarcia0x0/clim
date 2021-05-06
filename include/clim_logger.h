#ifndef CLIM_LOGGER_H
#define CLIM_LOGGER_H

#include "clim_platform_detector.h"

#ifdef CLIM_COMPILER_MSVC
	#pragma once
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "clim_base.h"
#include "clim_err_handle.h"
#include "clim_filesystem_types.h"


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

static const char* const levels_str[] =
{
	"DEBUG",
	"TRACE",
	"INFO",
	"WARN",
	"ERROR",
	"CRITICAL"
};

typedef enum
{
	CLIM_LOG_LEVEL_DEBUG = 0,
	CLIM_LOG_LEVEL_TRACE,
	CLIM_LOG_LEVEL_INFO,
	CLIM_LOG_LEVEL_WARN,
	CLIM_LOG_LEVEL_ERROR,
	CLIM_LOG_LEVEL_CRITICAL
} clim_log_level;

// TODO (Garcia): needed a mutex to run in different threads
static CLIM_THREAD_LOCAL char g_log_filepath[CLIM_MAX_OS_PATH] = {0};
static CLIM_THREAD_LOCAL FILE* g_file_handle = NULL;

#define GET_LEVEL_STR(lvl) (levels_str[(lvl)])

#define GET_TIME_STR(input, format) 	\
	do 									\
	{									\
		time_t t = time(NULL);			\
		size_t n = strftime(input, sizeof(input), format, localtime(&t));\
		CLIM_ASSERT(n > 0);					\
		(void)n; 							\
	} while (0)

#ifdef CLIM_DEBUG_MODE
	#define CLIM_LOG_DEBUG(...) clim_log_write(stdout, CLIM_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_TRACE(...) clim_log_write(stdout, CLIM_LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_INFO(...) clim_log_write(stdout, CLIM_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_WARN(...) clim_log_write(stderr, CLIM_LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_ERROR(...) clim_log_write(stderr, CLIM_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
	#define CLIM_LOG_CRITICAL(...) clim_log_write(stderr, CLIM_LOG_LEVEL_CRITICAL, __FILE__, __LINE__, __VA_ARGS__)
#else
	#define CLIM_LOG_DEBUG(...)
	#define CLIM_LOG_TRACE(...) clim_log_write(stdout, CLIM_LOG_LEVEL_TRACE, NULL, 0, __VA_ARGS__)
	#define CLIM_LOG_INFO(...) clim_log_write(stdout, CLIM_LOG_LEVEL_INFO, NULL, 0, __VA_ARGS__)
	#define CLIM_LOG_WARN(...) clim_log_write(stderr, CLIM_LOG_LEVEL_WARN, NULL, 0, __VA_ARGS__)
	#define CLIM_LOG_ERROR(...) clim_log_write(stderr, CLIM_LOG_LEVEL_ERROR, NULL, 0, __VA_ARGS__)
	#define CLIM_LOG_CRITICAL(...) clim_log_write(stderr, CLIM_LOG_LEVEL_CRITICAL, NULL, 0, __VA_ARGS__)
#endif

#define CLIM_PANIC(msg, ...) clim_panic((msg), __VA_ARGS__)

#define CLIM_PANIC_IF(cond, ...) 				\
	do												\
	{												\
		if (cond) clim_panic(__VA_ARGS__);	\
	} while(0)


static inline void clim_build_filepath(
	const char* filename, const char* extension
)
{
	char time_str[64] = {0};
	const char* const str_format = "%s_%s_%s";

	GET_TIME_STR(time_str, "%m-%d-%Y_%H-%M-%S");
	int len = snprintf(0, 0, "%s_%s_%s", filename, time_str, extension);

	CLIM_ASSERT(len > 0);
	CLIM_ASSERT(len < CLIM_MAX_OS_PATH);

	len = snprintf(g_log_filepath, (size_t)len + 1U, str_format,
				   filename, time_str, extension);

	CLIM_ASSERT(len > 0);
}

static void clim_log_write(
	FILE* const stream, clim_log_level level, const char* file, 
	unsigned line, const char* fmt, ...
) 
{
	va_list list;
	va_start(list, fmt);

	bool have_extra_info = file && *file;

	char time_str[16] = {0};
	GET_TIME_STR(time_str, "%H:%M:%S");

	const char* start_color = "", *end_color = "";
	switch (level)
	{
		case CLIM_LOG_LEVEL_DEBUG:
			start_color = end_color = CLIM_CONSOLE_FG_COLOR_WHITE;
			break;
		case CLIM_LOG_LEVEL_INFO:
		case CLIM_LOG_LEVEL_TRACE:
			start_color = CLIM_CONSOLE_FG_COLOR_GREEN;
			end_color = CLIM_CONSOLE_FG_COLOR_WHITE;
			break;
		case CLIM_LOG_LEVEL_WARN:
			start_color = CLIM_CONSOLE_FG_COLOR_YELLOW;
			end_color = CLIM_CONSOLE_FG_COLOR_WHITE;
			break;
		case CLIM_LOG_LEVEL_ERROR:
		case CLIM_LOG_LEVEL_CRITICAL:
			start_color = CLIM_CONSOLE_FG_COLOR_RED;
			end_color = CLIM_CONSOLE_FG_COLOR_WHITE;
			break;
		default:
			break;
	}

	const char* level_str = GET_LEVEL_STR((int)level);
	const char* filename = NULL;
	
	fprintf(stream, "[%s][%s%s%s] ",
				   time_str, start_color, 
				   level_str, end_color);
				
	if (have_extra_info)
	{
		filename = strrchr(file, CLIM_PATH_SEPERATOR);

		// BUG BUG
		if (!filename)
			filename = strrchr(file, '/');

		CLIM_ASSERT(filename);
		filename++;

		if (filename)
			fprintf(stream, "%s:%u: ", filename, line);
	}

	fprintf(stream, "%s", start_color);
	vfprintf(stream, fmt, list);
	fprintf(stream, "%s", end_color);

	fputc('\n', stream);
	fflush(stream);

  	// write info in file handle
	if (g_file_handle)
	{
		fprintf(g_file_handle, "[%s][%s] ", time_str, level_str);

		if (have_extra_info)
			fprintf(g_file_handle, "%s:%u: ", filename, line);

		vfprintf(g_file_handle, fmt, list);
		fputc('\n', g_file_handle);
		fflush(g_file_handle);
	}

	va_end(list);
}

static inline clim_errcode_t clim_log_init(const char* filepath, const char* extension)
{
	if (!filepath || !*filepath || !extension)
		return CLIM_EC_INVALID_PARAMETERS;

	clim_build_filepath(filepath, extension);

	CLIM_ASSERT(g_file_handle == NULL);
	if (g_file_handle)
	{
		CLIM_LOG_ERROR("clim_log_init was already initialized, release previous instance");
		return CLIM_EC_WAS_ALREADY_INIT;
	}

	g_file_handle = fopen(g_log_filepath, "a+");

	if (!g_file_handle)
		return CLIM_EC_CANNOT_OPEN_FILE;

	return CLIM_EC_SUCCESS;
}

static inline clim_errcode_t clim_log_close()
{
	if (g_file_handle)
	{
		if (fclose(g_file_handle) == EOF)
		{
			CLIM_LOG_CRITICAL("Error to close handle of log file.");
			return CLIM_EC_CANNOT_CLOSE_FILE;
		}

		g_file_handle = NULL;
	}

	memset(g_log_filepath, 0, sizeof g_log_filepath);
	return CLIM_EC_SUCCESS;
}

static CLIM_NORETURN void clim_panic(const char* fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	fprintf(stderr, "[CLIM_PANIC] ");
	vfprintf(stderr, fmt, list);
	fputc('\n', stderr);
	va_end(list);
	exit(EXIT_FAILURE);
}

#endif
