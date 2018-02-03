//
// Created by succlz123 on 17-9-19.
//

#ifndef BURSTLINKER_M2DITHERER_H
#define BURSTLINKER_M2DITHERER_H

#include "Ditherer.h"

class M2Ditherer : public Ditherer {

public:

    void
    dither(uint32_t *originalColors, int width, int height,
           uint8_t *quantizerColors, int quantizerSize) override;

};


#endif //BURSTLINKER_M2DITHERER_H
