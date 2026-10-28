
cmake_minimum_required(VERSION 3.20)

project(GraphicsLab05_Sphere LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw)

# GLAD 2
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

FetchContent_Declare(
    imgui_source
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b
)

FetchContent_MakeAvailable(imgui_source)

set(IMGUI_DIR "${imgui_source_SOURCE_DIR}")

add_library(imgui STATIC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)



target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)


target_link_libraries(imgui PUBLIC glfw)

# Application
add_executable(GraphicsLab05_Sphere src/main.cpp)

target_link_libraries(GraphicsLab05_Sphere
    PRIVATE
        glfw
        glad_gl_core_33
        imgui
)

if(WIN32)
    target_link_libraries(GraphicsLab05_Sphere PRIVATE opengl32)
endif()
