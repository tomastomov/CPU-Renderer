cmake_minimum_required(VERSION 3.25) 

if(DEFINED ENV{VCPKG_ROOT})
    set(
        CMAKE_TOOLCHAIN_FILE
        "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file"
    )
endif()

project(CPU-Renderer LANGUAGES CXX)
add_executable(CPU-Renderer src/main.cpp "src/headers/pch.h" "src/headers/Log.h")

target_compile_features(CPU-Renderer PRIVATE cxx_std_20)

find_package(SDL3 CONFIG REQUIRED)

target_link_libraries(CPU-Renderer PRIVATE SDL3::SDL3)

target_include_directories(CPU-Renderer PRIVATE src/headers)

target_precompile_headers(CPU-Renderer PRIVATE src/headers/pch.h)