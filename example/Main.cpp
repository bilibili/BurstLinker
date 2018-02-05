//
// Created by succlz123 on 17-8-7.
//

#include <iostream>
#include "../src/BurstLinker.h"

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

void addImage(const char *fileName, uint32_t width, uint32_t height, int delay, BurstLinker &burstLinker,
              QuantizerType quantizerType, DitherType ditherType) {
    uint32_t imageSize = width * height;
    if (imageSize < width || imageSize < height) {
        return;
    }
    int w, h, n;
    unsigned char *data = stbi_load(fileName, &w, &h, &n, 0);
    if (w != width || h != height) {
        cout << "Image is not the same width or height" << endl;
        return;
    }
    auto imagePixel = new uint32_t[imageSize];
    uint32_t index = 0;
    uint32_t pixelIndex = 0;
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
                index++;
            } else {
                return;
            }
            int bgr = b << 16 | g << 8 | r;
            imagePixel[pixelIndex++] = static_cast<uint32_t>(bgr);
        }
    }
    stbi_image_free(data);
    burstLinker.connect(imagePixel, delay, quantizerType, ditherType, 0, 0);
}

void
addImage(int r, int g, int b, uint32_t width, uint32_t height, int delay, BurstLinker &burstLinker,
         QuantizerType quantizerType, DitherType ditherType) {
    uint32_t imageSize = width * height;
    if (imageSize < width || imageSize < height) {
        return;
    }
    auto *imagePixel = new uint32_t[imageSize];
    int pixelIndex = 0;
    for (uint32_t i = 0; i < width; i++) {
        for (uint32_t j = 0; j < height; j++) {
            int rgb = b << 16 | g << 8 | r;
            imagePixel[pixelIndex++] = static_cast<uint32_t>(rgb);
        }
    }
    burstLinker.connect(imagePixel, delay, quantizerType, ditherType, 0, 0);
}

int main(int argc, char *argv[]) {
    const char *fileName = "1.jpg";

    int width, height, n;
    unsigned char *data = stbi_load("1.jpg", &width, &height, &n, 0);

    if (!data) {
        cout << "Image load failed" << endl;
        stbi_image_free(data);
        return 0;
    }
    int imageSize = width * height;
    if (width >= 65536 || height >= 65536) {
        cout << "Image is too large " << imageSize << endl;
        stbi_image_free(data);
        return 0;
    }

    BurstLinker burstLinker;
    if (!burstLinker.init("out.gif", width, height, 0, 4)) {
        cout << "GifEncoder init fail" << endl;
        stbi_image_free(data);
        return 0;
    }

    int delay = 1000;

    long long currentTime = currentTimeMs();

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::Uniform, DitherType::NO);
//    addImage(255, 255, 255, width, height, delay burstLinker,QuantizerType::Uniform, DitherType::Bayer);

    long long diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    cout << "Uniform " << diff << "ms" << endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::MedianCut, DitherType::NO);
//    addImage(255, 255, 255, width, height,delay, burstLinker,  QuantizerType::MedianCut, DitherType::M2);

    diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    cout << "MedianCut " << diff << "ms" << endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::KMeans, DitherType::NO);
//    addImage(255, 255, 255, width, height, delay,burstLinker, QuantizerType::KMeans, DitherType::M2);

    diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    cout << "KMeans " << diff << "ms" << endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::Random, DitherType::NO);
//    addImage(255, 255, 255, width, height,delay, burstLinker, QuantizerType::Random, DitherType::FloydSteinberg);

    diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    cout << "Random " << diff << "ms" << endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::Octree, DitherType::NO);
//    addImage(255, 255, 255, width, height, delay,burstLinker, QuantizerType::Octree, DitherType::FloydSteinberg);

    diff = currentTimeMs() - currentTime;
    currentTime = currentTimeMs();
    cout << "Octree " << diff << "ms" << endl;

    addImage(fileName, width, height, delay, burstLinker, QuantizerType::NeuQuant, DitherType::NO);
//    addImage(255, 255, 255, width, height,delay, burstLinker, QuantizerType::NeuQuant, DitherType::Bayer);

    diff = currentTimeMs() - currentTime;
    cout << "NeuQuant -10 " << diff << "ms" << endl;

//    vector<uint32_t *> imagePixels;
//    for (int i = startPosition; i < argc; ++i) {
//        const char *processFileName = fileName;
//        fipImage processImage;
//        processImage.load(processFileName);
//        size_t processWidth = processImage.getWidth();
//        size_t processHeight = processImage.getHeight();
//        if (processWidth != width || processHeight != height) {
//            cout << "Image is not the same width or height" << endl;
//            releaseImage(processImage, true);
//            return 0;
//        }
//        if (imageSize < width || imageSize < height) {
//            cout << "C6386" << endl;
//            releaseImage(processImage, true);
//            return 0;
//        }
//        auto *imagePixel = new uint32_t[imageSize];
//        memset(imagePixel, 0, imageSize * sizeof(uint32_t));
//        int pixelIndex = 0;
//        RGBQUAD color{};
//        for (uint32_t i = 0; i < height; i++) {
//            for (uint32_t j = 0; j < width; j++) {
//                processImage.getPixelColor(j, height - 1 - i, &color);
//                int r = color.rgbRed & 0xFF;
//                int g = color.rgbGreen & 0xFF;
//                int b = color.rgbBlue & 0xFF;
//                int bgr = b << 16 | g << 8 | r;
//                imagePixel[pixelIndex++] = static_cast<uint32_t>(bgr);
//            }
//        }
//        processImage.clear();
//        FreeImage_Unload(processImage);
//        imagePixels.emplace_back(imagePixel);
//    }
//    QuantizerType quantizerType = QuantizerType::NeuQuant;
//    DitherType ditherType = DitherType::FloydSteinberg;
//    burstLinker.connect(imagePixels, delay, quantizerType, ditherType, 0, 0);

    burstLinker.release();

    stbi_image_free(data);
//    burstLinker.analyzerGifInfo("out.gif");

    return 0;
}
