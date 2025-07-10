cmake_minimum_required(VERSION 3.15)

project(fft_img_comp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find packages - use the exact syntax from vcpkg output
find_package(OpenCV CONFIG REQUIRED)
find_package(FFTW3f CONFIG REQUIRED)

add_library(core
    src/image.hpp
    src/fft.hpp
    src/fftw_plan_manager.cpp
    src/fftw_plan_manager.hpp
    src/mask.hpp
    src/quant.hpp
    src/rle.hpp
    src/huffman.hpp
    src/compress.cpp
    src/compress.hpp
    src/decompress.cpp
    src/decompress.hpp
)

target_include_directories(core PUBLIC src)

# Use the correct target names from vcpkg output
target_link_libraries(core PUBLIC
    ${OpenCV_LIBS}
    FFTW3::fftw3f
)

add_executable(fft_ic
    src/main.cpp
    src/cli.cpp
)

target_link_libraries(fft_ic PRIVATE core)
