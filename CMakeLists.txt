cmake_minimum_required(VERSION 2.8)
project(Markov)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -04 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -pthread")

include_directories(.)
include_directories(data)


set(SOURCE
    src/main.cpp
    src/models/markov.cpp
    src/models/constrainedmarkov.cpp
    src/models/mnemonicmarkov.cpp
    src/utils.cpp
    src/debug.cpp
    src/options.cpp
    src/console.cpp
    src/server.cpp)

include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(libs)

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} MiniBoost)

