cmake_minimum_required(VERSION 3.14)
project(CG1)

# Define o padrão do C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir FetchContent
include(FetchContent)

# Configurar FetchContent para SDL2
FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.26.5  # Use uma versão estável
)

# Opcional: Definir opções de build para SDL2 antes de MakeAvailable
set(SDL_SHARED OFF CACHE BOOL "Build SDL as a static library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL as a static library" FORCE)
set(SDL_TEST OFF CACHE BOOL "Build SDL tests" FORCE)
set(SDL_DOC OFF CACHE BOOL "Build SDL documentation" FORCE)

# Baixar e adicionar o SDL2 ao projeto
FetchContent_MakeAvailable(SDL2)

# Lista dos arquivos fonte
set(SOURCE_FILES
        src/main.cpp
        src/Vec3.cpp
        src/Ray.cpp
        src/Sphere.cpp
        src/Plane.cpp
        src/Cilindro.cpp
)

# Adicionar o executável
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Incluir diretórios de cabeçalhos do projeto
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Linkar SDL2
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
