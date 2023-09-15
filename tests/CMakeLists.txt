cmake_minimum_required(VERSION 3.21)

project(SockLib-Tests)

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

add_executable(SockLib-Tests)
target_sources(SockLib-Tests PRIVATE ${SOURCES})

target_link_libraries(
    SockLib-Tests
    PRIVATE
    SockLib::SockLib
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(SockLib-Tests)
