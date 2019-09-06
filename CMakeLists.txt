cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(html5-gui-demo)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(HTML_5_DEMO_CLI_ONLY "only enable command-line demos (suitable when there's no X)" OFF)

######################
# warnings
if(MSVC)
    set(WG_WARNING_FLAGS "") # the default is fine here
else()
    set(WG_WARNING_FLAGS "-Wall -Wextra -Wno-unused-variable")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WG_WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WG_WARNING_FLAGS}")

######################
# cmake lib
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cef-cmake/cmake
)
include(src_group)

######################
# deps
find_package(Threads REQUIRED)

set(CEF_USE_SANDBOX OFF CACHE BOOL "Force turning off of sandbox")
include(cef_cmake)

######################
# subdirectories
add_subdirectory(third_party)
if(NOT HTML_5_DEMO_CLI_ONLY)
    set_target_properties(cefdll_wrapper PROPERTIES FOLDER "third_party")
endif()

add_subdirectory(helper_lib)
add_subdirectory(ws-simple)
if(NOT HTML_5_DEMO_CLI_ONLY)
    add_subdirectory(cef-sync)
    add_subdirectory(cef-async)
    add_subdirectory(demo-3d)
endif()
add_subdirectory(fs-browser)