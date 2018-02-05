//
// Created by succlz123 on 17-10-9.
//

#ifndef BURSTLINKER_KMEANSQUANTIZER_H
#define BURSTLINKER_KMEANSQUANTIZER_H

#include "ColorQuantizer.h"

namespace blk {

    class KMeansQuantizer : public ColorQuantizer {

    public:

        int32_t quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) override;

    };

}

#endif //BURSTLINKER_KMEANSQUANTIZER_H
