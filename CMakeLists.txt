cmake_minimum_required(VERSION 3.22)
project(raytracing)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(raytracing
        Camera.h
        Color.h
        Hittable.h
        HittableList.h
        main.cpp
        Materal.h
        MovingSphere.h
        Ray.h
        RTWeekend.h
        Sphere.h
        Vec3.h)
