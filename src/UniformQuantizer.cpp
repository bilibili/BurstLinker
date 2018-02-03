//
// Created by succlz123 on 17-9-12.
//

#include <cmath>
#include <cstring>
#include <iostream>
#include "UniformQuantizer.h"

using namespace std;

int
UniformQuantizer::quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) {
	size_t index = 0;
    auto baseSegments = static_cast<int>(pow(maxColorCount, 1.0 / 3.0));
    size_t redSegments = baseSegments;
	size_t greenSegments = baseSegments;
	size_t blueSegments = baseSegments;

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

                colorPalette[index++] = static_cast<uint8_t>(r * 255);
                colorPalette[index++] = static_cast<uint8_t>(g * 255);
                colorPalette[index++] = static_cast<uint8_t>(b * 255);
            }
        }
    }
    resultSize = index / 3;
    return resultSize;
}
