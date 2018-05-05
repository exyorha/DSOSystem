set(dso_sources
    include/DSO/ApplicationInstance.h
	include/DSO/DSOAccumulator.h
	include/DSO/DSOAcquisition.h
    include/DSO/DSOPlatform.h
    include/DSO/DSORootView.h
    include/DSO/DSOScreenView.h
	include/DSO/IDSOAcquisitionEventReceiver.h
    DSO/ApplicationInstance.cpp
	DSO/DSOAccumulator.cpp
	DSO/DSOAcquisition.cpp
    DSO/DSOPlatform.cpp
    DSO/DSORootView.cpp
    DSO/DSOScreenView.cpp
)

set(gui_sources
    include/GUI/Application.h
    include/GUI/ApplicationPlatform.h
    include/GUI/ApplicationPlatformInterface.h
    include/GUI/ChildEvent.h
    include/GUI/DisplayInformation.h
    include/GUI/Event.h
	include/GUI/FocusEvent.h
	include/GUI/KeyEvent.h
    include/GUI/LayoutConstraint.h
    include/GUI/LayoutGuide.h
    include/GUI/LayoutItem.h
    include/GUI/MoveEvent.h
    include/GUI/Object.h
    include/GUI/PaintEvent.h
    include/GUI/ResizeEvent.h
    include/GUI/TextView.h
    include/GUI/View.h
    GUI/Application.cpp
    GUI/ApplicationPlatform.cpp
    GUI/ChildEvent.cpp
    GUI/Event.cpp
	GUI/FocusEvent.cpp
	GUI/KeyEvent.cpp
    GUI/LayoutConstraint.cpp
    GUI/LayoutGuide.cpp
    GUI/LayoutItem.cpp
    GUI/MoveEvent.cpp
    GUI/Object.cpp
    GUI/PaintEvent.cpp
    GUI/ResizeEvent.cpp
    GUI/TextView.cpp
    GUI/View.cpp
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

    include/SkUserConfig.h

    main.cpp
)

source_group(DSO FILES ${dso_sources})
source_group(GUI FILES ${gui_sources})
source_group(Logging FILES ${logging_sources})
source_group(Platform FILES ${platform_sources})

target_include_directories(DSO PRIVATE include)
target_link_libraries(DSO PRIVATE skia clp)

if(UNIX)
	target_link_libraries(DSO PRIVATE libdrm)
    set_target_properties(DSO PROPERTIES LINK_FLAGS "-static")
endif()

install(TARGETS DSO DESTINATION sbin)
