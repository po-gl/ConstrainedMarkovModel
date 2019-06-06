cmake_minimum_required(VERSION 3.13)
project(Markov)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -04 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -pthread")

include_directories(.)
include_directories(data)


set(SOURCE
    src/main.cpp
    src/markov.cpp
    src/constrainedmarkov.cpp
    src/mnemonicmarkov.cpp
    src/utils.cpp
    src/debug.cpp
    src/options.cpp
    src/console.cpp
    src/server.cpp)

# TODO: make sure boost for vectors is included in boost folder
set(BOOST_ROOT "./boost")
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.69.0 COMPONENTS system serialization)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(${PROJECT_NAME} ${SOURCE})
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()
