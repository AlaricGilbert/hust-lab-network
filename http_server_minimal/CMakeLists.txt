add_executable(mini_http_server http_server_mini.cc)
if(WIN32)
    target_link_libraries(mini_http_server wsock32 ws2_32)
endif()