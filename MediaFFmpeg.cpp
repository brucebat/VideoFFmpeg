//
// Created by 孙天宇 on 2022/5/20.
//

#include "MediaFFmpeg.h"

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

bool MediaFFmpeg::Open(const char *path) {
    // 打开指定文件之前需要先关闭当前打开的文件
    Close();
    // todo 线程安全使用锁来进行并发控制
    // 打开一个视频文件
    int read = avformat_open_input(&ac_, path, nullptr, nullptr);
    if (read != 0) {
        // 打开文件失败，需要设置失败原因
        av_strerror(read, error_buff_, sizeof(error_buff_));
        return false;
    }
    // 获取读取的文件的总时间
    total_duration_ = (ac_->duration / AV_TIME_BASE) * 1000;
    for (int i = 0; i < ac_->nb_streams; ++i) {
        // 获取待处理的视频流数据
        AVStream *stream = ac_->streams[i];
        // 获取解码器，判断当前文件是否可以进行解码处理
        const AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
        if (!codec) {
            // todo 设置异常信息: 无法进行视频解码操作
            return false;
        }
        // 根据解码器进行解码上下文分配,并进行解码器相关信息复制
        AVCodecContext *codec_context = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codec_context, stream->codecpar);
        codec_context->pkt_timebase = stream->time_base;
        // 判断是否是视频
        if (codec_context->codec_type == AVMEDIA_TYPE_VIDEO) {
            int error = avcodec_open2(codec_context, codec, nullptr);
            if (error != 0) {
                // 使用解码器打开异常
                av_strerror(error, error_buff_, sizeof(error_buff_));
                return false;
            }
        }
    }
    return true;
}

void MediaFFmpeg::Close() {
    if (ac_) {
        // 进行上下文关闭
        avformat_close_input(&ac_);
    }
    if (yuv_) {
        // 释放视频数据信息
        av_frame_free(&yuv_);
    }
}

AVPacket MediaFFmpeg::Read() {
    AVPacket packet;
    std::memset(&packet, 0, sizeof(AVPacket));
    if (!ac_) {
        return packet;
    }
    int error = av_read_frame(ac_, &packet);
    if (error != 0) {
        av_strerror(error, error_buff_, sizeof(error_buff_));
    }
    return packet;
}

AVFrame *MediaFFmpeg::Decode(const AVPacket *packet) {
    if (!ac_) {
        return nullptr;
    }
    if (yuv_ == nullptr) {
        yuv_ = av_frame_alloc();
    }
    // todo 重复代码需要提取成公共方法
    AVStream *stream = ac_->streams[packet->stream_index];
    const AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
    // 根据解码器进行解码上下文分配,并进行解码器相关信息复制
    AVCodecContext *codec_context = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_context, stream->codecpar);
    codec_context->pkt_timebase = stream->time_base;
    int read = avcodec_send_packet(codec_context, packet);
    if (read != 0) {
        av_strerror(read, error_buff_, sizeof(error_buff_));
        return nullptr;
    }
    read = avcodec_receive_frame(codec_context, yuv_);
    if (read != 0) {
        av_strerror(read, error_buff_, sizeof(error_buff_));
        return nullptr;
    }
    return yuv_;
}

std::string MediaFFmpeg::GetError() {
    std::string re = this->error_buff_;
    return re;
}

MediaFFmpeg::~MediaFFmpeg() {

}

MediaFFmpeg::MediaFFmpeg() {
    // 初始化一下异常信息变量
    error_buff_[0] = '\0';
}
