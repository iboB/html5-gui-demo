add_executable(cef-sync
    cef-sync.cpp
    ${CEF_CMAKE_EXECUTABLE_RESOURCES}
)

target_link_libraries(cef-sync
    cefdll_wrapper
    jsbind-cef
    helper_lib
)

# this should be added by cef-cmake
# but to support earlier cmake versions which don't support
# target_link_options we manually add them here
set_target_properties(cef-sync PROPERTIES
    # passing local rpath linker flags
    LINK_FLAGS "-Wl,-rpath,.:$ORIGIN"
)
