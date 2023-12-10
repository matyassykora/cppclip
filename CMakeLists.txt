cmake_minimum_required(VERSION 3.26)
project(cppclip)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.14.0
)
FetchContent_GetProperties(googletest)
if (NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BUILD_DIR})
endif ()
FetchContent_MakeAvailable(googletest)

include_directories(.)

add_executable(cppclip
        CppClip.hpp
        main.cpp
        tests/help_test.cpp
        tests/nargs_test.cpp
        tests/add_test.cpp
        )

target_link_libraries(cppclip gtest_main gmock_main)
