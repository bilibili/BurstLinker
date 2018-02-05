//
// Created by succlz123 on 17-9-11.
//

#ifndef BURSTLINKER_COLORQUANTIZER_H
#define BURSTLINKER_COLORQUANTIZER_H

#include <set>
#include <cstdint>
#include "GifEncoder.h"

namespace blk {

    class ColorQuantizer {

    public:

        int32_t resultSize = 0;

        virtual ~ColorQuantizer() = default;;

        virtual int32_t
        quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[])= 0;

    protected:

        int sample = 10; // for NeuQuant

    };

}

#endif //BURSTLINKER_COLORQUANTIZER_H
