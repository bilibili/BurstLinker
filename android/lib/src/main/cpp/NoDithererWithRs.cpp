//
// Created by succlz123 on 2017/10/5.
//

#include "NoDithererWithRs.h"
#include "../../../../../src/Logger.h"
#include "../../../../../src/BayerDitherer.h"
#include "../../../build/generated/source/rs/debug/ScriptC_linear.h"

void NoDithererWithRs::dither(blk::RGB *originPixels, uint16_t width, uint16_t height,
                              blk::RGB quantizerPixels[], int32_t quantizerSize,
                              uint8_t *colorIndices, sp<RS> rs) {
    int32_t size = width * height;
    sp<const Element> element = Element::U32(rs);
    sp<const Type> type = Type::create(rs, element, size, 0, 0);
    sp<Allocation> inputAlloc = Allocation::createTyped(rs, type, RS_ALLOCATION_USAGE_SHARED |
                                                                  RS_ALLOCATION_USAGE_SCRIPT);
    sp<Allocation> outputAlloc = Allocation::createTyped(rs, type, RS_ALLOCATION_USAGE_SHARED |
                                                                   RS_ALLOCATION_USAGE_SCRIPT);
    inputAlloc->copy1DFrom(originPixels);
    ScriptC_linear *sc = new ScriptC_linear(rs);
    sc->set_quantizerSize(quantizerSize);
    sc->set_quantizerColors((uint8_t *) quantizerPixels);
    sc->forEach_invert(inputAlloc, outputAlloc);
    outputAlloc->copy1DTo(colorIndices);
    element.clear();
    type.clear();
    inputAlloc.clear();
    outputAlloc.clear();
}
