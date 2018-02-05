//
// Created by succlz123 on 2017/11/15.
//

#include "BayerDithererWithRs.h"
#include "../../../../../src/Logger.h"
#include "../../../../../src/BayerDitherer.h"
#include "../../../build/generated/source/rs/debug/ScriptC_linear.h"


void BayerDithererWithRs::dither(blk::RGB *originPixels, uint16_t width, uint16_t height,
                                 blk::RGB quantizerPixels[], int32_t quantizerSize,
                                 uint8_t *colorIndices, sp<RS> rs) {
    int32_t size = width * height;
    int bayer[64];
    const int delta = 1 << (5 - bayerScale);
    for (int i = 0; i < 64; i++) {
        bayer[i] = (bayerDitherValue(i) >> 1) - delta;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int position = y * width + x;
            auto rgb = originPixels[position];
            int b = rgb.b;
            int g = rgb.g;
            int r = rgb.r;
            int offset = (bayer[((y & 7) << 3) | (x & 7)]);
            r = (min(255, max(0, r + offset)));
            g = (min(255, max(0, g + offset)));
            b = (min(255, max(0, b + offset)));
            originPixels[position].r = (uint8_t) r;
            originPixels[position].g = (uint8_t) g;
            originPixels[position].b = (uint8_t) b;
        }
    }

    sp<const Element> element = Element::U32(rs);
    sp<const Type> type = Type::create(rs, element, size, 0, 0);
    sp<Allocation> inputAlloc = Allocation::createTyped(rs, type, RS_ALLOCATION_USAGE_SHARED |
                                                                  RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> outputAlloc = Allocation::createTyped(rs, type, RS_ALLOCATION_USAGE_SHARED |
                                                                   RS_ALLOCATION_USAGE_SCRIPT);
    inputAlloc->copy1DFrom(originPixels);
    ScriptC_linear *sc = new ScriptC_linear(rs);
    sc->set_quantizerSize(quantizerSize * 3);
    sc->set_quantizerColors((uint8_t *) quantizerPixels);
    sc->forEach_invert(inputAlloc, outputAlloc);
    outputAlloc->copy1DTo(colorIndices);
    element.clear();
    type.clear();
    inputAlloc.clear();
    outputAlloc.clear();
}
