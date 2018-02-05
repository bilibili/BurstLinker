//
// Created by succlz123 on 2017/11/5.
//

#include <random>
#include "RandomQuantizer.h"

using namespace std;
using namespace blk;

int32_t RandomQuantizer::quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) {
    mt19937 generator((uint32_t) time(nullptr));
    uniform_int_distribution<uint32_t> dis(0, pixelCount);
    set<RGB> randomColor;
    uint32_t index = 0;
    uint32_t maxCount = pixelCount / 4;
    while (randomColor.size() < maxColorCount && index < maxCount) {
        index++;
        uint32_t random = dis(generator);
        randomColor.insert(pixels[random]);
    }
    resultSize = static_cast<int32_t>(randomColor.size());
    int colorPaletteIndex = 0;
    for (RGB color:randomColor) {
        out[colorPaletteIndex].r = color.r;
        out[colorPaletteIndex].g = color.g;
        out[colorPaletteIndex].b = color.b;
        out[colorPaletteIndex].index = static_cast<uint8_t>(colorPaletteIndex);
        colorPaletteIndex++;
    }
    return resultSize;
}
