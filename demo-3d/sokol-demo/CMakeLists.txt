add_library(sokol-demo STATIC
    demo/Demo.hpp
    demo/Demo.cpp
    demo/App.hpp
    demo/App.cpp
    demo/GUI.hpp
)

target_include_directories(sokol-demo INTERFACE .)

target_link_libraries(sokol-demo PUBLIC
    sokol
)