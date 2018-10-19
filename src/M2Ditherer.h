//
// Created by succlz123 on 17-9-19.
//

#ifndef BURSTLINKER_M2DITHERER_H
#define BURSTLINKER_M2DITHERER_H

#include "Ditherer.h"

namespace blk {

    class M2Ditherer : public Ditherer {

    public:

        void dither(std::vector<ARGB> &origin, std::vector<ARGB> &quantize, uint8_t *colorIndices) override;

    };

}

#endif //BURSTLINKER_M2DITHERER_H
