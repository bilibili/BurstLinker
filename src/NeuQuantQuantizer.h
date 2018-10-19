//
// Created by succlz123 on 2017/11/5.
//

#ifndef BURSTLINKER_NEUQUANTQUANTIZER_H
#define BURSTLINKER_NEUQUANTQUANTIZER_H

#include "ColorQuantizer.h"

namespace blk {

    class NeuQuantQuantizer : public ColorQuantizer {

    public:

        int32_t quantize(const std::vector<ARGB> &in, uint32_t maxColorCount, std::vector<ARGB> &out) override;

    };

}

#endif //BURSTLINKER_NEUQUANTQUANTIZER_H
