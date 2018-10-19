//
// Created by succlz123 on 2017/11/15.
//

#include <algorithm>
#include <iostream>
#include "NoDitherer.h"
#include "KDTree.h"

using namespace blk;

void NoDitherer::dither(std::vector<ARGB> &origin, std::vector<ARGB> &quantize, uint8_t *colorIndices) {
    size_t totalSize = width * height;
    size_t size = origin.size();
    size_t quantizeSize = quantize.size();

//    int maxCentroidDistance = 255 * 255 * 255;
//    for (int i = 0; i < size; ++i) {
//        auto rgb = origin[i];
//        uint8_t r = rgb.r;
//        uint8_t g = rgb.g;
//        uint8_t b = rgb.b;
//        double nearestCentroidDistance = maxCentroidDistance;
//        int nearestIndices = 0;
//        for (int k = 0; k < quantizeSize; ++k) {
//            uint8_t qr = quantize[k].r;
//            uint8_t qg = quantize[k].g;
//            uint8_t qb = quantize[k].b;
//            int16_t nr = qr - r;
//            int16_t ng = qg - g;
//            int16_t nb = qb - b;
//            auto distance = nr * nr + ng * ng + nb * nb;
//            if (distance < nearestCentroidDistance) {
//                nearestCentroidDistance = distance;
//                nearestIndices = quantize[k].index;
//            };
//        }
//        colorIndices[i] = static_cast<uint8_t>(nearestIndices);
//    }

    KDTree kdTree;
    KDTree::Node rootNode;
    auto end = static_cast<uint8_t>(quantizeSize - 1);
    auto transparentColorIndex = static_cast<uint8_t>(quantizeSize + 1);
    kdTree.createKDTree(&rootNode, quantize, 0, end, 0);
    uint16_t lastR = 256;
    uint16_t lastG = 256;
    uint16_t lastB = 256;
    uint8_t lastIndex = 0;
    int count = 0;
    for (int i = 0; i < size; ++count) {
        auto rgb = origin[i];
        if (rgb.unTranpsparentIndex == count) {
            ++i;
            if (!(lastR == rgb.r && lastG == rgb.g && lastB == rgb.b)) {
                lastR = rgb.r;
                lastG = rgb.g;
                lastB = rgb.b;
                kdTree.searchNoBacktracking(&rootNode, rgb.r, rgb.g, rgb.b, -1);
                lastIndex = kdTree.nearest.index;
            }
            colorIndices[count] = lastIndex;
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
