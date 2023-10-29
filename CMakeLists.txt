cmake_minimum_required(VERSION 3.26)
project(lab2)

set(CMAKE_CXX_STANDARD 17)

add_executable(basic_lighting_specular basic_lighting_specular.cpp glut_ply.cpp glad.c objeto.cpp stb_image.cpp
        basic_lighting_specular.cpp)

target_link_libraries(basic_lighting_specular -lOpenGL32 -lglfw3)

