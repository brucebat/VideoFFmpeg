//
// Created by 孙天宇 on 2022/5/20.
//

#include "MediaFFmpeg.h"

// windows才需要添加
//#pragma comment(lib, "avformat.lib")
//#pragma comment(lib, "avutil.lib")
//#pragma comment(lib, "avcodec.lib")
//#pragma comment(lib, "swscale.lib")

/*
 * TODO
 *  1. 看一下ffmpeg example代码, 参考：http://ffmpeg.org/doxygen/4.1/decode_video_8c-example.html#a11
 *  2. 关注内存泄漏
 *  3. 理解AVPackage和AVFrame
 */
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
            std::strcpy(error_buff_, "codec not found in the resource");
            return false;
        }
        // 根据解码器进行解码上下文分配,并进行解码器相关信息复制，由于上面已经判断过解码器是否存在，所以这里不需要对解码上下文进行判断
        GetCodecContext(stream);
        // 判断是否是视频
        if (!codec_context_) {
            return false;
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
    if (codec_context_) {
        avcodec_free_context(&codec_context_);
    }
}

AVPacket MediaFFmpeg::Read() {
    AVPacket packet;
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
    AVStream *stream = ac_->streams[packet->stream_index];
    // 根据解码器进行解码上下文分配,并进行解码器相关信息复制
    GetCodecContext(stream);
    // 发送数据到解码队列当中
    int ret = avcodec_send_packet(this->codec_context_, packet);
    if (ret < 0) {
        av_strerror(ret, error_buff_, sizeof(error_buff_));
        return nullptr;
    }
    // 从解码队列中接收解码成功的视频帧数据
    while (ret >= 0) {
        // todo 该方法执行时出现了-35: Resource Temporarily Unavailable错误，这里需要查看一下为什么会出现这个问题
        ret = avcodec_receive_frame(this->codec_context_, yuv_);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            av_strerror(ret, error_buff_, sizeof(error_buff_));
            return nullptr;
        }
        if (ret < 0) {
            av_strerror(ret, error_buff_, sizeof(ret));
            exit(0);
        }
    }
    return yuv_;
}

std::string MediaFFmpeg::GetError() {
    std::string re = this->error_buff_;
    return re;
}

MediaFFmpeg::~MediaFFmpeg() {
    // todo 上层忘记调用的时候可能会出现内存泄漏，需要关注如何避免该问题
}

MediaFFmpeg::MediaFFmpeg() {
    // 初始化一下异常信息变量
    // todo 这里可以使用memset来处理
    error_buff_[0] = '\0';
}

void MediaFFmpeg::GetCodecContext(const AVStream *stream) {
    // 获取解码器，判断当前文件是否可以进行解码处理
    const AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!codec) {
        std::strcpy(error_buff_, "No codec found");
        return;
    }
    // 根据解码器进行解码上下文分配,并进行解码器相关信息复制
    // 这里修改为使用成员变量来代替函数内的局部变量
    if (!codec_context_) {
        codec_context_ = avcodec_alloc_context3(codec);
    }
    avcodec_parameters_to_context(codec_context_, stream->codecpar);
    codec_context_->pkt_timebase = stream->time_base;
    if (codec_context_->codec_type == AVMEDIA_TYPE_VIDEO) {
        // 如果想要使用解码器需要将解码器上下文打开
        int error = avcodec_open2(codec_context_, codec, nullptr);
        if (error != 0) {
            // 使用解码器打开异常, 当前数据流无法进行解码
            // 此处需要将错误信息透出
            av_strerror(error, error_buff_, sizeof(error_buff_));
            // 如果打开异常则将解码器上下文变脸设置为空指针
            codec_context_ = nullptr;
        }
    }
}
