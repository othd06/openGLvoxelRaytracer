

cmake_minimum_required(VERSION 3.5)

project(openGLvoxelTracer VERSION 0.0.1)

add_executable(openGLvoxelTracer
    src/main.cpp
    src/glad.c
    src/loadModel.cpp
    src/helper.cpp)

target_link_directories(openGLvoxelTracer
    PRIVATE
        /usr/lib64
        ${CMAKE_SOURCE_DIR}/glad/include/glad
        ${CMAKE_SOURCE_DIR}/glad/include/KHR)

find_package(glfw3 3.3.0 REQUIRED)
find_package(OpenGL REQUIRED)


target_link_libraries(openGLvoxelTracer
    glfw
    OpenGL::GL)




