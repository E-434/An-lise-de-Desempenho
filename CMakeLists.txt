cmake_minimum_required(VERSION 3.10)

project(SortingDatasetsGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flags de otimização
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Executável do gerador
add_executable(generator gerador.cpp)

# Executável dos algoritmos
add_executable(sorts sorting.cpp)

# Cria pastas automaticamente no build
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/datasets)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/results)