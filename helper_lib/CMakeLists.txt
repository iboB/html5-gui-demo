set(srcs)
src_group(platform srcs
    helper/DirUtil.hpp
    helper/DirUtil.cpp
)

add_library(helper_lib STATIC
    ${srcs}
)

target_include_directories(helper_lib PUBLIC .)

target_link_libraries(helper_lib PUBLIC
    ${CMAKE_DL_LIBS}
)

set_target_properties(helper_lib PROPERTIES FOLDER "helper")
