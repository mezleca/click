cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE HEADERS
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.hpp
)

project(elterclick)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# sdl
add_subdirectory(external/sdl)
target_include_directories(${PROJECT_NAME} PUBLIC external/sdl/include)

# glad
add_subdirectory(external/glad)
target_include_directories(${PROJECT_NAME} PUBLIC external/glad/include)

# json
add_subdirectory(external/json)
target_include_directories(${PROJECT_NAME} PUBLIC external/json/include)

# imgui
add_subdirectory(external/imgui)
target_include_directories(${PROJECT_NAME} PUBLIC external/imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
