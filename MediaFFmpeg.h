//
// Created by 孙天宇 on 2022/5/20.
//

#ifndef VIDEOFFMPEG_MEDIAFFMPEG_H
#define VIDEOFFMPEG_MEDIAFFMPEG_H

#include <string>

extern "C" {
#include "include/libavformat/avformat.h"
#include "include/libswscale/swscale.h"
#include "include/libavcodec/avcodec.h"
};

/*
 * 提供基于ffmpeg的对于视频的帧信息读取、解码、编码(TODO)以及封装(TODO)操作
 */
class MediaFFmpeg {
public:
    /*
     * 单例模式获取ffmpeg工具类
     */
    static MediaFFmpeg *Get() {
        static MediaFFmpeg media_ffmpeg;
        return &media_ffmpeg;
    }

    /*
     * 打开指定路径的文件, 如果存在已经打开的文件则不打开新的文件
     *
     * @param  path 待打开的文件路径
     * @return 是否打开成功
     */
    bool Open(const char *path);

    /*
     * 将原先打开的文件关闭
     */
    void Close();

    /*
     * 进行视频包读取
     *
     * @return 从视频文件中读取出来的视频包数据（解码前的数据）
     */
    AVPacket Read();

    /*
     * 进行视频包解码处理
     *
     * @param 待解码的视频包（压缩数据）
     * @return 解码完成的（原始）视频帧数据
     */
    AVFrame *Decode(const AVPacket *packet);

    /*
     * 获取相关错误信息
     *
     * @return 错误信息
     */
    std::string GetError();

    /*
     * 析构函数
     */
    virtual ~MediaFFmpeg();

    /*
     * 读取文件的总时长
     */
    long total_duration_ = 0;
protected:
    /*
     *
     */
    int video_stream_ = 0;
    /*
     * 错误信息缓冲区
     */
    char error_buff_[2048];
    // TODO 在实际使用的时候需要使用mutex锁来应对多线程情况下的调用

    /*
     * 格式化IO上下文
     */
    AVFormatContext *ac_ = nullptr;

    /*
     * 获取解码器上下文
     */
    AVCodecContext *codec_context_ = nullptr;
    /*
     * YUV视频数据信息（解码后数据）
     */
    AVFrame *yuv_ = nullptr;

    /*
     * 将构造函数设置成保护类型，需要使用单例模式来获取对应实例(使用单例模式的原因是否是减少内存的分配使用?这里参考的网络上的方案)
     */
    MediaFFmpeg();

private:
    /*
     * 获取解码上下文
     *
     * @param stream 待处理解码上下文
     * @param error_buff 异常信息缓冲区
     */
    void *GetCodecContext(const AVStream *stream, char *error_buff);

};


#endif //VIDEOFFMPEG_MEDIAFFMPEG_H
