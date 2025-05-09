cmake_minimum_required(VERSION 3.13)
project(imgui)

file(GLOB IMGUI_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

# add CPM
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

# add imgui library
add_library(imgui STATIC ${IMGUI_SRCS})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# add glfw via CPM
CPMAddPackage("gh:glfw/glfw#7b6aead")

find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
