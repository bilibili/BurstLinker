//
// Created by succlz123 on 17-9-12.
//

#include <cmath>
#include <cstring>
#include <iostream>
#include "UniformQuantizer.h"

using namespace blk;

int32_t UniformQuantizer::quantize(const std::vector<ARGB> &in, uint32_t maxColorCount, std::vector<ARGB> &out) {
    uint32_t index = 0;
    auto baseSegments = static_cast<int>(pow(maxColorCount, 1.0 / 3.0));
    int32_t redSegments = baseSegments;
    int32_t greenSegments = baseSegments;
    int32_t blueSegments = baseSegments;
    if (redSegments * (greenSegments + 1) * blueSegments <= maxColorCount) {
        ++greenSegments;
    }
    if ((redSegments + 1) * greenSegments * blueSegments <= maxColorCount) {
        ++redSegments;
    }
    for (size_t redSegment = 0; redSegment < redSegments; ++redSegment) {
        for (size_t greenSegment = 0; greenSegment < greenSegments; ++greenSegment) {
            for (size_t blueSegment = 0; blueSegment < blueSegments; ++blueSegment) {
                double dr = redSegment / (redSegments - 1.0);
                double dg = greenSegment / (greenSegments - 1.0);
                double db = blueSegment / (blueSegments - 1.0);
                auto r = static_cast<uint8_t>(dr * 255);
                auto g = static_cast<uint8_t>(dg * 255);
                auto b = static_cast<uint8_t>(db * 255);
                out.emplace_back(r, g, b, index);
                index++;
            }
        }
    }
    resultSize = index;
    return resultSize;
}
