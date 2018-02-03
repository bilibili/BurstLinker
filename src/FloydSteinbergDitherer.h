//
// Created by succlz123 on 17-9-19.
//

#ifndef BURSTLINKER_FLOYDSTEINBERGDITHERER_H
#define BURSTLINKER_FLOYDSTEINBERGDITHERER_H

#include "Ditherer.h"

class FloydSteinbergDitherer : public Ditherer {

public:

    void
    dither(uint32_t *originalColors, int width, int height,
           uint8_t *quantizerColors, int quantizerSize) override;

};


#endif //BURSTLINKER_FLOYDSTEINBERGDITHERER_H
