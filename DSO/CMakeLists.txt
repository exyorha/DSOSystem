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

add_executable(DSO
    ${dso_sources}
    ${gui_sources}

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

    include/SkUserConfig.h

    main.cpp
)

source_group(DSO FILES ${dso_sources})
source_group(GUI FILES ${gui_sources})

target_include_directories(DSO PRIVATE include)
target_compile_definitions(DSO PRIVATE GUI_DISABLE)
target_link_libraries(DSO PRIVATE skia clp libdrm)

install(TARGETS DSO DESTINATION sbin)
