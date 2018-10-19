//
// Created by succlz123 on 17-10-9.
//

#include <algorithm>
#include <random>
#include <map>
#include "KMeansQuantizer.h"
#include "KDTree.h"
#include "Logger.h"

using namespace blk;

int32_t KMeansQuantizer::quantize(const std::vector<ARGB> &in, uint32_t maxColorCount, std::vector<ARGB> &out) {
    size_t pixelCount = in.size();
    if (pixelCount <= 0) {
        return 0;
    }
    // random initial center
    std::mt19937 generator((uint32_t) time(nullptr));
    std::uniform_int_distribution<uint32_t> dis(0, pixelCount);
    std::set<ARGB> centroidsToRecompute;
    uint32_t randomCount = 0;
    while (centroidsToRecompute.size() < maxColorCount) {
        uint32_t random = dis(generator);
        centroidsToRecompute.insert(in[random]);
        if (randomCount++ > pixelCount) {
            break;
        }
    }

    int colorPaletteIndex = 0;
    auto centroidSize = static_cast<uint32_t>(centroidsToRecompute.size());
    if (centroidSize < maxColorCount) {
        resultSize = centroidSize;
        for (ARGB color : centroidsToRecompute) {
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
    int label = 0;
    int iterateNum = 0;
    int lastCost = 0;
    int currCost = 0;
    int unchanged = 0;
    bool loop = true;
    std::vector<uint32_t> counts;
    counts.reserve(maxColorCount);
    auto **nextMeans = new uint32_t *[maxColorCount];
    for (uint32_t i = 0; i < maxColorCount; i++) {
        nextMeans[i] = new uint32_t[3];
    }

    while (loop) {
        // init
        counts.assign(maxColorCount, 0);
        for (size_t i = 0; i < maxColorCount; i++) {
            memset(nextMeans[i], 0, sizeof(int) * 3);
        }
        lastCost = currCost;
        currCost = 0;

        // classification
        {
            KDTree kdTree;
            std::vector<ARGB> datas;
            for (uint32_t l = 0; l < maxColorCount; ++l) {
                auto color = means[l];
                auto r = static_cast<uint8_t>(color[0]);
                auto g = static_cast<uint8_t>(color[1]);
                auto b = static_cast<uint8_t>(color[2]);
                datas.emplace_back(r, g, b, l);
            }
            KDTree::Node rootNode;
            kdTree.createKDTree(&rootNode, datas, 0, maxColorCount - 1, 0);
            for (uint32_t i = 0; i < pixelCount; i++) {
                auto rgb = in[i];
                currCost += kdTree.searchNoBacktracking(&rootNode, rgb.r, rgb.g, rgb.b, -1);
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

//        std::stringstream ss;
//        ss << "iterateNum: " << iterateNum << " lastCost: " << lastCost << " currCost: " << currCost
//           << " diff:" << diff;
//        Logger::log(true, ss.str());
    }
    resultSize = maxColorCount;
    for (uint32_t l = 0; l < maxColorCount; ++l) {
        auto color = means[l];
        auto r = static_cast<uint8_t>(color[0]);
        auto g = static_cast<uint8_t>(color[1]);
        auto b = static_cast<uint8_t>(color[2]);
        out.emplace_back(r, g, b, colorPaletteIndex);
        colorPaletteIndex++;
    }
    for (uint32_t i = 0; i < maxColorCount; i++) {
        delete[] means[i];
        delete[] nextMeans[i];
    }
    delete[] means;
    delete[] nextMeans;
    return resultSize;
}
