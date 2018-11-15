cmake_minimum_required(VERSION 3.11)
project(grasslands)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "-static -lpthread -static-libgcc -static-libstdc++")

#setup gldfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw-3.2.1)
include_directories(glfw-3.2.1/include)
include_directories(glm)
include_directories(glad/include)

add_executable(grasslands glad/src/glad.c src/main.cpp src/Mesh.cpp src/Mesh.h src/glTypes.h src/ObjLoader.cpp src/ObjLoader.h src/DDSLoad.h src/DDSLoad.cpp
        src/Shader.h src/Shader.cpp src/Renderer.cpp src/Renderer.h src/Texture.cpp src/Texture.h src/Transform.h src/RenderObject.cpp src/RenderObject.h src/Camera.cpp src/Camera.h src/Octree.cpp src/Octree.h src/Material.h src/BVH.cpp src/BVH.h src/Batch.cpp src/Batch.h src/SETTINGS.h src/Material.cpp src/ShadowMap.cpp src/ShadowMap.h src/PingPong.cpp src/PingPong.h src/CUBELoader.cpp src/CUBELoader.h src/PassMask.h)

target_link_libraries(grasslands glfw)