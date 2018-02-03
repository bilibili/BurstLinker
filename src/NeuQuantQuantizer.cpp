//
// Created by succlz123 on 2017/11/5.
//

#include "NeuQuantQuantizer.h"
#include "NeuQuant.h"

int32_t NeuQuantQuantizer::quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) {
    initnet(reinterpret_cast<uint8_t *>(originalColors), pixelCount * 3, sample);
    learn();
    unbiasnet();
    resultSize = getColourMap(colorPalette);
    return resultSize;
}
