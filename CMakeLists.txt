cmake_minimum_required(VERSION 3.10)
project(mandelbrot)

set(CMAKE_CXX_STANDARD 17)

add_executable(mandelbrot
    main.cpp
    shaders/shader.cpp
    window/window.cpp
    third-party/glad/glad.c
)

target_compile_options(mandelbrot PRIVATE
    -O3
)

find_package(glfw3 REQUIRED)

target_include_directories(mandelbrot PRIVATE
    shaders
    window
)

target_link_libraries(mandelbrot
    glfw
    GL
    mpfr
    gmp
    dl
)
