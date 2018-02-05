//
// Created by succlz123 on 17-9-12.
//

#include <cmath>
#include <cstring>
#include <iostream>
#include "UniformQuantizer.h"

using namespace std;
using namespace blk;

int32_t
UniformQuantizer::quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) {
    uint32_t index = 0;
    auto baseSegments = static_cast<int32_t>(pow(maxColorCount, 1.0 / 3.0));
    int32_t redSegments = baseSegments;
    int32_t greenSegments = baseSegments;
    int32_t blueSegments = baseSegments;

    // see if we can add an extra segment to one or two channels.
    if (redSegments * (greenSegments + 1) * blueSegments <= maxColorCount) {
        ++greenSegments;
    }
    if ((redSegments + 1) * greenSegments * blueSegments <= maxColorCount) {
        ++redSegments;
    }

    for (size_t redSegment = 0; redSegment < redSegments; ++redSegment) {
        for (size_t greenSegment = 0; greenSegment < greenSegments; ++greenSegment) {
            for (size_t blueSegment = 0; blueSegment < blueSegments; ++blueSegment) {
                double r = redSegment / (redSegments - 1.0);
                double g = greenSegment / (greenSegments - 1.0);
                double b = blueSegment / (blueSegments - 1.0);
                out[index].r = static_cast<uint8_t>(r * 255);
                out[index].g = static_cast<uint8_t>(g * 255);
                out[index].b = static_cast<uint8_t>(b * 255);
                out[index].index = static_cast<uint8_t>(index);
                index++;
            }
        }
    }
    resultSize = index;
    return resultSize;
}
