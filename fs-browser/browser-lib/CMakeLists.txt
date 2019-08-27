add_library(fsbrowser STATIC
    fsbrowser/Dir.cpp
    fsbrowser/Dir.hpp
    fsbrowser/FSBrowser.hpp
    fsbrowser/FSBrowser.cpp
)

target_include_directories(fsbrowser INTERFACE .)

target_link_libraries(fsbrowser PUBLIC
    helper_lib
)