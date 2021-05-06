#include "clim_test_filesystem.h"

int main(int argc, char** argv)
{
	CLIM_UNREF_PARAM(argc);
	CLIM_UNREF_PARAM(argv);

	fs_init_tests();

	return EXIT_SUCCESS;
}
