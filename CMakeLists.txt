cmake_minimum_required(VERSION 3.21)

project(
    SockLib
    VERSION 0.1.0
    DESCRIPTION "Small and easy to use Networking Library"
    LANGUAGES CXX
)

option(SOCKLIB_BUILD_TESTS "Build SockLib tests" OFF)
# option(MYLIB_BUILD_EXAMPLES "Build mylib examples" OFF)
option(SOCKLIB_INSTALL "Generate target for installing SockLib" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GCC_CXX "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(MSVC_CXX "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

add_library(CompilerFlags INTERFACE)
target_compile_features(CompilerFlags INTERFACE cxx_std_20)
target_compile_options(CompilerFlags INTERFACE
  "$<${GCC_CXX}:$<BUILD_INTERFACE:-Wall;-Werror;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${MSVC_CXX}:$<BUILD_INTERFACE:-Wall;-W4;-WX>>"
)

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set(
    PUB_HEADERS
    include/socklib/socklib.h
)

set(
    SOURCES
    ${PUB_HEADERS}
    src/socklib.cc
    src/ws2.cc
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32 PRIVATE CompilerFlags)
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    PUBLIC_HEADER "${PUB_HEADERS}"
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
)

if(SOCKLIB_INSTALL AND NOT CMAKE_SKIP_INSTALL_RULES)
    include(GNUInstallDirs)

    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )

    install(
        TARGETS ${PROJECT_NAME} CompilerFlags
        EXPORT SockLib_Exports
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(FILES ${PUB_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(
        EXPORT SockLib_Exports
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SockLib-${PROJECT_VERSION}
        NAMESPACE SockLib::
    )

    include(CMakePackageConfigHelpers)

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SockLib-${PROJECT_VERSION}
    )

    write_basic_package_version_file(
        "${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

endif()

if(SOCKLIB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
