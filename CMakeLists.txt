cmake_minimum_required(VERSION 3.10)
project(TerminalDoom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get absolute paths for source and include directories
get_filename_component(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" ABSOLUTE)
get_filename_component(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" ABSOLUTE)

# Find required packages
find_package(Curses REQUIRED)

# Add source files
add_executable(doom_terminal
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/game.cpp
    ${SRC_DIR}/player.cpp
    ${SRC_DIR}/map.cpp
    ${SRC_DIR}/renderer.cpp
)

# Include directories
target_include_directories(doom_terminal PRIVATE
    ${CURSES_INCLUDE_DIR}
    ${INCLUDE_DIR}
)

# Link libraries
target_link_libraries(doom_terminal PRIVATE
    ${CURSES_LIBRARIES}
)

# Set compiler flags
if(MSVC)
    target_compile_options(doom_terminal PRIVATE /W4)
else()
    target_compile_options(doom_terminal PRIVATE -Wall -Wextra)
endif()

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)