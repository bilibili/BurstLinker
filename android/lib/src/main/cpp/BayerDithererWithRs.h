//
// Created by succlz123 on 2017/11/15.
//

#ifndef BURSTLINKER_BAYERDITHERERWITHRS_H
#define BURSTLINKER_BAYERDITHERERWITHRS_H

#include "../../../../../src/Ditherer.h"
#include "DithererWithRs.h"
#include <RenderScript.h>

using namespace android::RSC;

class BayerDithererWithRs : public DithererWithRs {

public:

    void dither(blk::RGB *originPixels, uint16_t width, uint16_t height,
                blk::RGB quantizerPixels[], int32_t quantizerSize,
                uint8_t *colorIndices, sp<RS> rs) override;

};


#endif //BURSTLINKER_BAYERDITHERERWITHRS_H
