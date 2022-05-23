//
// Created by 孙天宇 on 2022/5/20.
//

#include "MediaFFmpeg.h"

bool MediaFFmpeg::Open(const char *path) {
    // 打开指定文件之前需要先关闭当前打开的文件
    Close();

    return false;
}

void MediaFFmpeg::Close() {

}

AVPacket MediaFFmpeg::Read() {
    return AVPacket();
}

AVFrame *MediaFFmpeg::Decode(const AVPacket *packet) {
    return nullptr;
}

std::string MediaFFmpeg::GetError() {
    return std::string();
}

MediaFFmpeg::~MediaFFmpeg() {

}

MediaFFmpeg::MediaFFmpeg() {

}
