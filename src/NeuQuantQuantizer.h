//
// Created by succlz123 on 2017/11/5.
//

#ifndef BURSTLINKER_NEUQUANTQUANTIZER_H
#define BURSTLINKER_NEUQUANTQUANTIZER_H

#include "ColorQuantizer.h"

namespace blk {

    class NeuQuantQuantizer : public ColorQuantizer {

    public:

        int32_t quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) override;

    };

}

#endif //BURSTLINKER_NEUQUANTQUANTIZER_H
