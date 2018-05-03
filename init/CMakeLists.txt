add_executable(init
    FieldBasedFileParser.cpp
    FieldBasedFileParser.h
    InitConfiguration.cpp
    InitConfiguration.h
    main.cpp
)

install(TARGETS init DESTINATION sbin)
install(FILES init.conf DESTINATION etc)

if(UNIX)
    set_target_properties(init PROPERTIES LINK_FLAGS "-static")
endif()
