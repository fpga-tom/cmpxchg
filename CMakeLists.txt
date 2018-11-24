cmake_minimum_required(VERSION 3.12)
project(cmpxchg)

set(CMAKE_CXX_STANDARD 14)

add_executable(cmpxchg main.cpp Signal.cpp Signal.h Task.cpp Task.h)
target_link_libraries(cmpxchg pthread)