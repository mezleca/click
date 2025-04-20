cmake_minimum_required(VERSION 3.13)

# vars
set(include_dirs)
set(link_libs)

# cpp files
file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# hpp files
file(GLOB_RECURSE HEADERS
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.hpp
)

set(JSONCPP_SOURCES
    ${CMAKE_SOURCE_DIR}/external/jsoncpp/src/lib_json/json_reader.cpp
    ${CMAKE_SOURCE_DIR}/external/jsoncpp/src/lib_json/json_value.cpp
    ${CMAKE_SOURCE_DIR}/external/jsoncpp/src/lib_json/json_writer.cpp
)

# set project name
project(elterclick)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${JSONCPP_SOURCES})

# add subdirectories
add_subdirectory(external/sdl)
add_subdirectory(external/glad)
add_subdirectory(external/imgui)

# add include directories
list(APPEND include_dirs external/imgui external/sdl/include external/glad/include external/jsoncpp/include)

# include linux garbage
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    find_package(X11 REQUIRED COMPONENTS Xi XTest)
    list(APPEND include_dirs ${X11_INCLUDE_DIR})
    list(APPEND link_libs
        ${X11_LIBRARIES}
        ${X11_Xi_LIB}
        ${X11_XTest_LIB}
    )
endif()

# include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${include_dirs}
)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SDL2 
    glad 
    imgui
    ${link_libs}
)
