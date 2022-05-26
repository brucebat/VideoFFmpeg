#include <iostream>

extern "C" {
#include "include/libavdevice/avdevice.h"
}

#include "MediaFFmpeg.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    // 进行ffmpeg方法测试
    std::cout << "avdevice_licence --> " << avdevice_license() << std::endl;
    std::cout << "avdevice_configuration --> " << avdevice_configuration() << std::endl;
    char video_path[2048] = "/Users/suntianyu/Downloads/蜘蛛侠.mp4";
    if (MediaFFmpeg::Get()->Open(video_path)) {
        std::cout << "文件打开成功" << std::endl;
    } else {
        return 0;
    }
    AVPacket packet = MediaFFmpeg::Get()->Read();
    while (packet.size != 0) {
        std::cout << "视频数据包的pts = " << packet.pts << std::endl;
        AVFrame *yuv = MediaFFmpeg::Get()->Decode(&packet);
        if (yuv) {
            std::cout << "解码成功, 对应数据包的持续时间duration = " << yuv->pkt_duration << ", pts = " << yuv->pts << std::endl;
        } else {
            std::cout << "解码失败, 错误信息为: " << MediaFFmpeg::Get()->GetError() << std::endl;
        }
        av_packet_unref(&packet);
        packet = MediaFFmpeg::Get()->Read();
    }
    // 释放空间
    av_packet_unref(&packet);

    return 0;
}
