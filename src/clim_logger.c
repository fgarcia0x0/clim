#include "..\include\clim_logger.h"

static const char* const levels_str[] =
{
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"CRITICAL"
};

static char g_log_filepath[CLIM_MAX_OS_PATH] = {0};
static FILE* g_file_handle = {0};

#define GET_LEVEL_STR(lvl) (levels_str[(lvl)])

#define GET_TIME_STR(input, format) 	\
	do 									\
	{									\
		time_t t = time(NULL);			\
		size_t n = strftime(input, sizeof(input), format, localtime(&t));\
		assert(n > 0);					\
	} while (0)

static void clim_build_filepath(const char* filename, const char* extension);

CLIM_API void clim_log(
	FILE* const stream, clim_log_level level, const char* file, 
	unsigned line, const char* fmt, ...
) 
{
	va_list list;
	va_start(list, fmt);

	char time_str[16] = {0};
	GET_TIME_STR(time_str, "%H:%M:%S");

	const char* start_color = NULL, *end_color = NULL;
	switch (level)
	{
		case CLIM_LOG_LEVEL_DEBUG:
			start_color = end_color = CLIM_CONSOLE_FG_COLOR_WHITE;
			break;
		case CLIM_LOG_LEVEL_INFO:
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
	const char* filename = strrchr(file, '\\');

	if (filename == NULL)
		filename = strrchr(file, '/');

	assert(filename);
	filename++;

	fprintf(stream, "[%s][%s%s%s] %s:%u: ",
			time_str, start_color, level_str, 
			end_color, filename, line);

	fprintf(stream, "%s", start_color);
	vfprintf(stream, fmt, list);
	fprintf(stream, "%s", end_color);

	fputc('\n', stream);
  	fflush(stream);

  	// write info in file handle
	if (g_file_handle)
	{
		fprintf(g_file_handle, "[%s][%s] %s:%u: ", 
				time_str, level_str,
				filename, line);

		vfprintf(g_file_handle, fmt, list);
		fputc('\n', g_file_handle);
  		fflush(g_file_handle);
	}

	va_end(list);
}

static void clim_build_filepath(const char* filename, const char* extension)
{
	char time_str[64] = {0};
	const char* const str_format = "%s_%s_%s";

	GET_TIME_STR(time_str, "%m-%d-%Y_%H-%M-%S");
	int len = snprintf(0, 0, "%s_%s_%s", filename, time_str, extension);

	assert(len > 0);
	assert(len < CLIM_MAX_OS_PATH);

	len = snprintf(g_log_filepath, (size_t)len + 1U, str_format,
				   filename, time_str, extension);

	assert(len > 0);
}

CLIM_API clim_err_code_t clim_log_init(const char* filepath, const char* extension)
{
	if (!filepath || !*filepath)
		return CLIM_EC_INVALID_PARAMETERS;

	clim_build_filepath(filepath, extension);

	assert(g_file_handle == NULL);
	g_file_handle = fopen(g_log_filepath, "a+");

	if (!g_file_handle)
		return CLIM_EC_CANNOT_OPEN_FILE;

	return CLIM_EC_SUCCESS;
}

CLIM_API clim_err_code_t clim_log_close()
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

	memset(g_log_filepath, 0, CLIM_MAX_OS_PATH);
	return CLIM_EC_SUCCESS;
}