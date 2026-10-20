cmake_minimum_required(VERSION 3.20)

project(Lab02_BunnyGrid_Instancing LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------
# OpenGL
# -------------------------------------------------

find_package(OpenGL REQUIRED)

# -------------------------------------------------
# FetchContent
# -------------------------------------------------

include(FetchContent)

# -------------------------------------------------
# GLFW 3.4
# -------------------------------------------------

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

# -------------------------------------------------
# GLAD 2 - OpenGL 3.3 Core
# -------------------------------------------------

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
    SOURCE_SUBDIR cmake
)

FetchContent_MakeAvailable(glad)

glad_add_library(
    glad_gl_core_33
    REPRODUCIBLE
    API gl:core=3.3
)

# -------------------------------------------------
# Lab 00 executable
# -------------------------------------------------




add_executable(
    Lab02_BunnyGrid_Instancing
    src/main.cpp
)

add_dependencies(
    Lab02_BunnyGrid_Instancing
    glad_gl_core_33
)

# Our own header files
target_include_directories(
    Lab02_BunnyGrid_Instancing
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    Lab02_BunnyGrid_Instancing
    PRIVATE
    glad_gl_core_33
    glfw
    OpenGL::GL
)