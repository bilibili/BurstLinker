//
// Created by succlz123 on 2017/11/5.
//

#ifndef BURSTLINKER_RANDOMQUANTIZER_H
#define BURSTLINKER_RANDOMQUANTIZER_H

#include "ColorQuantizer.h"

namespace blk {

    class RandomQuantizer : public ColorQuantizer {

    public:

        int32_t quantize(const std::vector<ARGB> &in, uint32_t maxColorCount, std::vector<ARGB> &out) override;

    };

}

#endif //BURSTLINKER_RANDOMQUANTIZER_H
