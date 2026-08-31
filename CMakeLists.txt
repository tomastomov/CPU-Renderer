cmake_minimum_required(VERSION 3.25) 

if(DEFINED ENV{VCPKG_ROOT})
    set(
        CMAKE_TOOLCHAIN_FILE
        "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file"
    )
endif()

project(CPU-Renderer LANGUAGES CXX)
add_executable(CPU-Renderer src/main.cpp "src/headers/pch.h" "src/headers/Log.h" "src/headers/Vector2.h" "src/Vector2.cpp"   "src/headers/Matrix3x3.h" "src/Matrix3x3.cpp" "src/headers/Quad2D.h" "src/headers/Triangle2D.h" "src/headers/GameConfig.h" "src/headers/external/stb_image.h" "src/headers/Texture.h" "src/Texture.cpp" "src/headers/Pixel.h" "src/headers/Vertex2.h" "src/headers/ThreadPool.h" "src/ThreadPool.cpp" "src/headers/Renderer2D.h"   "src/headers/Utils.h" "src/headers/CustomMath.h" "src/headers/Circle2D.h" "src/headers/Renderer3D.h"  "src/headers/Vector3.h" "src/headers/Matrix4x4.h" "src/Matrix4x4.cpp" "src/Vector3.cpp" "src/headers/Tethradon.h" "src/headers/Cube.h" "src/headers/Camera.h")

target_compile_features(CPU-Renderer PRIVATE cxx_std_23)

find_package(SDL3 CONFIG REQUIRED)

target_link_libraries(CPU-Renderer PRIVATE SDL3::SDL3)

target_include_directories(CPU-Renderer PRIVATE src/headers)

target_precompile_headers(CPU-Renderer PRIVATE src/headers/pch.h)

target_compile_options(CPU-Renderer PRIVATE
    $<$<CONFIG:Release>:/O2>
)

add_custom_command(TARGET CPU-Renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/src/resources"
    "$<TARGET_FILE_DIR:CPU-Renderer>/resources"
)

if(MSVC)
    target_compile_options(CPU-Renderer PRIVATE
        $<$<CONFIG:Debug>:/FAs>
        $<$<CONFIG:Release>:/FAs>
    )
endif()