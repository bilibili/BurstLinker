//
// Created by succlz123 on 2017/11/5.
//

#ifndef BURSTLINKER_RANDOMQUANTIZER_H
#define BURSTLINKER_RANDOMQUANTIZER_H


#include "ColorQuantizer.h"

class RandomQuantizer : public ColorQuantizer {

public:

    int32_t quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) override;

};


#endif //BURSTLINKER_RANDOMQUANTIZER_H
