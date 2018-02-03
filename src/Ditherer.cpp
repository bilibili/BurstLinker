//
// Created by succlz123 on 17-9-19.
//

#include "Ditherer.h"

using namespace std;

Ditherer::~Ditherer() {
    delete[] colorIndices;
    colorIndices = nullptr;
    colorQuantizer = nullptr;
}

void Ditherer::getColorIndices(uint32_t *out, int32_t size) {
    memcpy(out, colorIndices, size * sizeof(*colorIndices));
}
