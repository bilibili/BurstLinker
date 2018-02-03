//
// Created by succlz123 on 2017/11/10.
//

#ifndef BURSTLINKER_DITHERERWITHRS_H
#define BURSTLINKER_DITHERERWITHRS_H


#include "../../../../../src/Ditherer.h"

class DithererWithRs : public Ditherer {

public:

    void dither(uint32_t *originalColors, int width, int height, uint8_t *quantizerColors,
                int quantizerSize) override {};

    virtual void
    dither(uint32_t *originalColors, int width, int height, unsigned char *quantizerColors,
           int quantizerSize, sp<RS> rs)=0;

};


#endif //BURSTLINKER_DITHERERWITHRS_H
