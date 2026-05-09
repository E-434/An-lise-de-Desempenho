#Código gerado por IA
cmake_minimum_required(VERSION 3.10)

project(SortingDatasetsGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flags de otimização
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Executável principal
add_executable(generator gerador.cpp)

# Cria pasta datasets automaticamente no build
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/datasets)