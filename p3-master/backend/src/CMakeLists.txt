add_library(common services/ReadConfig.cpp services/zmqMessageHandler.hpp requestType.hpp)
target_include_directories(common PUBLIC services)
link_libraries(${JSONCPP_LIBRARIES})
find_package(Sqlite3)
target_link_libraries(common PUBLIC 
    restbed-static
    cppzmq
    nlohmann_json::nlohmann_json
    jsoncpp_lib
    jwt
    sqlite3::sqlite3
)
