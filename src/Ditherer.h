//
// Created by succlz123 on 17-9-19.
//

#ifndef BURSTLINKER_DITHERER_H
#define BURSTLINKER_DITHERER_H

#include <cstdint>
#include "ColorQuantizer.h"
#include "GifEncoder.h"

#if defined(__Android__)

#include <RenderScript.h>

using namespace android::RSC;
#endif

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

    virtual ~Ditherer();

    uint32_t *colorIndices = nullptr;

    ColorQuantizer *colorQuantizer;

    QuantizerType quantizerType;

    void getColorIndices(uint32_t *out, int32_t size);

    virtual void
    dither(uint32_t *originalColors, int width, int height,
           uint8_t *quantizerColors, int quantizerSize) = 0;

};


#endif //BURSTLINKER_DITHERER_H
