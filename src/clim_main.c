#include <stdio.h>
#include <stdlib.h>

#include "../include/clim_base.h"
#include "../include/clim_img.h"
#include "../include/clim_logger.h"
#include "../include/clim_utils.h"

int main()
{
	#ifdef CLIM_DEBUG_MODE
		CLIM_LOG_DEBUG("Running in Debug Mode");
	#else
		CLIM_LOG_INFO("Running in Release Mode");
	#endif

	CLIM_LOG_INFO("clim has been initialized");
}
