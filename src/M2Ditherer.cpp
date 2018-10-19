//
// Created by succlz123 on 2017/10/26.
//

#include <cmath>
#include "M2Ditherer.h"
#include "KDTree.h"

using namespace blk;

static const uint8_t matrix4x4[4][4] = {
        {0,  8,  2,  10},
        {12, 4,  14, 6},
        {3,  11, 1,  9},
        {15, 7,  13, 5}
};

void M2Ditherer::dither(std::vector<ARGB> &origin, std::vector<ARGB> &quantize, uint8_t *colorIndices) {
    size_t totalSize = width * height;
    size_t size = origin.size();
    KDTree kdTree;
    KDTree::Node rootNode;
    size_t quantizeSize = quantize.size();
    auto end = static_cast<uint8_t>(quantizeSize - 1);
    auto transparentColorIndex = static_cast<uint8_t>(quantizeSize + 1);
    kdTree.createKDTree(&rootNode, quantize, 0, end, 0);
    int count = 0;
    for (int i = 0; i < size; ++count) {
        auto rgb = origin[i];
        if (rgb.unTranpsparentIndex == count) {
            ++i;
            int x = i % width;
            auto y = static_cast<int>(std::ceil(i / width));
            int offset = (matrix4x4[(x & 3)][y & 3]);
            auto r = static_cast<uint8_t>(std::min(255, std::max(0, rgb.r + offset)));
            auto g = static_cast<uint8_t>(std::min(255, std::max(0, rgb.g + offset)));
            auto b = static_cast<uint8_t>(std::min(255, std::max(0, rgb.b + offset)));
            kdTree.searchNoBacktracking(&rootNode, r, g, b, -1);
            colorIndices[count] = kdTree.nearest.index;
        } else {
            colorIndices[count] = transparentColorIndex;
        }
    }
    if (count < totalSize) {
        for (int i = count; i < totalSize; ++i) {
            colorIndices[i] = transparentColorIndex;
        }
    }
    kdTree.freeKDTree(&rootNode);
}
