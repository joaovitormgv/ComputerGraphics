cmake_minimum_required(VERSION 3.29)
project(CG1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "C:/SDL2/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

set(SOURCE_FILES
        src/main.cpp
        src/Vec3.cpp
        src/Ray.cpp
        src/Sphere.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})