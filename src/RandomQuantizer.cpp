//
// Created by succlz123 on 2017/11/5.
//

#include <random>
#include "RandomQuantizer.h"

using namespace blk;

int32_t RandomQuantizer::quantize(const std::vector<ARGB> &in, uint32_t maxColorCount, std::vector<ARGB> &out) {
    size_t pixelCount = in.size();
    std::mt19937 generator((uint32_t) time(nullptr));
    std::uniform_int_distribution<uint32_t> dis(0, pixelCount);
    std::set<ARGB> randomColor;
    uint32_t index = 0;
    size_t maxCount = pixelCount / 4;
    while (randomColor.size() < maxColorCount && index < maxCount) {
        index++;
        auto rColor = in[dis(generator)];
        randomColor.emplace(rColor.r, rColor.g, rColor.b);
    }
    resultSize = static_cast<int32_t>(randomColor.size());
    uint8_t colorPaletteIndex = 0;
    for (ARGB color:randomColor) {
        out.emplace_back(color.r, color.g, color.b, colorPaletteIndex);
        colorPaletteIndex++;
    }
    return resultSize;
}
