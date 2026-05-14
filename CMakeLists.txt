cmake_minimum_required(VERSION 3.20)

project(VideoEngine)

set(CMAKE_CXX_STANDARD 20)

set(OpenCV_DIR "D:/OpenCV/opencv/build/x64/vc16/lib")

find_package(OpenCV REQUIRED)

add_executable(
        VideoEngine
        main.cpp
        src/EdgeFilter.cpp
        src/BlurFilter.cpp
)

target_include_directories(
        VideoEngine
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
        VideoEngine
        PRIVATE
        ${OpenCV_LIBS}
)

add_executable(
        SequentialBenchmark
        benchmarks/SequentialBenchmark.cpp
)

target_include_directories(
        SequentialBenchmark
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
        SequentialBenchmark
        PRIVATE
        ${OpenCV_LIBS}
)

add_executable(
        ConcurrentBenchmark
        benchmarks/ConcurrentBenchmark.cpp
        src/EdgeFilter.cpp
)

target_include_directories(
        ConcurrentBenchmark
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
        ConcurrentBenchmark
        PRIVATE
        ${OpenCV_LIBS}
)