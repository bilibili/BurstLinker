//
// Created by succlz123 on 17-9-30.
//

#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

#include "GifLogger.h"

#if defined(__Android__) || defined(__AndroidLog__)

#include  <android/log.h>

#define LOG_TAG "BURSTLINKER"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG,__VA_ARGS__)

#elif defined(__Other__)

#endif

using namespace std;

long long currentTime = 0;

long long currentTimeMillis() {
    //struct timeval tv{};
    //gettimeofday(&tv, nullptr);
    //return ((unsigned long long) tv.tv_sec * 1000 + (unsigned long long) tv.tv_usec / 1000);
    chrono::time_point<chrono::system_clock, chrono::milliseconds> tp = chrono::time_point_cast<chrono::milliseconds>(
            chrono::system_clock::now());
    auto tmp = chrono::duration_cast<chrono::milliseconds>(tp.time_since_epoch());
    auto timestamp = tmp.count();
    return timestamp;
}

void GifLogger::log(bool show, string str) {
    if (!show) {
        return;
    }
    long long diff = currentTimeMillis() - currentTime;
    if (currentTime == 0) {
        diff = 0;
    }
#if defined(__Android__) || defined(__AndroidLog__)
    LOGI("%s time : %dms", str.c_str(), diff);
#elif defined(__Other__)
    cout << str << " - time " << diff << "ms" << endl;
#endif
    currentTime = currentTimeMillis();
}
