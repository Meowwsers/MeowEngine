cmake_minimum_required(VERSION 3.26)
project(MeowEngine)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)

add_executable(MeowEngine main.cpp
        MeowEngine/MeowEngine.hpp
        MeowEngine/Core.hpp
        MeowEngine/Application.hpp
        MeowEngine/Graphics.hpp
        MeowEngine/EventSystem.hpp
        MeowEngine/Input.hpp
        MeowEngine/Player.hpp
        MeowEngine/Collider.hpp
        MeowEngine/Math.hpp
        MeowEngine/ImageProcess.hpp
        MeowEngine/GenerateNoise.hpp
)
target_link_libraries(MeowEngine PRIVATE SDL3::SDL3)
target_link_libraries(MeowEngine PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(MeowEngine PRIVATE SDL3_image::SDL3_image)
