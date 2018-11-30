cmake_minimum_required(VERSION 3.12)
project(cmpxchg)

set(CMAKE_CXX_STANDARD 14)

add_executable(cmpxchg main.cpp Signal.cpp Signal.h Task.cpp Task.h Module.cpp Module.h Sync.cpp Sync.h Slot.cpp Slot.h Port.cpp Port.h Rx.cpp Rx.h RxBuilder.cpp RxBuilder.h DummySink.cpp DummySink.h)
target_link_libraries(cmpxchg pthread iio)