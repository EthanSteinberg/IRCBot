cmake_minimum_required (VERSION 2.8.11)
project (IRCBot)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Boost
    1.36.0 
    REQUIRED             # Fail with error if Boost is not found
    COMPONENTS system # Boost libraries by their canonical name
)    

find_package(CURL
    7.45.0
    REQUIRED
)    

find_package(Threads 
    REQUIRED
)

find_package(rapidjson 
    REQUIRED
)

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS} ${RAPIDJSON_CXX_FLAGS}")

file(GLOB SOURCES "src/*.cpp")

include_directories(src ${Boost_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${RAPIDJSON_INCLUDE_DIRS})

add_executable(ircbot ${SOURCES})
target_link_libraries(ircbot ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${RAPIDJSON_LIBRARIES})
