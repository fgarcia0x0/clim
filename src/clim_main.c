#include <stdio.h>
#include <stdlib.h>

#include "../include/clim_base.h"
#include "../include/clim_img.h"
#include "../include/clim_logger.h"
#include "../include/clim_utils.h"
#include "../include/clim_filesystem.h"

static inline void clim_init()
{
	#ifdef CLIM_OS_WIN
		CLIM_DO_IF(
			!SetConsoleOutputCP(CLIM_WIN_UTF8_CODEPAGE),
			CLIM_LOG_WARN("Fail to configure console codepage to utf8")
		);
	#endif

	#ifdef CLIM_DEBUG_MODE
		CLIM_LOG_DEBUG("Running in Debug Mode");
	#endif

	CLIM_LOG_DEBUG("clim has been initialized");
}

int main(int argc, char* argv[])
{
	clim_init();
}
