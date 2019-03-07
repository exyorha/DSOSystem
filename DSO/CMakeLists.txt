begin_resource_library(DSOResources)
add_font_resource(DSOResources ${CMAKE_CURRENT_SOURCE_DIR}/Resources/fonts/helvR12.bdf helvR12)
finish_resource_library(DSOResources)

set(dso_sources
    include/DSO/ApplicationInstance.h
	include/DSO/DSOAccumulator.h
	include/DSO/DSOAcquisition.h
	include/DSO/DSOKeys.h
    include/DSO/DSOPlatform.h
    include/DSO/DSORootView.h
    #include/DSO/DSOScreenView.h
	include/DSO/IDSOAcquisitionEventReceiver.h
    DSO/ApplicationInstance.cpp
	DSO/DSOAccumulator.cpp
	DSO/DSOAcquisition.cpp
    DSO/DSOPlatform.cpp
    DSO/DSORootView.cpp
    #DSO/DSOScreenView.cpp
)

set(gui_sources
    include/GUI/Application.h
    include/GUI/ApplicationPlatform.h
    include/GUI/ApplicationPlatformInterface.h
    include/GUI/DisplayInformation.h
	include/GUI/GUIFont.h
    include/GUI/NuklearPlatform.h
	include/GUI/NuklearRenderer.h
	include/GUI/PCFFont.h
	include/GUI/PCFTypes.h
	include/GUI/PixmanUtils.h
	include/GUI/Window.h
    GUI/Application.cpp
    GUI/ApplicationPlatform.cpp
	GUI/GUIFont.cpp
	GUI/NuklearImplementation.cpp
	GUI/NuklearRenderer.cpp
	GUI/PCFFont.cpp
	GUI/PixmanUtils.cpp
	GUI/Window.cpp
)

set(logging_sources
	include/Logging/ILogSink.h
	include/Logging/LogFacility.h
	include/Logging/LogManager.h
	include/Logging/LogSink.h
	Logging/LogFacility.cpp
	Logging/LogManager.cpp
	Logging/LogSink.cpp
)

if(UNIX)
	set(platform_sources
	    include/LinuxPlatform/ILinuxDescriptorEventReceiver.h
		include/LinuxPlatform/LinuxAcquisition.h
		include/LinuxPlatform/LinuxApplicationPlatform.h
		include/LinuxPlatform/LinuxDRMRenderer.h
		include/LinuxPlatform/LinuxDSOPlatform.h
		include/LinuxPlatform/LinuxError.h
		include/LinuxPlatform/LinuxEventLoop.h
		include/LinuxPlatform/LinuxManagedDescriptor.h
		LinuxPlatform/LinuxAcquisition.cpp
		LinuxPlatform/LinuxApplicationPlatform.cpp
		LinuxPlatform/LinuxDRMRenderer.cpp
		LinuxPlatform/LinuxDSOPlatform.cpp
		LinuxPlatform/LinuxError.cpp
		LinuxPlatform/LinuxEventLoop.cpp
		LinuxPlatform/LinuxManagedDescriptor.cpp
	)
endif()

if(WIN32)
	set(platform_sources
        include/Win32/HandleWrappers.h
		include/Win32/WindowsAcquisition.h
        include/Win32/WindowsApplicationPlatform.h
		include/Win32/WindowsDSOAccumulator.h
        include/Win32/WindowsDSOPlatform.h
        include/Win32/WindowsHostWindow.h
		include/Win32/WindowsLogSink.h
        Win32/HandleWrappers.cpp
		Win32/WindowsAcquisition.cpp
        Win32/WindowsApplicationPlatform.cpp
		Win32/WindowsDSOAccumulator.cpp
        Win32/WindowsDSOPlatform.cpp
        Win32/WindowsHostWindow.cpp
		Win32/WindowsLogSink.cpp
		Resources/resources.rc
	)
endif()

add_executable(DSO
    ${dso_sources}
    ${gui_sources}
	${logging_sources}
	${platform_sources}

    main.cpp
)

source_group(DSO FILES ${dso_sources})
source_group(GUI FILES ${gui_sources})
source_group(Logging FILES ${logging_sources})
source_group(Platform FILES ${platform_sources})

target_include_directories(DSO PRIVATE include)

target_link_libraries(DSO PRIVATE nuklear pixman DSOResources)

if(UNIX)
    set_target_properties(DSO PROPERTIES LINK_FLAGS "-static")
endif()
