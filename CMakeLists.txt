cmake_minimum_required(VERSION 3.13)

add_definitions(-D_WIN32_WINNT=0x0501)

# vars
set(include_dirs)
set(link_libs)

option(MINGW_STDTHREADS_GENERATE_STDHEADERS "" ON)

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

# add CPM
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

# set project name
project(elterclick)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${JSONCPP_SOURCES})

# add subdirectories
add_subdirectory(external/glad)
add_subdirectory(external/imgui)

# add glfw via CPM
CPMAddPackage("gh:glfw/glfw#7b6aead")

# add include directories
list(APPEND include_dirs external/imgui external/glad/include external/jsoncpp/include external/mingw-std-threads)

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
    glfw
    glad 
    imgui
    ${link_libs}
)
