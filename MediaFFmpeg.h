//
// Created by 孙天宇 on 2022/5/20.
//

#ifndef VIDEOFFMPEG_MEDIAFFMPEG_H
#define VIDEOFFMPEG_MEDIAFFMPEG_H

#include <string>

extern "C" {
#include "include/libavformat/avformat.h"
#include "include/libswscale/swscale.h"
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
        static MediaFFmpeg mediaFFmpeg;
        return &mediaFFmpeg;
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
     * @return 从视频文件中读取出来的视频包数据（压缩数据）
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

protected:
    /*
     *
     */
    int videoStream = 0;
    /*
     * 错误信息缓冲区
     */
    char errorBuff[2048];
    // TODO 在实际使用的时候需要使用mutex锁来应对多线程情况下的调用

    /*
     * 格式化IO上下文
     */
    AVFormatContext *ac = nullptr;
    /*
     * YUV视频数据信息（解码后数据）
     */
    AVFrame *yuv = nullptr;

    /*
     * 将构造函数设置成保护类型，需要使用单例模式来获取对应实例
     */
    MediaFFmpeg();

};


#endif //VIDEOFFMPEG_MEDIAFFMPEG_H
