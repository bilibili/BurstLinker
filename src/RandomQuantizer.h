//
// Created by succlz123 on 2017/11/5.
//

#ifndef BURSTLINKER_RANDOMQUANTIZER_H
#define BURSTLINKER_RANDOMQUANTIZER_H

#include "ColorQuantizer.h"

namespace blk {

    class RandomQuantizer : public ColorQuantizer {

    public:

        int32_t quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) override;

    };

}

#endif //BURSTLINKER_RANDOMQUANTIZER_H
