cmake_minimum_required(VERSION 3.10)

# set the project name
project(clim)

set(CMAKE_C_STANDARD 11)

# add the executable
add_executable(clim src/clim_img.c src/clim_utils.c src/clim_logger.c src/clim_main.c src/clim_mem.c)