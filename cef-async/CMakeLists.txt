add_executable(cef-async
    cef-async.cpp
    ${CEF_CMAKE_EXECUTABLE_RESOURCES}
)

target_link_libraries(cef-async
    cefdll_wrapper
    jsbind-cef
    helper_lib
)
