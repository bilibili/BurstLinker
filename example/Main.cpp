//
// Created by succlz123 on 17-8-7.
//

#include <iostream>
#include "../src/BurstLinker.h"

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

void addImage(const char *fileName, uint32_t width, uint32_t height, uint32_t delay, BurstLinker &burstLinker,
              QuantizerType quantizerType, DitherType ditherType) {
    uint32_t imageSize = width * height;
    if (imageSize < width || imageSize < height) {
        return;
    }
    int w, h, n;
    unsigned char *data = stbi_load(fileName, &w, &h, &n, 0);
    if (w != width || h != height) {
        std::cout << "Image is not the same width or height" << std::endl;
        return;
    }
    std::vector<uint32_t> image;
    image.reserve(imageSize);
    uint32_t index = 0;
    int a = 255;
    int r = 0;
    int g = 0;
    int b = 0;
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            if (n == 3) {
                r = data[index++];
                g = data[index++];
                b = data[index++];
            } else if (n == 4) {
                r = data[index++];
                g = data[index++];
                b = data[index++];
                a = data[index++];
            } else {
                return;
            }
            image.push_back(a << 24 | b << 16 | g << 8 | r);
        }
    }
    stbi_image_free(data);
    int ignoreTranslucency = 0;
    int enableTransparency = (n == 4 ? 1 : 0);
    int transparencyOption = ignoreTranslucency << 8 | enableTransparency;

    std::vector<std::vector<uint32_t >> images;
    images.emplace_back(image);
    images.emplace_back(image);
    burstLinker.connect(images, delay, quantizerType, ditherType, transparencyOption, 0, 0);
}

void
addImage(int r, int g, int b, uint32_t width, uint32_t height, uint32_t delay, BurstLinker &burstLinker,
         QuantizerType quantizerType, DitherType ditherType) {
    uint32_t imageSize = width * height;
    if (imageSize < width || imageSize < height) {
        return;
    }
    std::vector<uint32_t> imagePixel;
    imagePixel.reserve(imageSize);
    for (uint32_t i = 0; i < width; i++) {
        for (uint32_t j = 0; j < height; j++) {
            imagePixel.push_back(b << 16 | g << 8 | r);
        }
    }
    int ignoreTranslucency = 1;
    int enableTransparency = 0;
    int transparencyOption = ignoreTranslucency << 8 | enableTransparency;
    burstLinker.connect(imagePixel, delay, quantizerType, ditherType, transparencyOption, 0, 0);
}

int main(int argc, char *argv[]) {
    const char *fileName = "../screenshot/lenna-original.png";
    int width, height, n;
    unsigned char *data = stbi_load(fileName, &width, &height, &n, 0);
    if (!data) {
        std::cout << "Image load failed" << std::endl;
        stbi_image_free(data);
        return 0;
    }
    if (width >= 65536 || height >= 65536) {
        std::cout << "Image is too large width = " << width << "height = " << height << std::endl;
        stbi_image_free(data);
        return 0;
    }

    BurstLinker burstLinker;
    if (!burstLinker.init("out.gif", width, height, 0, 4)) {
        std::cout << "GifEncoder init fail" << std::endl;
        stbi_image_free(data);
        return 0;
    }

    uint32_t delay = 1000;
    long long currentTime = currentTimeMs();
    bool colorTest = false;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::Uniform, DitherType::No);
    if (colorTest) {
        addImage(255, 255, 255, width, height, delay, burstLinker, QuantizerType::Uniform, DitherType::Bayer);
    }
    long long diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    std::cout << "Uniform " << diff << "ms" << std::endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::MedianCut, DitherType::Bayer);
    if (colorTest) {
        addImage(255, 255, 255, width, height, delay, burstLinker, QuantizerType::MedianCut, DitherType::M2);
    }
    diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    std::cout << "MedianCut " << diff << "ms" << std::endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::KMeans, DitherType::M2);
    if (colorTest) {
        addImage(255, 255, 255, width, height, delay, burstLinker, QuantizerType::KMeans, DitherType::M2);
    }
    diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    std::cout << "KMeans " << diff << "ms" << std::endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::Random, DitherType::FloydSteinberg);
    if (colorTest) {
        addImage(255, 255, 255, width, height, delay, burstLinker, QuantizerType::Random, DitherType::FloydSteinberg);
    }
    diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    std::cout << "Random " << diff << "ms" << std::endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::Octree, DitherType::No);
    if (colorTest) {
        addImage(255, 255, 255, width, height, delay, burstLinker, QuantizerType::Octree, DitherType::FloydSteinberg);
    }
    diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    std::cout << "Octree " << diff << "ms" << std::endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::NeuQuant, DitherType::FloydSteinberg);
    if (colorTest) {
        addImage(255, 255, 255, width, height, delay, burstLinker, QuantizerType::NeuQuant, DitherType::Bayer);
    }
    diff = currentTimeMs() - currentTime;
    std::cout << "NeuQuant -10 " << diff << "ms" << std::endl;

    burstLinker.release();
    stbi_image_free(data);
//    burstLinker.analyzerGifInfo("out.gif");
    return 0;
}
