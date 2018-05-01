cmake_minimum_required(VERSION 3.5.1)
project(DSOSystem)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CLP clp REQUIRED)
pkg_check_modules(LIBDRM libdrm REQUIRED)
pkg_check_modules(SKIA skia REQUIRED)

set(CMAKE_CXX_STANDARD 11)

add_library(clp INTERFACE)
target_include_directories(clp INTERFACE ${CLP_INCLUDE_DIRS})
target_link_libraries(clp INTERFACE ${CLP_LIBRARIES})

add_library(libdrm INTERFACE)
target_include_directories(libdrm INTERFACE ${LIBDRM_INCLUDE_DIRS})
target_link_libraries(libdrm INTERFACE ${LIBDRM_LIBRARIES})

add_library(skia INTERFACE)
target_include_directories(skia INTERFACE ${SKIA_INCLUDE_DIRS}/core ${SKIA_INCLUDE_DIRS}/gpu)
target_link_libraries(skia INTERFACE ${SKIA_LDFLAGS} -Wl,-rpath-link,${SKIA_LIBRARY_DIRS})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set_directory_properties(PROPERTIES COMPILE_OPTIONS -Wno-psabi)
endif()

add_subdirectory(initramfs_init)
add_subdirectory(init)
add_subdirectory(DSO)
add_subdirectory(system_support)
