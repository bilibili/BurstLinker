//
// Created by succlz123 on 2017/10/26.
//

#ifndef BURSTLINKER_BAYERDITHERER_H
#define BURSTLINKER_BAYERDITHERER_H


#include <vector>
#include "Ditherer.h"

using namespace std;


class BayerDitherer : public Ditherer {

public:

    void
    dither(uint32_t *originalColors, int width, int height,
           uint8_t *quantizerColors, int quantizerSize) override;
};


#endif //BURSTLINKER_BAYERDITHERER_H
