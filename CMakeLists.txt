cmake_minimum_required(VERSION 3.21)
project(toy-renderer)
set(CMAKE_CXX_STANDARD 17)

# load deps
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glad)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/deps/glm)
file(GLOB IMGUI_SOURCES ${PROJECT_SOURCE_DIR}/deps/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${PROJECT_SOURCE_DIR}/deps/imgui)

# add source code
add_executable(${PROJECT_NAME})
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw glm imgui)

# cd build
# cmake -G "Visual Studio 17" -A x64 ..
# cmake --build . --config Release -- /verbosity:minimal

