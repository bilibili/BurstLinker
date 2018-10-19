//
// Created by succlz123 on 2017/11/15.
//

#ifndef BURSTLINKER_NODITHERER_H
#define BURSTLINKER_NODITHERER_H

#include "Ditherer.h"

namespace blk {

    class NoDitherer : public Ditherer {

    public:

        void dither(std::vector<ARGB> &origin, std::vector<ARGB> &quantize, uint8_t *colorIndices) override;

    };

}

#endif //BURSTLINKER_NODITHERER_H
