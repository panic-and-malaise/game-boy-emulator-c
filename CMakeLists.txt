cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(gameboy-emu LANGUAGES C)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build SDL3 as a static library")

# SDL stuff
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.8
)

FetchContent_MakeAvailable(SDL3)

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/roms/)
file(COPY ${CMAKE_SOURCE_DIR}/roms DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB SOURCES "src/*.c")

add_executable(
	main
	main.c
	${SOURCES}
)

add_executable(
	test
	test.c
	${SOURCES}
)

target_include_directories(main PRIVATE include)
target_include_directories(main PRIVATE src)

target_include_directories(test PRIVATE include)
target_include_directories(test PRIVATE src)

target_link_libraries(main PRIVATE SDL3::SDL3)
