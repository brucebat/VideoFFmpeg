cmake_minimum_required(VERSION 3.22)
project(VideoFFmpeg)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp)

link_directories(./lib/)
include_directories(./include/)

add_executable(VideoFFmpeg ${SOURCE_FILES})
target_link_libraries(
        VideoFFmpeg
        avcodec
        avdevice
        avfilter
        avformat
)
