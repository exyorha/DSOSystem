function(begin_resource_library name)
	add_library(${name} STATIC
		${CMAKE_CURRENT_BINARY_DIR}/ResourceLibrary${name}/${name}.cpp
		${CMAKE_CURRENT_BINARY_DIR}/ResourceLibrary${name}/${name}.h
	)
	target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/ResourceLibrary${name})

	set(RESOURCE_LIBRARY_CONTENTS_${name} "" CACHE INTERNAL "" FORCE)
endfunction()

function(add_font_resource name filename resourcename)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fonts/${resourcename}.pcf
		COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fonts
		COMMAND bdftopcf -p4 -u4 -l -L -o ${CMAKE_CURRENT_BINARY_DIR}/fonts/${resourcename}.pcf ${filename}
		MAIN_DEPENDENCY ${filename}
		VERBATIM
	)

	add_file_resource(${name} ${CMAKE_CURRENT_BINARY_DIR}/fonts/${resourcename}.pcf ${resourcename})
endfunction()

function(add_file_resource name filename resourcename)
	list(APPEND RESOURCE_LIBRARY_CONTENTS_${name} "${resourcename}=${filename}")
	set(RESOURCE_LIBRARY_CONTENTS_${name} "${RESOURCE_LIBRARY_CONTENTS_${name}}" CACHE INTERNAL "" FORCE)
endfunction()

function(finish_resource_library name)
	set(RESOURCE_LIBRARY_DEPENDENCIES)

	foreach(dependency IN LISTS RESOURCE_LIBRARY_CONTENTS_${name})
		string(REPLACE "=" ";" parts "${dependency}")
		list(GET parts 1 filename)
		list(APPEND RESOURCE_LIBRARY_DEPENDENCIES ${filename})
	endforeach()

	add_custom_command(
		OUTPUT
			${CMAKE_CURRENT_BINARY_DIR}/ResourceLibrary${name}/${name}.cpp
			${CMAKE_CURRENT_BINARY_DIR}/ResourceLibrary${name}/${name}.h
		COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/ResourceLibrary${name}
		COMMAND
			GenerateResources
				${CMAKE_CURRENT_BINARY_DIR}/ResourceLibrary${name}/${name}.cpp
				${CMAKE_CURRENT_BINARY_DIR}/ResourceLibrary${name}/${name}.h
				${RESOURCE_LIBRARY_CONTENTS_${name}}
		DEPENDS
			${RESOURCE_LIBRARY_DEPENDENCIES}
		VERBATIM
	)
endfunction()
