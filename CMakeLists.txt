cmake_minimum_required(VERSION 4.3)
project(my_3d_world C)

set(CMAKE_C_STANDARD 11)

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        cglm
        GIT_REPOSITORY https://github.com/recp/cglm.git
        GIT_TAG        v0.9.4
)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw cglm)

add_executable(my_3d_world main.c)

target_include_directories(my_3d_world PRIVATE include)

target_link_libraries(my_3d_world PRIVATE glad glfw cglm)

if(WIN32)
    target_link_libraries(my_3d_world PRIVATE dwmapi)
endif()