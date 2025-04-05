cmake_minimum_required(VERSION 3.10)
project(gitgobbler C)

# Compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Source and build directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
file(GLOB SOURCES "${SRC_DIR}/*.c")

# Output directory for build
set(BUILD_DIR "${CMAKE_BINARY_DIR}/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

# Create executable
add_executable(gitgobbler ${SOURCES})
