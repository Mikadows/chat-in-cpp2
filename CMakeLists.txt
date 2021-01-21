cmake_minimum_required(VERSION 3.2)

project(chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server server.cpp)
add_executable(client client.cpp)

find_package(Threads REQUIRED)

if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET server PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET server PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()

if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(server "${CMAKE_THREAD_LIBS_INIT}")
endif()
