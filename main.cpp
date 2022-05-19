#include <iostream>

extern "C" {
#include "include/libavdevice/avdevice.h"
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    // 进行ffmpeg方法测试
    std::cout << "avdevice_licence --> " << avdevice_license() << std::endl;
    std::cout << "avdevice_configuration --> " << avdevice_configuration() << std::endl;
    return 0;
}
