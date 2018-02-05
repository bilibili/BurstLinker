//
// Created by succlz123 on 2017/10/26.
//

#include "M2Ditherer.h"
#include "KDTree.h"

using namespace std;
using namespace blk;

static const uint8_t matrix4x4[4][4] = {
        {0,  8,  2,  10},
        {12, 4,  14, 6},
        {3,  11, 1,  9},
        {15, 7,  13, 5}
};

void M2Ditherer::dither(RGB *originPixels, uint16_t width, uint16_t height,
                        RGB quantizerPixels[], int32_t quantizerSize,
                        uint8_t *colorIndices) {
    KDTree kdTree;
    KDTree::Node rootNode;
    kdTree.createKDTree(&rootNode, quantizerPixels, 0, quantizerSize - 1, 0);
    RGB target;
    uint8_t lastIndex = 0;
    int position = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto rgb = originPixels[position];
            int offset = (matrix4x4[(x & 3)][y & 3]);
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
