cmake_minimum_required(VERSION 3.5.1)
project(DSOSystemHostTools)

add_library(xorgproto INTERFACE)
target_include_directories(xorgproto INTERFACE xorgproto/include)


add_executable(bdftopcf
	bdftopcf/atom.c
	bdftopcf/bdfint.h
	bdftopcf/bdfread.c
	bdftopcf/bdftopcf.c
	bdftopcf/bdfutils.c
	bdftopcf/bitmap.c
	bdftopcf/bitmap.h
	bdftopcf/bitmaputil.c
	bdftopcf/bufio.c
	bdftopcf/bufio.h
	bdftopcf/defaults.c
	bdftopcf/fileio.c
	bdftopcf/filewr.c
	bdftopcf/fntfil.h
	bdftopcf/fntfilio.h
	bdftopcf/fntfilst.h
	bdftopcf/font.h
	bdftopcf/fontaccel.c
	bdftopcf/fontink.c
	bdftopcf/fontmisc.h
	bdftopcf/fontutil.h
	bdftopcf/fontxlfd.h
	bdftopcf/pcf.h
	bdftopcf/pcfwrite.c
	bdftopcf/private.c
	bdftopcf/stubs.h
	bdftopcf/utilbitmap.c
)
target_link_libraries(bdftopcf PRIVATE xorgproto)
target_compile_definitions(bdftopcf PRIVATE "-DPACKAGE_STRING=\"bdftopcf\"")
if(MSVC)
target_include_directories(bdftopcf PRIVATE xwincompat)
target_compile_definitions(bdftopcf PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_sources(bdftopcf PRIVATE
	xwincompat/unistd.h
)
endif()

add_subdirectory(GenerateResources)

export(TARGETS bdftopcf GenerateResources FILE ${PROJECT_BINARY_DIR}/ImportHostTools.cmake)
