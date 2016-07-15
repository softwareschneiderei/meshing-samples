cmake_minimum_required(VERSION 3.5)
project(gltest)

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(GLTest WIN32 main.cpp)

target_link_libraries(GLTest ${SDL2_LIBRARY} ${OPENGL_LIBRARIES})
target_include_directories(GLTest
	PRIVATE ${SDL2_INCLUDE_DIR})