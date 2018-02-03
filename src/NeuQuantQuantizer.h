//
// Created by succlz123 on 2017/11/5.
//

#ifndef BURSTLINKER_NEUQUANTQUANTIZER_H
#define BURSTLINKER_NEUQUANTQUANTIZER_H


#include "ColorQuantizer.h"

class NeuQuantQuantizer : public ColorQuantizer {

public:

    int32_t quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) override;

};


#endif //BURSTLINKER_NEUQUANTQUANTIZER_H
