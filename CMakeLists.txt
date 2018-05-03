cmake_minimum_required(VERSION 3.5.1)
project(DSOSystem)

set(CMAKE_CXX_STANDARD 17)

add_library(clp INTERFACE)
add_library(skia INTERFACE)

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

if(UNIX)
	add_library(libdrm INTERFACE)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(CLP clp REQUIRED)
	pkg_check_modules(LIBDRM libdrm REQUIRED)
	pkg_check_modules(SKIA skia REQUIRED)
	target_include_directories(clp INTERFACE ${CLP_INCLUDE_DIRS})
	target_link_libraries(clp INTERFACE ${CLP_LIBRARIES})
	target_include_directories(libdrm INTERFACE ${LIBDRM_INCLUDE_DIRS})
	target_link_libraries(libdrm INTERFACE ${LIBDRM_LIBRARIES})
	target_include_directories(skia INTERFACE ${SKIA_INCLUDE_DIRS}/core ${SKIA_INCLUDE_DIRS}/gpu)
	target_link_libraries(skia INTERFACE ${SKIA_LDFLAGS} -lfreetype -Wl,-rpath-link,${SKIA_LIBRARY_DIRS})
endif()

if(WIN32)
	target_include_directories(skia INTERFACE ${PROJECT_SOURCE_DIR}/../skia/include/core ${PROJECT_SOURCE_DIR}/../skia/include/gpu)
	target_link_libraries(skia INTERFACE ${PROJECT_SOURCE_DIR}/../skia/out/Static/skia.lib)
	target_include_directories(clp INTERFACE
		${PROJECT_SOURCE_DIR}/../Clp-1.16.11/Clp/src
		${PROJECT_SOURCE_DIR}/../Clp-1.16.11/CoinUtils/src
	)
	target_link_libraries(clp INTERFACE
		${PROJECT_SOURCE_DIR}/../Clp-1.16.11/Clp/MSVisualStudio/v10/x64-v141-Release/libClp.lib
		${PROJECT_SOURCE_DIR}/../Clp-1.16.11/Clp/MSVisualStudio/v10/x64-v141-Release/libCoinUtils.lib
	)
	
    add_definitions(-D_UNICODE -DUNICODE)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set_directory_properties(PROPERTIES COMPILE_OPTIONS -Wno-psabi)
endif()

if(UNIX)
	add_subdirectory(initramfs_init)
	add_subdirectory(init)
	add_subdirectory(system_support)
endif()

add_subdirectory(DSO)
