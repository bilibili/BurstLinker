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

class DisableDithererWithRs : public DithererWithRs {

public:

    void dither(uint32_t *originalColors, int width, int height, unsigned char *quantizerColors,
                int quantizerSize, sp<RS> rs) override;

};


#endif //BURSTLINKER_DISABLEDITHERERWITHRS_H
