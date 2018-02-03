//
// Created by succlz123 on 2017/11/15.
//

#ifndef BURSTLINKER_DISABLEDITHERER_H
#define BURSTLINKER_DISABLEDITHERER_H


#include "Ditherer.h"

using namespace std;


class DisableDitherer : public Ditherer {

public:

    void
    dither(uint32_t *originalColors, int width, int height,
           uint8_t *quantizerColors, int quantizerSize);

};

#endif //BURSTLINKER_DISABLEDITHERER_H
