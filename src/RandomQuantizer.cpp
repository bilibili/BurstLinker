//
// Created by succlz123 on 2017/11/5.
//

#include <random>
#include <ctime>
#include "RandomQuantizer.h"

using namespace std;

int32_t RandomQuantizer::quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) {
    mt19937 generator((uint32_t)time(nullptr));
    uniform_int_distribution<uint32_t> dis(0, pixelCount);
    set<uint32_t> randomColor;
	uint32_t index = 0;
	uint32_t maxCount = pixelCount / 4;
    while (randomColor.size() < maxColorCount && index < maxCount) {
		index++;
        uint32_t random = dis(generator);
        randomColor.insert(originalColors[random]);
    }
    resultSize = randomColor.size();
    int colorPaletteIndex = 0;
    for (uint32_t color:randomColor) {
        colorPalette[colorPaletteIndex++] = color & 0xFF;
        colorPalette[colorPaletteIndex++] = color >> 8 & 0xFF;
        colorPalette[colorPaletteIndex++] = color >> 16 & 0xFF;
    }
    return resultSize;
}
