//
// Created by succlz123 on 17-9-19.
//

#ifndef BURSTLINKER_DITHERER_H
#define BURSTLINKER_DITHERER_H

#include <cstdint>
#include "ColorQuantizer.h"

namespace blk {

    class Ditherer {

    public:

        // ffmpeg vf_paletteuse.c
        static int bayerDitherValue(int p) {
            const int q = p ^(p >> 3);
            return (p & 4) >> 2 | (q & 4) >> 1 \
 | (p & 2) << 1 | (q & 2) << 2 \
 | (p & 1) << 4 | (q & 1) << 5;
        }

        // only for bayer
        int bayerScale = 1;

        virtual ~Ditherer() = default;

        virtual void
        dither(RGB *originPixels, uint16_t width, uint16_t height,
               RGB quantizerPixels[], int32_t quantizerSize,
               uint8_t *colorIndices) = 0;

    };

}

#endif //BURSTLINKER_DITHERER_H
