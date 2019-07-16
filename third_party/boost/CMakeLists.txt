if(NOT HTML5_DEMO_BOOST_DIR)
    if (OS_WINDOWS)
        set(HTML5_DEMO_BOOST_DIR boost-win)
    else()
        set(HTML5_DEMO_BOOST_DIR boost-unix)
    endif()
endif()

# "fake" target which only gives us the boost headers as an include directory
add_library(boost-headers INTERFACE)
target_include_directories(boost-headers INTERFACE ${HTML5_DEMO_BOOST_DIR})
target_compile_definitions(boost-headers INTERFACE
    -D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING=1
    -DBOOST_ALL_NO_LIB=1
)

add_library(boost-beast INTERFACE)
target_link_libraries(boost-beast INTERFACE boost-headers)
