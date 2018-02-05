//
// Created by succlz123 on 17-10-9.
//

#include <algorithm>
#include <random>
#include <map>
#include <cstring>
#include "KMeansQuantizer.h"
#include "KDTree.h"

using namespace std;
using namespace blk;

int32_t
KMeansQuantizer::quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) {
    if (pixelCount == 0) {
        return 0;
    }
    // random initial center
    mt19937 generator((uint32_t) time(nullptr));
    uniform_int_distribution<uint32_t> dis(0, pixelCount);
    set<RGB> centroidsToRecompute;
    uint32_t randomCount = 0;
    while (centroidsToRecompute.size() < maxColorCount) {
        uint32_t random = dis(generator);
        centroidsToRecompute.insert(pixels[random]);
        if (randomCount++ > pixelCount) {
            break;
        }
    }

    int colorPaletteIndex = 0;
    auto centroidSize = static_cast<uint32_t>(centroidsToRecompute.size());
    if (centroidSize < maxColorCount) {
        resultSize = centroidSize;
        for (RGB color : centroidsToRecompute) {
            out[colorPaletteIndex].r = color.r;
            out[colorPaletteIndex].g = color.g;
            out[colorPaletteIndex].b = color.b;
            out[colorPaletteIndex].index = static_cast<uint8_t>(colorPaletteIndex);
            colorPaletteIndex++;
        }
        return centroidSize;
    }

    auto **means = new uint32_t *[maxColorCount];
    for (uint32_t m = 0; m < maxColorCount; ++m) {
        auto *current = new uint32_t[3]{0};
        means[m] = current;
    }
    int currentMeans = 0;
    for (auto color : centroidsToRecompute) {
        means[currentMeans][0] = color.r;
        means[currentMeans][1] = color.g;
        means[currentMeans][2] = color.b;
        currentMeans++;
    }

    // recursion
    RGB rgb;
    int label = 0;
    int iterateNum = 0;
    int lastCost = 0;
    int currCost = 0;
    int unchanged = 0;
    bool loop = true;
    auto *counts = new uint32_t[maxColorCount];
    auto **nextMeans = new uint32_t *[maxColorCount];
    for (uint32_t i = 0; i < maxColorCount; i++) {
        nextMeans[i] = new uint32_t[3];
    }

    while (loop) {
        // init
        memset(counts, 0, sizeof(int) * maxColorCount);
        for (size_t i = 0; i < maxColorCount; i++) {
            memset(nextMeans[i], 0, sizeof(int) * 3);
        }
        lastCost = currCost;
        currCost = 0;

        // classification
        {
            KDTree kdTree;
            unique_ptr<RGB[]> datas(new RGB[pixelCount]);
            for (uint32_t l = 0; l < maxColorCount; ++l) {
                auto color = means[l];
                datas[l].r = static_cast<uint8_t>(color[0]);
                datas[l].g = static_cast<uint8_t>(color[1]);
                datas[l].b = static_cast<uint8_t>(color[2]);
                datas[l].index = static_cast<uint8_t>(l);
            }
            KDTree::Node rootNode;
            kdTree.createKDTree(&rootNode, datas.get(), 0, maxColorCount - 1, 0);
            for (uint32_t i = 0; i < pixelCount; i++) {
                rgb = pixels[i];
                currCost += kdTree.searchNNNoBacktracking(&rootNode, rgb, -1);
                label = kdTree.nearest.index;
                counts[label]++;
                nextMeans[label][0] += rgb.r;
                nextMeans[label][1] += rgb.g;
                nextMeans[label][2] += rgb.b;
            }
            kdTree.freeKDTree(&rootNode);
        }

        currCost /= pixelCount;

        // reestimation
        for (uint32_t i = 0; i < maxColorCount; i++) {
            if (counts[i] > 0) {
                for (int d = 0; d < 3; d++) {
                    nextMeans[i][d] /= counts[i];
                }
                memcpy(means[i], nextMeans[i], sizeof(int) * 3);
            }
        }

        // terminal conditions
        iterateNum++;
        int diff = abs(lastCost - currCost);
        if (diff <= 3) {
            unchanged++;
        }
        if (iterateNum > 9 || unchanged > 1) {
            loop = false;
        }

//        stringstream ss;
//        ss << "iterateNum: " << iterateNum << " lastCost: " << lastCost << " currCost: " << currCost
//           << " diff:" << diff;
//        Logger::log(true, ss.str());
    }
    resultSize = maxColorCount;
    for (uint32_t l = 0; l < maxColorCount; ++l) {
        auto color = means[l];
        out[colorPaletteIndex].r = static_cast<uint8_t>(color[0]);
        out[colorPaletteIndex].g = static_cast<uint8_t>(color[1]);
        out[colorPaletteIndex].b = static_cast<uint8_t>(color[2]);
        out[colorPaletteIndex].index = static_cast<uint8_t>(colorPaletteIndex);
        colorPaletteIndex++;
    }
    delete[] counts;
    for (uint32_t i = 0; i < maxColorCount; i++) {
        delete[] means[i];
        delete[] nextMeans[i];
    }
    delete[] means;
    delete[] nextMeans;
    return resultSize;
}
