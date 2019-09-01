add_executable(nogui-3d
    nogui-3d-main.cpp
    NoGUIApp.cpp
    NoGUIApp.hpp
)

target_link_libraries(nogui-3d sokol-demo)