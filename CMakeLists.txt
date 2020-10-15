cmake_minimum_required(VERSION 3.17)
project(hust_lab_network)
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(http_server)
add_subdirectory(http_server_minimal)
add_subdirectory(rdt)