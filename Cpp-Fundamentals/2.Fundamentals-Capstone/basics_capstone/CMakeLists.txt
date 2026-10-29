cmake_minimum_required(VERSION 3.16)
project(basics_capstone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(capstone
    src/main.cpp
    src/inverse_kinematics.cpp
    src/math_utils.cpp
    src/user_interface.cpp
    src/utility_functions.cpp
)

target_include_directories(capstone PRIVATE includes)
target_compile_options(capstone PRIVATE -Wall -Wextra)