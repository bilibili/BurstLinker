//
// Created by succlz123 on 17-9-11.
//

#include <cstring>
#include "ColorQuantizer.h"

ColorQuantizer::~ColorQuantizer() {
}

void ColorQuantizer::getColorPalette(unsigned char *out) {
    memcpy(out, colorPalette, resultSize * 3 * sizeof(*colorPalette));
}
