//
// Created by succlz123 on 2017/11/15.
//

#ifndef BURSTLINKER_BAYERDITHERERWITHRS_H
#define BURSTLINKER_BAYERDITHERERWITHRS_H

#include "../../../../../src/Ditherer.h"
#include "DithererWithRs.h"
#include <RenderScript.h>

using namespace android::RSC;
using namespace std;

class BayerDithererWithRs : public DithererWithRs {

public:

    void dither(uint32_t *originalColors, int width, int height, unsigned char *quantizerColors,
                int quantizerSize, sp<RS> rs) override;

};


#endif //BURSTLINKER_BAYERDITHERERWITHRS_H
