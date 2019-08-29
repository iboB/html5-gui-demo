add_subdirectory(boost)
add_subdirectory(cef-cmake)
add_subdirectory(jsbind-cef)

add_library(sokol INTERFACE)
target_sources(sokol INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/sokol/sokol_app.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sokol/sokol_gfx.h
)
target_include_directories(sokol INTERFACE sokol)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(sokol INTERFACE
        GL
        X11
        dl
        pthread
    )
endif()

