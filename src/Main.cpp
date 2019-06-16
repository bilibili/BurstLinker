//
// Created by succlz123 on 17-8-7.
//

#include <iostream>
#include "BurstLinker.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../third_part/stb_image.h"

using namespace blk;

long long currentTimeMs() {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    auto timestamp = tmp.count();
    return timestamp;
}

int main(int argc, char *argv[]) {
    QuantizerType quantizerType = QuantizerType::Octree;
    DitherType ditherType = DitherType::No;
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
            ditherType = DitherType::No;
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
            std::cout << "Delay time is too short" << std::endl;
            return 0;
        }
        startPosition++;
        fileName = argv[startPosition];
    } else {
        std::cout << "Missing input parameters" << std::endl;
        return 0;
    }

    int width, height, n;
    unsigned char *data = stbi_load(fileName, &width, &height, &n, 0);
    if (!data) {
        std::cout << "Image load failed" << std::endl;
        stbi_image_free(data);
        return 0;
    }
    int imageSize = width * height;
    if (width >= 65536 || height >= 65536) {
        std::cout << "Image is too large " << width * height << std::endl;
        stbi_image_free(data);
        return 0;
    }

    BurstLinker burstLinker;
    if (!burstLinker.init("out.gif", width, height, 0, 4)) {
        std::cout << "GifEncoder init fail" << std::endl;
        stbi_image_free(data);
        return 0;
    }

    long long currentTime = currentTimeMs();
    std::cout << "Start" << std::endl;

    std::vector<std::vector<uint32_t >> tasks;
    int enableTransparency = 0;
    for (int i = startPosition; i < argc; ++i) {
        const char *processFileName = argv[i];
        int processWidth, processHeight, processN;
        unsigned char *processImage = stbi_load(processFileName, &processWidth, &processHeight, &processN, 0);
        if (!processImage) {
            std::cout << "Image load failed " << processFileName << std::endl;
            stbi_image_free(processImage);
            return 0;
        }
        if (processWidth != width || processHeight != height) {
            std::cout << "The height and width of the front and back images do not match " << processFileName << std::endl;
            stbi_image_free(processImage);
            return 0;
        }
        if (imageSize < width || imageSize < height) {
            std::cout << "C6386 " << processFileName << std::endl;
            stbi_image_free(processImage);
            return 0;
        }

        std::vector<uint32_t> image;
        image.reserve(width * height);
        int index = 0;
        int a = 255;
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
                    a = processImage[index++];
                } else {
                    std::cout << "Unsupported images" << std::endl;
                    return 0;
                }
                if (enableTransparency == 0) {
                    enableTransparency = (n == 4 ? 1 : 0);
                }
                image.push_back(a << 24 | b << 16 | g << 8 | r);
            }
        }
        stbi_image_free(processImage);
        tasks.push_back(image);
    }
    int ignoreTranslucency = 0;
    int transparencyOption = ignoreTranslucency << 8 | enableTransparency;
    burstLinker.connect(tasks, delay, quantizerType, ditherType, transparencyOption, 0, 0);

    long long diff = currentTimeMs() - currentTime;
    std::cout << "End " << diff << "ms" << std::endl;

    burstLinker.release();
    stbi_image_free(data);

    return 0;
}
