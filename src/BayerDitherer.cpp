//
// Created by succlz123 on 2017/10/26.
//

#include <cmath>
#include <iostream>
#include "BayerDitherer.h"
#include "KDTree.h"

using namespace std;

int bayer[64];

void BayerDitherer::dither(uint32_t *originalColors, int width, int height,
                           uint8_t *quantizerColors, int quantizerSize) {
    colorIndices = new uint32_t[width * height];

    const int delta = 1 << (5 - bayerScale);
    for (int i = 0; i < 64; i++) {
        bayer[i] = (bayerDitherValue(i) >> 1) - delta;
    }

//    if (quantizerType == Octree) {
//        auto getOffset = [](int x, int y) { return bayer[((y & 7) << 3) | (x & 7)]; };
//        static_cast<OctreeQuantizer *>(colorQuantizer)->getColorIndices(originalColors, colorIndices, width * height,
//                                                                        getOffset);
//        return;
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
    int lastIndex = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int position = y * width + x;
            uint32_t rgb = originalColors[position];
            int b = (rgb >> 16) & 0xFF;
            int g = (rgb >> 8) & 0xFF;
            int r = (rgb) & 0xFF;
            int offset = (bayer[((y & 7) << 3) | (x & 7)]);
            r = (min(255, max(0, r + offset)));
            g = (min(255, max(0, g + offset)));
            b = (min(255, max(0, b + offset)));
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
            colorIndices[position] = lastIndex;
        }
    }
    kdTree->freeKDTree(tree);
    delete[] datas;
    delete kdTree;
}
