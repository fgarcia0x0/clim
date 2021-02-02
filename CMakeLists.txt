cmake_minimum_required(VERSION 3.10)

# set cross-compiling
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CROSSCOMPILING 1)

# set ext
if (WIN32)
	message(STATUS "defining the executable file suffix")
	set(CMAKE_EXECUTABLE_SUFFIX_C .exe)
endif()

# set the project name
project(clim)

# set additional compiler flags
if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
  if (CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  	message(STATUS "Compiling with CLANG_CL")
    set(CMAKE_COMPILER_IS_CLANG_CL true)
  elseif (CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
  	message(STATUS "Compiling with CLANG")
    set(CMAKE_COMPILER_IS_CLANG true)
  endif()
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	message(STATUS "Compiling with GCC")
    set(CMAKE_COMPILER_IS_GCC true)
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Compiling with MSVC")
    set(CMAKE_COMPILER_IS_MSVC true)
endif()

if (CMAKE_COMPILER_IS_MSVC)
	message(STATUS "Generating MSVC Additional Arguments")
    add_compile_options(/W4 /MP)
elseif (CMAKE_COMPILER_IS_GCC)
	message(STATUS "Generating GCC Additional Arguments")

    add_compile_options(-Wall -Wextra -Werror -Wpedantic -Wdouble-promotion -Wformat-overflow -Wformat-security -Wnull-dereference -Wmisleading-indentation -Wconversion -Wshadow -funsafe-loop-optimizations -Wunsafe-loop-optimizations -Wcast-qual -Wcast-align -Waddress)

    if (${CMAKE_C_COMPILER_VERSION} VERSION_GREATER_EQUAL 10.0)
    	add_compile_options(-Warith-conversion -fanalyzer)
    endif()

elseif (CMAKE_COMPILER_IS_CLANG)
	 message(STATUS "Generating Clang Additional Arguments")
     add_compile_options(-Wall -Wextra -Werror -Wpedantic)
endif()

# add the executable
add_executable(clim src/clim_img.c src/clim_utils.c src/clim_logger.c src/clim_main.c src/clim_mem.c)

# set c version
set_property(TARGET clim PROPERTY C_STANDARD 11)