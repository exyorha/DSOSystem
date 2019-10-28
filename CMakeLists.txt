cmake_minimum_required(VERSION 3.5.1)
project(DSOSystem)

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL arm)
	enable_language(ASM)
endif()

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

include(CMake/Resources.cmake)

if (MSVC)
	set(CMAKE_C_FLAGS_MINSIZEREL "/MT /GL /Ox /Os /DNDEBUG /MP")
	set(CMAKE_C_FLAGS_RELEASE "/MT /GL /Ox /Ot /DNDEBUG /MP")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MT /Zi /GL /Ox /Ot /DNDEBUG /Zc:inline")
	set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /Zc:inline")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /GL /Ox /Os /DNDEBUG /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "/MT /GL /Ox /Ot /DNDEBUG /MP")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /GL /Ox /Ot /DNDEBUG /MP /Zc:inline")
	set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /Zc:inline")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG:FASTLINK" CACHE STRING "Flags used by the linker during debug builds." FORCE)
	set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/LTCG:incremental /INCREMENTAL:NO /CGTHREADS:8" CACHE STRING "Flags used by the linker during release minsize builds." FORCE)
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG:incremental /INCREMENTAL:NO /CGTHREADS:8" CACHE STRING "Flags used by the linker during release builds." FORCE)
	set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/DEBUG:FASTLINK /LTCG:incremental /INCREMENTAL:NO /CGTHREADS:8" CACHE STRING "Flags used by the linker during Release with Debug Info builds." FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/DEBUG:FASTLINK" CACHE STRING "Flags used by the linker during debug builds." FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "/LTCG:INCREMENTAL /INCREMENTAL:NO /CGTHREADS:8" CACHE STRING "Flags used by the linker during release minsize builds." FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/LTCG:INCREMENTAL /INCREMENTAL:NO /CGTHREADS:8" CACHE STRING "Flags used by the linker during release builds." FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "/DEBUG:FASTLINK /LTCG:INCREMENTAL /INCREMENTAL:NO /CGTHREADS:8" CACHE STRING "Flags used by the linker during Release with Debug Info builds." FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/DEBUG:FASTLINK" CACHE STRING "Flags used by the linker during debug builds." FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "/DEBUG:FASTLINK /LTCG:incremental /INCREMENTAL:NO /CGTHREADS:8" CACHE STRING "Flags used by the linker during release minsize builds." FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "/DEBUG:FASTLINK /LTCG:incremental /INCREMENTAL:NO /CGTHREADS:8" CACHE STRING "Flags used by the linker during release builds." FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "/DEBUG:FASTLINK /LTCG:incremental /INCREMENTAL:NO /CGTHREADS:8" CACHE STRING "Flags used by the linker during Release with Debug Info builds." FORCE)
endif()

if(WIN32)
  add_definitions(-D_UNICODE -DUNICODE)
endif()

if(UNIX)
    add_compile_options(-pthread)
endif()

add_library(nuklear INTERFACE)
target_include_directories(nuklear INTERFACE nuklear)

if(CMAKE_CROSSCOMPILING)
	set(IMPORT_HOST_TOOLS "HOSTTOOLS-NOTFOUND" CACHE FILEPATH "Path to HostTools import file")
	include(${IMPORT_HOST_TOOLS})
else()
	add_subdirectory(HostTools)
endif()

add_subdirectory(pixman)

add_subdirectory(DSO)
