find_package(PkgConfig REQUIRED)

set (zmqServer
    serverZMQ/ServerZmq.cpp 
    serverZMQ/ServerZmq.h
)

add_executable(server main.cpp ${zmqServer})
target_compile_options(server PRIVATE ${WARNING_FLAGS})
target_link_libraries(server PRIVATE common)
