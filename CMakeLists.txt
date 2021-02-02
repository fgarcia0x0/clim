cmake_minimum_required(VERSION 3.10)

# set c version
set(CMAKE_C_STANDARD 11)

# set cross-compiling
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CROSSCOMPILING 1)

# set additional compiler flags
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Werror -Wpedantic -Wdouble-promotion -Wformat-overflow -Wformat-security -Wnull-dereference -Wmisleading-indentation -Warith-conversion -Wconversion -Wshadow -funsafe-loop-optimizations -Wunsafe-loop-optimizations -Wcast-qual -Wcast-align -Waddress -fanalyzer)
endif()

# set the project name
project(clim)

# add the executable
add_executable(clim src/clim_img.c src/clim_utils.c src/clim_logger.c src/clim_main.c src/clim_mem.c)