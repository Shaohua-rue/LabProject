cmake_minimum_required(VERSION 3.10)
project(UseJson)

include_directories(
    include
)

add_executable(jsonExample
    src/jsonExample.cpp
)

add_executable(server
    src/server.cpp
)
add_executable(client
    src/client.cpp
)