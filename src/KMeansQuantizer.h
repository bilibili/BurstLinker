//
// Created by succlz123 on 17-10-9.
//

#ifndef BURSTLINKER_KMEANSQUANTIZER_H
#define BURSTLINKER_KMEANSQUANTIZER_H

#include "ColorQuantizer.h"

namespace blk {

    class KMeansQuantizer : public ColorQuantizer {

    public:

        int32_t quantize(const std::vector<ARGB> &in, uint32_t maxColorCount, std::vector<ARGB> &out) override;

    };

}

#endif //BURSTLINKER_KMEANSQUANTIZER_H
