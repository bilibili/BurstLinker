//
// Created by succlz123 on 2017/11/15.
//

#include <algorithm>
#include "NoDitherer.h"
#include "KDTree.h"

using namespace blk;

void
NoDitherer::dither(RGB *originPixels, uint16_t width, uint16_t height,
                   RGB quantizerPixels[], int32_t quantizerSize,
                   uint8_t *colorIndices) {
    size_t size = width * height;

//    int32_t maxCentroidDistance = 255 * 255 * 255;
//    for (int i = 0; i < size; ++i) {
//        auto rgb = originPixels[i];
//        uint8_t r = rgb.r;
//        uint8_t g = rgb.g;
//        uint8_t b = rgb.b;
//        int nearestCentroidDistance = maxCentroidDistance;
//        int nearestIndices = 0;
//        for (int k = 0; k < quantizerSize; ++k) {
//            uint8_t qr = quantizerPixels[k].r;
//            uint8_t qg = quantizerPixels[k].g;
//            uint8_t qb = quantizerPixels[k].b;
//            int16_t nr = r - qr;
//            int16_t ng = g - qg;
//            int16_t nb = b - qb;
//            int32_t distance = 2 * nr * nr + 4 * ng * ng + 3 * nb * nb;
//            if (distance < nearestCentroidDistance) {
//                nearestCentroidDistance = distance;
//                nearestIndices = quantizerPixels[k].index;
//            };
//        }
//        colorIndices[i] = static_cast<uint8_t>(nearestIndices);
//    }

    KDTree kdTree;
    KDTree::Node rootNode;
    kdTree.createKDTree(&rootNode, quantizerPixels, 0, quantizerSize - 1, 0);
    uint16_t lastR = 256;
    uint16_t lastG = 256;
    uint16_t lastB = 256;
    uint8_t lastIndex = 0;
    for (int i = 0; i < size; ++i) {
        auto rgb = originPixels[i];
        if (!(lastR == rgb.r && lastG == rgb.g && lastB == rgb.b)) {
            lastR = rgb.r;
            lastG = rgb.g;
            lastB = rgb.b;
            kdTree.searchNNNoBacktracking(&rootNode, rgb, -1);
            lastIndex = kdTree.nearest.index;
        }
        colorIndices[i] = lastIndex;
    }
    kdTree.freeKDTree(&rootNode);
}
