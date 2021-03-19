option(BUILD_SSL OFF)

option(BUILD_TESTS OFF)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(restbed)
add_subdirectory(zeromq)
add_subdirectory(nlohmann)
add_subdirectory(jsoncpp)
add_subdirectory(jwt-cpp)
add_subdirectory(sqlite-amalgamation)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)


target_include_directories(restbed-static PUBLIC restbed/source)

