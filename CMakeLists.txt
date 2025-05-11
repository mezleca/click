cmake_minimum_required(VERSION 3.13)
project(elterclick)

# until i find out how to use ming64 on win
add_definitions(-D_WIN32_WINNT=0x0501)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections -s")
set(CMAKE_CXX_FLAGS_DEBUG   "-g")

find_package(OpenGL REQUIRED)

# srcs...
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS src/*.h)

set(JSONCPP_SOURCES
    external/jsoncpp/src/lib_json/json_reader.cpp
    external/jsoncpp/src/lib_json/json_value.cpp
    external/jsoncpp/src/lib_json/json_writer.cpp
)

file(GLOB IMGUI_CORE_SOURCES "external/imgui/*.cpp")

# only glfw / opengl is needed
set(IMGUI_BACKEND_SOURCES 
    "external/imgui/backends/imgui_impl_glfw.cpp"
    "external/imgui/backends/imgui_impl_opengl3.cpp"
)

set(IMGUI_SOURCES 
    ${IMGUI_CORE_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
)

include(CPM/CPM.cmake)
CPMAddPackage("gh:glfw/glfw#7b6aead")

add_subdirectory(external/glad)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${JSONCPP_SOURCES}
    ${IMGUI_SOURCES}
)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    find_package(X11 REQUIRED COMPONENTS Xi XTest)
    target_include_directories(elterclick PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(elterclick PRIVATE
        ${X11_LIBRARIES}
        ${X11_Xi_LIB}
        ${X11_XTest_LIB}
    )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    external/imgui
    external/imgui/backends
    external/glad/include
    external/jsoncpp/include
    external/mingw-std-threads
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
    OpenGL::GL
)