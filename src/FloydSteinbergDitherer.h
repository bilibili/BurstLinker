//
// Created by succlz123 on 17-9-19.
//

#ifndef BURSTLINKER_FLOYDSTEINBERGDITHERER_H
#define BURSTLINKER_FLOYDSTEINBERGDITHERER_H

#include "Ditherer.h"

namespace blk {

    class FloydSteinbergDitherer : public Ditherer {

    public:

        void dither(std::vector<ARGB> &origin, std::vector<ARGB> &quantize, uint8_t *colorIndices) override;

    };

}

#endif //BURSTLINKER_FLOYDSTEINBERGDITHERER_H
