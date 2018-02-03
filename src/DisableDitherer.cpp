//
// Created by succlz123 on 2017/11/15.
//

#include <algorithm>
#include "DisableDitherer.h"
#include "OctreeQuantizer.h"
#include "KDTree.h"

void
DisableDitherer::dither(uint32_t *originalColors, int width, int height, uint8_t *quantizerColors, int quantizerSize) {
    colorIndices = new uint32_t[width * height];
    if (quantizerType == Octree) {
        static_cast<OctreeQuantizer *>(colorQuantizer)->getColorIndices(originalColors, colorIndices, width * height, nullptr);
        return;
    }

//    int maxCentroidDistance = 255 * 255 * 255;
//    for (uint32_t y = 0; y < height; ++y) {
//        for (uint32_t x = 0; x < width; ++x) {
//            int position = y * width + x;
//            uint32_t rgb = originalColors[position];
//            uint32_t b = (rgb >> 16) & 0xFF;
//            uint32_t g = (rgb >> 8) & 0xFF;
//            uint32_t r = (rgb) & 0xFF;
//            int nearestCentroidDistance = maxCentroidDistance;
//            int index = 0;
//            int nearestIndices = 0;
//            for (int k = 0; k < quantizerSize * 3; k = k + 3) {
//                int qr = quantizerColors[k] & 0xFF;
//                int qg = quantizerColors[k + 1] & 0xFF;
//                int qb = quantizerColors[k + 2] & 0xFF;
//                int nr = r - qr;
//                int ng = g - qg;
//                int nb = b - qb;
//                float distance = 2 * nr * nr + 4 * ng * ng + 3 * nb * nb;
//                if (distance < nearestCentroidDistance) {
//                    nearestCentroidDistance = distance;
//                    nearestIndices = index;
//                };
//                index++;
//            }
//            colorIndices[position] = nearestIndices;
//        }
//    }

    auto **datas = new int *[quantizerSize];
    for (int k = 0; k < quantizerSize * 3; k = k + 3) {
        int qr = quantizerColors[k] & 0xFF;
        int qg = quantizerColors[k + 1] & 0xFF;
        int qb = quantizerColors[k + 2] & 0xFF;
        int index = k / 3;
        datas[index] = new int[4]{qr, qg, qb, index};
    }

    auto *kdTree = new KDTree();
    KDTree::Node *tree = kdTree->createKDTree(datas, quantizerSize, 0);
    int target[3];
    int lastR = 256;
    int lastG = 256;
    int lastB = 256;
    int lastIndex = -1;
    for (int i = 0; i < width * height; ++i) {
        uint32_t rgb = originalColors[i];
        int b = (rgb >> 16) & 0xFF;
        int g = (rgb >> 8) & 0xFF;
        int r = (rgb) & 0xFF;
        if (!(lastR == r && lastG == g && lastB == b)) {
            lastR = r;
            lastG = g;
            lastB = b;
            target[0] = r;
            target[1] = g;
            target[2] = b;
            kdTree->searchNN(tree, target, -1);
            lastIndex = kdTree->nearest.data[3];
        }
        colorIndices[i] = lastIndex;
    }
    kdTree->freeKDTree(tree);
    delete[] datas;
    delete kdTree;
}
