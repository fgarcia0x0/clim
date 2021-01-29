cmake_minimum_required(VERSION 3.10)

# set c version
set(CMAKE_C_STANDARD 11)

# set the project name
project(clim)

# add the executable
add_executable(clim src/clim_img.c src/clim_utils.c src/clim_logger.c src/clim_main.c src/clim_mem.c)