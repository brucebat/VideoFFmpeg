//
// Created by 孙天宇 on 2022/5/20.
//

#include "MediaFFmpeg.h"

bool MediaFFmpeg::Open(const char *path) {
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
