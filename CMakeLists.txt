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
project(clim VERSION 0.0.2)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
	message(STATUS "Build type not specified: Use Debug by default")
endif(NOT CMAKE_BUILD_TYPE)

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

if (CMAKE_BUILD_TYPE MATCHES Release)
	message("Compiling in Release Mode")
	add_compile_options(-DNDEBUG)
endif()

if (WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (CMAKE_BUILD_TYPE MATCHES Release)
  message("Compiling in Release Mode")
  add_compile_options(-DNDEBUG)
endif()

if (CMAKE_COMPILER_IS_MSVC)
	message(STATUS "Generating MSVC Additional Arguments")
	add_compile_options(/W4 /MP /utf-8)

elseif (CMAKE_COMPILER_IS_GCC)
  message(STATUS "Generating GCC Additional Arguments")

	add_compile_options(-Wall -Wno-sign-conversion -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-function -Wdouble-promotion -Wformat-overflow -Wformat-security -Wnull-dereference -Wmisleading-indentation -Wconversion -Wshadow -funsafe-loop-optimizations -Wunsafe-loop-optimizations -Wcast-qual -Wcast-align -Waddress -D__USE_MINGW_ANSI_STDIO=1)
	if (${CMAKE_C_COMPILER_VERSION} VERSION_GREATER_EQUAL 10.0)
		add_compile_options(-fanalyzer)
	endif()

elseif (CMAKE_COMPILER_IS_CLANG OR CMAKE_COMPILER_IS_CLANG_CL)
	message(STATUS "Generating Clang Additional Arguments")
	add_compile_options(-Wall -Wextra -Werror -Wpedantic -Wno-unused-parameter -Wno-unused-function)
endif()

file(GLOB_RECURSE CLIM_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")
file(GLOB_RECURSE CLIM_TEST_SUITE_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "clim_test_main.c")

# add the executable
add_executable(clim ${CLIM_SOURCES})

# add test executable
add_executable(clim_test_suite ${CLIM_TEST_SUITE_SOURCES})

# set c version
set_target_properties(clim PROPERTIES
	C_STANDARD 11
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
)