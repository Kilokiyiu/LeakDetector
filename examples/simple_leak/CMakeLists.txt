cmake_minimum_required(VERSION 3.16)
project(simple_leak LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LEAK_DETECTOR_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

add_executable(simple_leak
    main.cpp
    "${LEAK_DETECTOR_INCLUDE}/leak_detector.cpp"
)
target_include_directories(simple_leak PRIVATE "${LEAK_DETECTOR_INCLUDE}")
