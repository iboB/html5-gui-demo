add_executable(ws-simple-server
    ws_simple_server.cpp
)

target_link_libraries(ws-simple-server
    boost-beast
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(ws-simple-server PROPERTIES FOLDER "demos")
