#include "..\include\clim_base.h"
#include "..\include\clim_img.h"
#include "..\include\clim_logger.h"
#include "..\include\clim_utils.h"

int main()
{
	clim_log_init("clim_log", ".txt");

	CLIM_LOG_INFO("Init Commit");
	CLIM_LOG_WARN("Warn Ocurred");
	CLIM_LOG_ERROR("Fail Ocurred");

	clim_log_close();
	return 0;
}