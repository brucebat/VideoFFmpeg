# 支持的cmake最低版本
cmake_minimum_required(VERSION 3.22)
# 声明项目名称
project(VideoFFmpeg)


set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp MediaFFmpeg.cpp MediaFFmpeg.h)

# 为链接器添加库的搜索路径
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
