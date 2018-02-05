//
// Created by succlz123 on 2017/10/5.
//

#include "DisableDithererWithRs.h"
#include "../../../../../src/Logger.h"
#include "../../../../../src/BayerDitherer.h"
#include "../../../build/generated/source/rs/debug/ScriptC_linear.h"


void DisableDithererWithRs::dither(uint32_t *originalColors, int width, int height,
                                   unsigned char *quantizerColors, int quantizerSize, sp<RS> rs) {
    int32_t size = width * height;
    sp<const Element> element = Element::U32(rs);
    sp<const Type> type = Type::create(rs, element, size, 0, 0);
    sp<Allocation> inputAlloc = Allocation::createTyped(rs, type, RS_ALLOCATION_USAGE_SHARED |
                                                                  RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> outputAlloc = Allocation::createTyped(rs, type, RS_ALLOCATION_USAGE_SHARED |
                                                                   RS_ALLOCATION_USAGE_SCRIPT);
    inputAlloc->copy1DFrom(originalColors);
    ScriptC_linear *sc = new ScriptC_linear(rs);
    sc->set_quantizerSize(quantizerSize * 3);
    sc->set_quantizerColors(quantizerColors);
    sc->forEach_invert(inputAlloc, outputAlloc);
    colorIndices = new uint32_t[size];
    outputAlloc->copy1DTo(colorIndices);
    element.clear();
    type.clear();
    inputAlloc.clear();
    outputAlloc.clear();
}
