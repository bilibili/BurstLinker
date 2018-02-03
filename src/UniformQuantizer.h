//
// Created by succlz123 on 17-9-12.
//

#ifndef BURSTLINKER_UNIFORMQUANTIZER_H
#define BURSTLINKER_UNIFORMQUANTIZER_H


#include "ColorQuantizer.h"

class UniformQuantizer : public ColorQuantizer {

public:

    int quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) override;
};


#endif //BURSTLINKER_UNIFORMQUANTIZER_H
