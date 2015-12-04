cmake_minimum_required (VERSION 2.8.11)
project (IRCBot)

file(GLOB SOURCES "src/*.cpp")

include_directories(src)

add_executable(ircbot ${SOURCES})
