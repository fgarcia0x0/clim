#include "..\include\clim_logger.h"

static const char* const levels_str[] =
{
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"CRITICAL"
};

#define GET_LEVEL_STR(lvl) (levels_str[(lvl)])

CLIM_API void clim_log(
	FILE* const stream, clim_log_level level, const char* file, 
	unsigned line, const char* fmt, ...) 
{
	va_list list;
	va_start(list, fmt);

	char time_str[16] = {0};
	time_t t = time(NULL);
	strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&t));

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
	const char* short_filepath = NULL;

	if (!(short_filepath = strrchr(file, '\\')))
	{
		short_filepath = strrchr(file, '/');
	}

	assert(short_filepath);
	short_filepath++;

	fprintf(stream, "[%s][%s%s%s] %s:%u: ",
			time_str, start_color, level_str, end_color,
			short_filepath, line);

	fprintf(stream, "%s", start_color);
	vfprintf(stream, fmt, list);
	fprintf(stream, "%s", end_color);

	fputc('\n', stream);
  	fflush(stream);

	va_end(list);
}