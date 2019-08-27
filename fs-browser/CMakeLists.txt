# Lib
add_subdirectory(browser-lib)

set_target_properties(fsbrowser PROPERTIES FOLDER "demos\\fs-browser")

# WS
add_executable(fs-ws-server
    ws-server/fs-ws-server.cpp
)

target_link_libraries(fs-ws-server
    fsbrowser
    boost-beast
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(fs-ws-server PROPERTIES FOLDER "demos\\fs-browser")

# CEF
add_executable(fs-cef
    cef/fs-cef.cpp
    ${CEF_CMAKE_EXECUTABLE_RESOURCES}
)

target_link_libraries(fs-cef
    cefdll_wrapper
    jsbind-cef
    fsbrowser
)

# this should be added by cef-cmake
# but to support earlier cmake versions which don't support
# target_link_options we manually add them here
if (CEF_CMAKE_OS_POSIX)
    set_target_properties(fs-cef PROPERTIES
        # passing local rpath linker flags
        LINK_FLAGS "-Wl,-rpath,.:$ORIGIN"
    )
endif()

set_target_properties(fs-cef PROPERTIES FOLDER "demos\\fs-browser")