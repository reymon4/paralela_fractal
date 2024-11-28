cmake_minimum_required(VERSION 3.29)
project(ejemplo2_fractal)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")  # Enable OpenMP
find_package(fmt)
find_package(SFML)
find_package(glfw3)
add_executable(ejemplo2_fractal main.cpp
        fps_counter.cpp
        fps_counter.h)
target_link_libraries(ejemplo2_fractal fmt::fmt sfml::sfml glfw)
