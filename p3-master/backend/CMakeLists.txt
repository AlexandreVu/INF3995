cmake_minimum_required(VERSION 3.14)

IF(APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
ENDIF()

project(backend)

set(CMAKE_CXX_STANDARD 17)

set(WARNING_FLAGS -Wextra -Wall -Wpedantic)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(projects)
