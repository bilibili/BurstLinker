//
// Created by succlz123 on 17-9-11.
//

#ifndef BURSTLINKER_COLORQUANTIZER_H
#define BURSTLINKER_COLORQUANTIZER_H

#include <set>
#include <cstdint>

using namespace std;

class ColorQuantizer {

public:

    int width = 0;

    int height = 0;

    virtual ~ColorQuantizer();

    virtual int32_t
    quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount)= 0;

    void getColorPalette(uint8_t out[]);

protected:

    int resultSize = 0;

    int sample = 10; // for NeuQuant

    uint8_t colorPalette[256 * 3]{'\0'};
};


#endif //BURSTLINKER_COLORQUANTIZER_H
