cmake_minimum_required(VERSION 3.21)
project(toy-renderer)
set(CMAKE_CXX_STANDARD 17)

# load deps
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glad)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glm)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/nfd)
file(GLOB IMGUI_SOURCES 
    ${PROJECT_SOURCE_DIR}/deps/imgui/*.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/deps/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC 
    ${PROJECT_SOURCE_DIR}/deps/imgui
    ${PROJECT_SOURCE_DIR}/deps/imgui/backends
    ${PROJECT_SOURCE_DIR}/deps/glfw/include
)
include_directories(${PROJECT_SOURCE_DIR}/deps)

# load vulkan
find_package(Vulkan REQUIRED)

# add source code
add_executable(${PROJECT_NAME})
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw glm imgui nfd Vulkan::Vulkan)
