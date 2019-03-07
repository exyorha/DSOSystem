include(CheckTypeSize)
include(CheckIncludeFile)
include(CheckFunctionExists)

add_library(pixman STATIC
	pixman/pixman/pixman.c
	pixman/pixman/pixman-access.c
	pixman/pixman/pixman-access-accessors.c
	pixman/pixman/pixman-bits-image.c
	pixman/pixman/pixman-combine32.c
	pixman/pixman/pixman-combine-float.c
	pixman/pixman/pixman-conical-gradient.c
	pixman/pixman/pixman-filter.c
	pixman/pixman/pixman-x86.c
	pixman/pixman/pixman-mips.c
	pixman/pixman/pixman-arm.c
	pixman/pixman/pixman-ppc.c
	pixman/pixman/pixman-edge.c
	pixman/pixman/pixman-edge-accessors.c
	pixman/pixman/pixman-fast-path.c
	pixman/pixman/pixman-glyph.c
	pixman/pixman/pixman-general.c
	pixman/pixman/pixman-gradient-walker.c
	pixman/pixman/pixman-image.c	
	pixman/pixman/pixman-implementation.c
	pixman/pixman/pixman-linear-gradient.c
	pixman/pixman/pixman-matrix.c
	pixman/pixman/pixman-noop.c
	pixman/pixman/pixman-radial-gradient.c
	pixman/pixman/pixman-region16.c
	pixman/pixman/pixman-region32.c
	pixman/pixman/pixman-solid-fill.c
	pixman/pixman/pixman-timer.c
	pixman/pixman/pixman-trap.c
	pixman/pixman/pixman-utils.c
	pixman/pixman/pixman.h
	pixman/pixman/pixman-accessor.h
	pixman/pixman/pixman-combine32.h
	pixman/pixman/pixman-compiler.h
	pixman/pixman/pixman-edge-imp.h
	pixman/pixman/pixman-inlines.h
	pixman/pixman/pixman-private.h
	${CMAKE_CURRENT_BINARY_DIR}/config.h
	${CMAKE_CURRENT_BINARY_DIR}/public/pixman-version.h
)

set(PIXMAN_VERSION_MAJOR 0)
set(PIXMAN_VERSION_MINOR 38)
set(PIXMAN_VERSION_MICRO 0)


if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	set(USE_GCC_INLINE_ASM TRUE)
	set(TOOLCHAIN_SUPPORTS_ATTRIBUTE_CONSTRUCTOR TRUE)
	set(HAVE_BUILTIN_CLZ TRUE)

	target_compile_options(pixman PRIVATE -Wno-unknown-attributes -Wno-shift-negative-value)
else()
	set(USE_GCC_INLINE_ASM FALSE)
	set(TOOLCHAIN_SUPPORTS_ATTRIBUTE_CONSTRUCTOR FALSE)
	set(HAVE_BUILTIN_CLZ FALSE)
endif()

# MIPS
set(USE_MIPS_DSPR2 FALSE)
set(USE_LOONGSON_MMI FALSE)
# PPC
set(USE_VMX FALSE)

# Set if Intel
set(USE_X86_MMX FALSE)
set(USE_SSE2 FALSE)
set(USE_SSSE3 FALSE)

# Set if ARM
set(USE_ARM_SIMD FALSE)
set(USE_ARM_NEON FALSE)
set(USE_ARM_IWMMXT FALSE)

message("Compiling pixman for ${CMAKE_SYSTEM_PROCESSOR}")

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL i386 OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL AMD64 OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
	set(USE_X86_MMX TRUE)
	set(USE_SSE2 TRUE)
	set(USE_SSSE3 TRUE)

	target_sources(pixman PRIVATE 
		pixman/pixman/pixman-mmx.c
		pixman/pixman/pixman-sse2.c
		pixman/pixman/pixman-ssse3.c
	)
endif()

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL arm)
	set(USE_ARM_SIMD TRUE)
	set(USE_ARM_NEON TRUE)

	target_sources(pixman PRIVATE
		pixman/pixman/pixman-arm-simd.c
		pixman/pixman/pixman-arm-common.h
		pixman/pixman/pixman-arm-simd-asm.S
		pixman/pixman/pixman-arm-simd-asm-scaled.S
		pixman/pixman/pixman-arm-asm.h
		pixman/pixman/pixman-arm-simd-asm.h
		pixman/pixman/pixman-arm-neon.c
        pixman/pixman/pixman-arm-common.h
        pixman/pixman/pixman-arm-neon-asm.S
		pixman/pixman/pixman-arm-neon-asm-bilinear.S
        pixman/pixman/pixman-arm-asm.h
        pixman/pixman/pixman-arm-neon-asm.h
	)

	target_compile_options(pixman PRIVATE
		$<$<COMPILE_LANGUAGE:ASM>:-no-integrated-as>
	)
endif()

check_type_size("long" SIZEOF_LONG BUILTIN_TYPES_ONLY)
check_include_file("pthread.h" HAVE_PTHREADS)

set(WORDS_BIGENDIAN FALSE)

configure_file(config.h ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
configure_file(pixman/pixman/pixman-version.h.in ${CMAKE_CURRENT_BINARY_DIR}/public/pixman-version.h @ONLY)

target_include_directories(pixman PUBLIC pixman/pixman ${CMAKE_CURRENT_BINARY_DIR}/public)
target_include_directories(pixman PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(pixman PRIVATE -DHAVE_CONFIG_H)

if(MSVC)
	target_compile_options(pixman PRIVATE /wd4244 /wd4146 /wd4098)
	target_compile_definitions(pixman PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
