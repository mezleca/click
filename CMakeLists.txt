cmake_minimum_required(VERSION 3.13)

project(elterclick)
add_executable(${PROJECT_NAME} src/main.cpp)

# sdl
add_subdirectory(external/sdl)
target_include_directories(${PROJECT_NAME} PUBLIC external/sdl/include)
