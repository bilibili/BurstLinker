//
// Created by succlz123 on 17-9-30.
//

#include <iostream>

#include "Logger.h"

#if defined(__RenderScript__) || defined(__AndroidLog__)

#include  <android/log.h>

#define LOG_TAG "JNI_BURSTLINKER"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG,__VA_ARGS__)

#endif

using namespace blk;

static long long currentTime = 0;

static long long currentTimeMillis() {
    //struct timeval tv{};
    //gettimeofday(&tv, nullptr);
    //return ((unsigned long long) tv.tv_sec * 1000 + (unsigned long long) tv.tv_usec / 1000);
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    auto timestamp = tmp.count();
    return timestamp;
}

void Logger::log(bool show, std::string str) {
    if (!show) {
        return;
    }
    long long diff = currentTimeMillis() - currentTime;
    if (currentTime == 0) {
        diff = 0;
    }
#if defined(__RenderScript__) || defined(__AndroidLog__)
    LOGI("%s time : %dms", str.c_str(), (int) diff);
#else
    std::cout << str << " - time " << diff << "ms" << std::endl;
#endif
    currentTime = currentTimeMillis();
}
