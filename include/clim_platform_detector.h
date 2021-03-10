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

#if CLIM_COMPILER_MSVC
	#if _MSC_VER < 1920
		#error This project needs MSVC 14.2 or greater
	#endif
#else
	#if (__STDC_VERSION__ < 201112L)
		#error This program needs at least a C11 compliant compiler
	#endif
#endif

#ifdef CLIM_COMPILER_MSVC
	#pragma once
	#define _CRT_SECURE_NO_WARNINGS
	#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#endif

#define CLIM_MAX_OS_PATH FILENAME_MAX

#ifdef CLIM_COMPILER_MSVC
	#ifndef __func__
		#define __func__ __FUNCTION__
	#endif
	#define restrict __restrict
#endif

#if defined(CLIM_GNU_EXT)
	#define CLIM_HAVE_MEMMEM
#endif

#ifndef NDEBUG
	#define CLIM_DEBUG_MODE
#else
	#define CLIM_RELEASE_MODE
#endif

#ifdef CLIM_COMPILER_MSVC
	#define CLIM_NORETURN __declspec(noreturn)
#else
	#define CLIM_NORETURN noreturn
#endif

#ifdef CLIM_COMPILER_MSVC
	#define CLIM_DLL_EXPORT extern "C" __declspec(dllexport)
	#define CLIM_DLL_IMPORT	extern "C" __declspec(import)
#else
	// -fvisibility=hidden
	#define CLIM_DLL_EXPORT	extern "C"
	#define CLIM_DLL_IMPORT
#endif

#ifdef CLIM_DYNAMIC_COMPILATION
	#define CLIM_API CLIM_DLL_EXPORT
#else
	#define CLIM_API CLIM_DLL_IMPORT
#endif

#if defined(CLIM_COMPILER_GCC) || defined(CLIM_COMPILER_CLANG)
	#define CLIM_LIKELY(cond) 	__builtin_expect(!!(cond), 1)
	#define CLIM_UNLIKELY(cond) __builtin_expect(!!(cond), 0)
#else
	#define CLIM_LIKELY(cond) 	(!!(cond))
	#define CLIM_UNLIKELY(cond) (!!(cond))
#endif

#ifndef CLIM_THREAD_LOCAL
	#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
		#define CLIM_THREAD_LOCAL _Thread_local
	#elif defined(CLIM_COMPILER_MSVC) || defined(CLIM_COMPILER_CLANG_CL)
		#define CLIM_THREAD_LOCAL __declspec(thread) 
	#elif defined CLIM_COMPILER_GCC || defined(CLIM_COMPILER_CLANG)
		#define CLIM_THREAD_LOCAL __thread
	#else
		#error "Cannot define CLIM_THREAD_LOCAL"
	#endif
#endif

#if defined(CLIM_OS_WIN) && defined(CLIM_COMPILER_CLANG)
	#undef CLIM_THREAD_LOCAL
	#define CLIM_THREAD_LOCAL
#endif

#if defined(CLIM_COMPILER_GCC) || defined(CLIM_COMPILER_CLANG)
	#define CLIM_ALWAYS_INLINE inline __attribute__((always_inline))
#else
	#define CLIM_ALWAYS_INLINE inline
#endif

#endif
