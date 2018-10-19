//
// Created by succlz123 on 2017/10/26.
//

#ifndef BURSTLINKER_BAYERDITHERER_H
#define BURSTLINKER_BAYERDITHERER_H

#include "Ditherer.h"

namespace blk {

    class BayerDitherer : public Ditherer {

    public:

        void dither(std::vector<ARGB> &origin, std::vector<ARGB> &quantize, uint8_t *colorIndices) override;
    };

}

#endif //BURSTLINKER_BAYERDITHERER_H
