cmake_minimum_required(VERSION 3.13)
project(imgui)

file(GLOB IMGUI_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

add_library(imgui STATIC ${IMGUI_SRCS})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC SDL2::SDL2 OpenGL::GL)
