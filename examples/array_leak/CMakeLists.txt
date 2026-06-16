cmake_minimum_required(VERSION 3.16)
project(array_leak LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LEAK_DETECTOR_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

add_executable(array_leak
    main.cpp
    "${LEAK_DETECTOR_INCLUDE}/leak_detector.cpp"
)
target_include_directories(array_leak PRIVATE "${LEAK_DETECTOR_INCLUDE}")
