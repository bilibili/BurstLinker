//
// Created by succlz123 on 17-8-7.
//

#include <iostream>
#include "BurstLinker.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../thirdpart/stb_image.h"

using namespace std;
using namespace blk;

long long currentTimeMs() {
    chrono::time_point<chrono::system_clock, chrono::milliseconds> tp = chrono::time_point_cast<chrono::milliseconds>(
            chrono::system_clock::now());
    auto tmp = chrono::duration_cast<chrono::milliseconds>(tp.time_since_epoch());
    auto timestamp = tmp.count();
    return timestamp;
}

int main(int argc, char *argv[]) {
    QuantizerType quantizerType = QuantizerType::Octree;
    DitherType ditherType = DitherType::NO;
    int delay = 0;
    const char *fileName = nullptr;

    int startPosition = 1;
    if (argc > 1) {
        char *qt = argv[startPosition];
        if (!strcmp(qt, "-q0")) {
            quantizerType = QuantizerType::Uniform;
            startPosition++;
        } else if (!strcmp(qt, "-q1")) {
            quantizerType = QuantizerType::MedianCut;
            startPosition++;
        } else if (!strcmp(qt, "-q2")) {
            quantizerType = QuantizerType::KMeans;
            startPosition++;
        } else if (!strcmp(qt, "-q3")) {
            quantizerType = QuantizerType::Random;
            startPosition++;
        } else if (!strcmp(qt, "-q4")) {
            quantizerType = QuantizerType::Octree;
            startPosition++;
        } else if (!strcmp(qt, "-q5")) {
            quantizerType = QuantizerType::NeuQuant;
            startPosition++;
        }
        char *dt = argv[startPosition];
        if (!strcmp(dt, "-d0")) {
            ditherType = DitherType::NO;
            startPosition++;
        } else if (!strcmp(dt, "-d1")) {
            ditherType = DitherType::M2;
            startPosition++;
        } else if (!strcmp(dt, "-d2")) {
            ditherType = DitherType::Bayer;
            startPosition++;
        } else if (!strcmp(dt, "-d3")) {
            ditherType = DitherType::FloydSteinberg;
            startPosition++;
        }
        delay = atol(argv[startPosition]);
        if (delay <= 0) {
            cout << "Delay time is too short" << endl;
            return 0;
        }
        startPosition++;
        fileName = argv[startPosition];
    } else {
        cout << "Missing input parameters" << endl;
        return 0;
    }

    int width, height, n;
    unsigned char *data = stbi_load(fileName, &width, &height, &n, 0);
    if (!data) {
        cout << "Image load failed" << endl;
        stbi_image_free(data);
        return 0;
    }
    int imageSize = width * height;
    if (width >= 65536 || height >= 65536) {
        cout << "Image is too large " << width * height << endl;
        stbi_image_free(data);
        return 0;
    }

    BurstLinker burstLinker;
    if (!burstLinker.init("out.gif", width, height, 0, 4)) {
        cout << "GifEncoder init fail" << endl;
        stbi_image_free(data);
        return 0;
    }

    long long currentTime = currentTimeMs();
    cout << "Start" << endl;

    vector<uint32_t *> imagePixels;
    for (int i = startPosition; i < argc; ++i) {
        const char *processFileName = argv[i];
        int processWidth, processHeight, processN;
        unsigned char *processImage = stbi_load(processFileName, &processWidth, &processHeight, &processN, 0);
        if (!processImage) {
            cout << "Image load failed " << processFileName << endl;
            stbi_image_free(processImage);
            return 0;
        }
        if (processWidth != width || processHeight != height) {
            cout << "Image is not the same width or height " << processFileName << endl;
            stbi_image_free(processImage);
            return 0;
        }
        if (imageSize < width || imageSize < height) {
            cout << "C6386 " << processFileName << endl;
            stbi_image_free(processImage);
            return 0;
        }
        auto *imagePixel = new uint32_t[imageSize];
        memset(imagePixel, 0, imageSize * sizeof(uint32_t));

        int pixelIndex = 0;
        int index = 0;
        int r = 0;
        int g = 0;
        int b = 0;
        for (uint32_t k = 0; k < height; k++) {
            for (uint32_t j = 0; j < width; j++) {
                if (n == 3) {
                    r = processImage[index++];
                    g = processImage[index++];
                    b = processImage[index++];
                } else if (n == 4) {
                    r = processImage[index++];
                    g = processImage[index++];
                    b = processImage[index++];
                    index++;
                } else {
					cout << "Unsupported images" << endl;
                    return 0;
                }
                int bgr = b << 16 | g << 8 | r;
                imagePixel[pixelIndex++] = static_cast<uint32_t>(bgr);
            }
        }
        stbi_image_free(processImage);
        imagePixels.emplace_back(imagePixel);
    }

    burstLinker.connect(imagePixels, delay, quantizerType, ditherType, 0, 0);

    long long diff = currentTimeMs() - currentTime;
    cout << "End " << diff << "ms" << endl;

    burstLinker.release();
    stbi_image_free(data);

    return 0;
}
