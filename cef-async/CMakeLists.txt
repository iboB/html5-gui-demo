add_executable(cef-async
    cef-async.cpp
    ${CEF_CMAKE_EXECUTABLE_RESOURCES}
)

target_link_libraries(cef-async
    cefdll_wrapper
    jsbind-cef
    helper_lib
)

# this should be added by cef-cmake
# but to support earlier cmake versions which don't support
# target_link_options we manually add them here
if (CEF_CMAKE_OS_POSIX)
    set_target_properties(cef-async PROPERTIES
        # passing local rpath linker flags
        LINK_FLAGS "-Wl,-rpath,.:$ORIGIN"
    )
endif()

set_target_properties(cef-async PROPERTIES FOLDER "demos")
