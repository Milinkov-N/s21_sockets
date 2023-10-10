cmake_minimum_required(VERSION 3.21)

project(S21_Sockets-Tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF)

set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(googletest)

set(SOURCES main.cc)

add_executable(S21_Sockets-Tests)
target_sources(S21_Sockets-Tests PRIVATE ${SOURCES})

target_link_libraries(
    S21_Sockets-Tests
    PRIVATE
    s21_sockets::s21_sockets
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(S21_Sockets-Tests)
add_test(S21_Sockets-Tests S21_Sockets-Tests)
