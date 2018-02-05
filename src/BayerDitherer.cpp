//
// Created by succlz123 on 2017/10/26.
//

#include "BayerDitherer.h"
#include "KDTree.h"

using namespace std;
using namespace blk;

void BayerDitherer::dither(RGB *originPixels, uint16_t width, uint16_t height,
                           RGB quantizerPixels[], int32_t quantizerSize,
                           uint8_t *colorIndices) {
    int bayer[64];
    const int delta = 1 << (5 - bayerScale);
    for (int i = 0; i < 64; i++) {
        bayer[i] = (bayerDitherValue(i) >> 1) - delta;
    }
    KDTree kdTree;
    KDTree::Node rootNode;
    kdTree.createKDTree(&rootNode, quantizerPixels, 0, quantizerSize - 1, 0);
    RGB target;
    uint8_t lastIndex = 0;
    int position = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto rgb = originPixels[position];
            int offset = (bayer[((y & 7) << 3) | (x & 7)]);
            target.r = static_cast<uint8_t>((min(255, max(0, rgb.r + offset))));
            target.g = static_cast<uint8_t>((min(255, max(0, rgb.g + offset))));
            target.b = static_cast<uint8_t>((min(255, max(0, rgb.b + offset))));
            kdTree.searchNNNoBacktracking(&rootNode, target, -1);
            lastIndex = kdTree.nearest.index;
            colorIndices[position] = lastIndex;
            position++;
        }
    }
    kdTree.freeKDTree(&rootNode);
}
