add_executable(cef-sync
    cef-sync.cpp
    ${CEF_CMAKE_EXECUTABLE_RESOURCES}
)

target_link_libraries(cef-sync
    cefdll_wrapper
)
