//
// Created by succlz123 on 17-10-9.
//

#ifndef BURSTLINKER_KMEANSQUANTIZER_H
#define BURSTLINKER_KMEANSQUANTIZER_H


#include "ColorQuantizer.h"

class KMeansQuantizer : public ColorQuantizer {

public:

    int32_t quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) override;

};


#endif //BURSTLINKER_KMEANSQUANTIZER_H
