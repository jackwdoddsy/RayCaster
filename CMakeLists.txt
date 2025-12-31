cmake_minimum_required(VERSION 3.16)

project(raycaster
    VERSION 1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(raycaster
    src/main.c
)

target_link_libraries(raycaster PRIVATE m ${SDL2_LIBRARIES})

target_include_directories(raycaster PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(raycaster PRIVATE ${SDL2_LIBRARIES})

target_compile_options(raycaster PRIVATE -Wall -Wextra -O2)
