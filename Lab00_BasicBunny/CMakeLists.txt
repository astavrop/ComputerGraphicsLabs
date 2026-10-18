cmake_minimum_required(VERSION 3.20)

project(Lab00_BasicBunny LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# OpenGL
# -------------------------------
find_package(OpenGL REQUIRED)

# -------------------------------
# GLFW
# -------------------------------
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

# -------------------------------
# GLAD
# -------------------------------
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)

FetchContent_MakeAvailable(glad)

# -------------------------------
# Our executable
# -------------------------------
add_executable(
    Lab00_BasicBunny
    src/main.cpp
)

# BunnyMesh.h lives here
target_include_directories(
    Lab00_BasicBunny
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -------------------------------
# Libraries
# -------------------------------
target_link_libraries(
    Lab00_BasicBunny
    PRIVATE
    glad
    glfw
    OpenGL::GL
)