cmake_minimum_required(VERSION 3.16)

project(F1)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # Prefer static libraries where possible

find_package(GLEW CONFIG REQUIRED)
find_package(GLFW3 CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)

add_executable(F1 
    src/main.cpp
    src/car.cpp
    src/shader.cpp
    src/circuit.cpp
    src/wheel.cpp
    src/utils.cpp
    src/audio.cpp
    # src/dashboard.cpp
                )


# copy the assets into the build/debug folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
file(COPY ${ASSETS_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

message(STATUS "Copying assets from ${ASSETS_DIR} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Link GLEW and GLFW.

# 替换链接行
target_link_libraries(F1 PRIVATE
    GLEW::glew
    glfw 
    opengl32
    OpenAL::OpenAL
    ) 

target_include_directories(F1 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )