//
// Created by succlz123 on 2017/10/5.
//

#ifndef BURSTLINKER_DISABLEDITHERERWITHRS_H
#define BURSTLINKER_DISABLEDITHERERWITHRS_H

#include "../../../../../src/Ditherer.h"
#include "DithererWithRs.h"
#include <RenderScript.h>
#include <vector>

using namespace android::RSC;
using namespace std;

class NoDithererWithRs : public DithererWithRs {

public:

    void dither(blk::RGB *originPixels, uint16_t width, uint16_t height,
                blk::RGB quantizerPixels[], int32_t quantizerSize,
                uint8_t *colorIndices, sp<RS> rs) override;

};


#endif //BURSTLINKER_DISABLEDITHERERWITHRS_H
