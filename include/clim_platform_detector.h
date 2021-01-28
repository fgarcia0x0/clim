#ifndef CLIM_PLATFORM_DETECTOR_H
#define CLIM_PLATFORM_DETECTOR_H

#ifdef _MSC_VER
	#pragma once
#endif

#ifdef ANDROID
	#define CLIM_OS_ANDROID 1
#elif defined(__APPLE__)
	#include <TargetConditionals.h>
	#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
		#define CLIM_OS_IOS 1
	#else
		#define CLIM_OS_MAC 1
	#endif
#elif defined(__linux__)
	#define CLIM_OS_LINUX 1
	#include <unistd.h>
	#if defined(__GLIBC__) && !defined(__UCLIBC__)
		#define CLIM_LIBC_GLIBC 1
	#endif
#elif defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
	#define CLIM_OS_WIN 1
#else
	#error Please add support for your platform
#endif

#if defined(OS_MAC) || defined(OS_IOS)
	#define CLIM_OS_APPLE 1
#endif

#if defined(__GNUC__) && !defined(__clang__)
	#define CLIM_COMPILER_GCC 1
#elif defined(_MSC_VER)
	#define CLIM_COMPILER_MSVC 1
#elif defined(__GNUC__) && defined(__clang__)
	#define CLIM_COMPILER_CLANG 1
#else
	#error Please add support for your compiler
#endif

#ifdef _GNU_SOURCE
	#define CLIM_GNU_EXT 1
#endif

#if defined(_M_X64) || defined(__x86_64__)
	#define CLIM_ARCH_CPU_X86_FAMILY 1
	#define CLIM_ARCH_CPU_X86_64 1
	#define CLIM_ARCH_CPU_64_BITS 1
	#define CLIM_ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
	#define CLIM_ARCH_CPU_X86_FAMILY 1
	#define CLIM_ARCH_CPU_X86 1
	#define CLIM_ARCH_CPU_32_BITS 1
	#define CLIM_ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__ARMEL__)
	#define CLIM_ARCH_CPU_ARM_FAMILY 1
	#define CLIM_ARCH_CPU_ARMEL 1
	#define CLIM_ARCH_CPU_32_BITS 1
	#define CLIM_ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__aarch64__) || defined(_M_ARM64)
	#define CLIM_ARCH_CPU_ARM_FAMILY 1
	#define CLIM_ARCH_CPU_ARM64 1
	#define CLIM_ARCH_CPU_64_BITS 1
	#define CLIM_ARCH_CPU_LITTLE_ENDIAN 1
#else
	#error Please add support for your architecture
#endif

#endif