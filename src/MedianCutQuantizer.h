//
// Created by succlz123 on 2017/10/14.
//

#ifndef BURSTLINKER_MEDIANCUTQUANTIZER_H
#define BURSTLINKER_MEDIANCUTQUANTIZER_H

#include "ColorQuantizer.h"

namespace blk {

    class MedianCutQuantizer : public ColorQuantizer {

    public:

        int32_t quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) override;

    };

}

#endif //BURSTLINKER_MEDIANCUTQUANTIZER_H
