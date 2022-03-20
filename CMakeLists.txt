cmake_minimum_required(VERSION 3.22)
project(raytracing)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(raytracing
        camera.h
        color.h
        hittable.h
        hittable_list.h
        main.cpp
        material.h
        moving_sphere.h
        ray.h
        rtweekend.h
        sphere.h
        vec3.h)
