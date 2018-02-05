//
// Created by succlz123 on 2017/11/5.
//

#include "NeuQuantQuantizer.h"
#include "NeuQuant.h"

using namespace blk;

int32_t
NeuQuantQuantizer::quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) {
    NeuQuant neuQuant;
    neuQuant.initnet(reinterpret_cast<uint8_t *>(pixels), pixelCount * 3, sample);
    neuQuant.learn();
    neuQuant.unbiasnet();
    resultSize = neuQuant.getColourMap(out);
    return resultSize;
}
