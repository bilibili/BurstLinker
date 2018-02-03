//
// Created by succlz123 on 2017/11/15.
//

#include "BayerDithererWithRs.h"
#include "../../../../../src/GifLogger.h"
#include "../../../../../src/BayerDitherer.h"
#include "../../../build/generated/source/rs/debug/ScriptC_linear.h"


void BayerDithererWithRs::dither(uint32_t *originalColors, int width, int height,
                                 unsigned char *quantizerColors, int quantizerSize, sp<RS> rs) {
    int32_t size = width * height;
    int bayer[64];
    const int delta = 1 << (5 - bayerScale);
    for (int i = 0; i < 64; i++) {
        bayer[i] = (bayerDitherValue(i) >> 1) - delta;
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int position = y * width + x;
            uint32_t rgb = originalColors[position];
            int b = (rgb >> 16) & 0xFF;
            int g = (rgb >> 8) & 0xFF;
            int r = (rgb) & 0xFF;
            int offset = (bayer[((y & 7) << 3) | (x & 7)]);
            r = (min(255, max(0, r + offset)));
            g = (min(255, max(0, g + offset)));
            b = (min(255, max(0, b + offset)));
            originalColors[position] = b << 16 | g << 8 | r;
        }
    }

    sp<const Element> element = Element::U32(rs);
    sp<const Type> type = Type::create(rs, element, size, 0, 0);
    sp<Allocation> inputAlloc = Allocation::createTyped(rs, type, RS_ALLOCATION_USAGE_SHARED |
                                                                  RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> outputAlloc = Allocation::createTyped(rs, type, RS_ALLOCATION_USAGE_SHARED |
                                                                   RS_ALLOCATION_USAGE_SCRIPT);
    inputAlloc->copy1DFrom(originalColors);
    ScriptC_linear *sc = new ScriptC_linear(rs);
    sc->set_quantizerSize(quantizerSize*3);
    sc->set_quantizerColors(quantizerColors);
    sc->forEach_invert(inputAlloc, outputAlloc);
    colorIndices = new uint32_t[size];
    outputAlloc->copy1DTo(colorIndices);
    element.clear();
    type.clear();
    inputAlloc.clear();
    outputAlloc.clear();
}
