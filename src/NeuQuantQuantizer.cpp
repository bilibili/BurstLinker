//
// Created by succlz123 on 2017/11/5.
//

#include "NeuQuantQuantizer.h"
#include "NeuQuant.h"

using namespace blk;

int32_t NeuQuantQuantizer::quantize(const std::vector<ARGB> &in, uint32_t maxColorCount, std::vector<ARGB> &out) {
    NeuQuant neuQuant;
    size_t size = in.size();
    auto pixels = new uint8_t[size * 3];
    int index = 0;
    for (int i = 0; i < size; ++i) {
        auto inColor = in[i];
        pixels[index++] = inColor.r;
        pixels[index++] = inColor.g;
        pixels[index++] = inColor.b;
    }
    neuQuant.initnet(pixels, size * 3, sample);
    neuQuant.learn();
    neuQuant.unbiasnet();
    resultSize = neuQuant.getColourMap(out, maxColorCount);
    delete[] pixels;
    return resultSize;
}
