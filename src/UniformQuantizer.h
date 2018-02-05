//
// Created by succlz123 on 17-9-12.
//

#ifndef BURSTLINKER_UNIFORMQUANTIZER_H
#define BURSTLINKER_UNIFORMQUANTIZER_H

#include "ColorQuantizer.h"

namespace blk {

    class UniformQuantizer : public ColorQuantizer {

    public:

        int32_t quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) override;
    };

}

#endif //BURSTLINKER_UNIFORMQUANTIZER_H
