//
// Created by succlz123 on 17-9-12.
//

#ifndef BURSTLINKER_UNIFORMQUANTIZER_H
#define BURSTLINKER_UNIFORMQUANTIZER_H

#include "ColorQuantizer.h"

namespace blk {

    class UniformQuantizer : public ColorQuantizer {

    public:

        int32_t quantize(const std::vector<ARGB> &in, uint32_t maxColorCount, std::vector<ARGB> &out) override;
    };

}

#endif //BURSTLINKER_UNIFORMQUANTIZER_H
