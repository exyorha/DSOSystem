add_executable(initramfs_init
    main.cpp
)

set_target_properties(initramfs_init PROPERTIES LINK_FLAGS "-static")
target_compile_options(initramfs_init PRIVATE -fno-exceptions -fno-rtti)

install(TARGETS initramfs_init DESTINATION initramfs)
install(FILES ima-policy DESTINATION initramfs)
