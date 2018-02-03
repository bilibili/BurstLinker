//
// Created by succlz123 on 2017/10/14.
//

#ifndef BURSTLINKER_MEDIANCUTQUANTIZER_H
#define BURSTLINKER_MEDIANCUTQUANTIZER_H


#include "ColorQuantizer.h"
#include "GifEncoder.h"
#include <cmath>
#include <algorithm>
#include <utility>
#include <iostream>
#include <queue>

class MedianCutQuantizer : public ColorQuantizer {


public:

    ~MedianCutQuantizer() override;

    int32_t quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) override;


};

#endif //BURSTLINKER_MEDIANCUTQUANTIZER_H
