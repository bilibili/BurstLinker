//
// Created by succlz123 on 17-10-9.
//

#include <algorithm>
#include <random>
#include <map>
#include <iostream>
#include <cstring>
#include <ctime>
#include "KMeansQuantizer.h"
#include "KDTree.h"
#include "GifLogger.h"

using namespace std;

uint32_t getNearestColor(uint32_t color, set<uint32_t> &colors) {
    int nearestCentroidR = 0;
    int nearestCentroidG = 0;
    int nearestCentroidB = 0;

    int positiveInfinity = 255 * 255 * 3;

    uint32_t b = (color >> 16) & 0xFF;
    uint32_t g = (color >> 8) & 0xFF;
    uint32_t r = (color) & 0xFF;

    int nearestCentroidDistance = positiveInfinity;

    for (uint32_t centroidColor :colors) {

        int qb = (centroidColor >> 16) & 0xFF;
        int qg = (centroidColor >> 8) & 0xFF;
        int qr = (centroidColor) & 0xFF;

        int nr = r - qr;
        int ng = g - qg;
        int nb = b - qb;

        int distance = nr * nr + ng * ng + nb * nb;
        if (distance < nearestCentroidDistance) {
            nearestCentroidR = qr;
            nearestCentroidG = qg;
            nearestCentroidB = qb;
            nearestCentroidDistance = distance;
        };
    }
    return nearestCentroidR << 16 | nearestCentroidG << 8 | nearestCentroidB;
}

uint32_t getCentroid(vector<uint32_t> &cluster) {
    uint32_t sumR = 0;
    uint32_t sumG = 0;
    uint32_t sumB = 0;
    unsigned long size = cluster.size();

    for (unsigned long j = 0; j < size; ++j) {
        uint32_t color = cluster[j];
        uint32_t b = color >> 16 & 0xFF;
        uint32_t g = color >> 8 & 0xFF;
        uint32_t r = color & 0xFF;

        uint32_t scaledR = r * 1;
        uint32_t scaledG = g * 1;
        uint32_t scaledB = b * 1;

        sumR += scaledR;
        sumG += scaledG;
        sumB += scaledB;
    }

    sumR = sumR / size;
    sumG = sumG / size;
    sumB = sumB / size;
    return sumB << 16 | sumG << 8 | sumR;
}

int32_t
KMeansQuantizer::quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) {
    // random initial center
    mt19937 generator((uint32_t) time(nullptr));
    uniform_int_distribution<uint32_t> dis(0, pixelCount);
    set<uint32_t> centroidsToRecompute;
    uint32_t randomCount = 0;
    while (centroidsToRecompute.size() < maxColorCount) {
        uint32_t random = dis(generator);
        centroidsToRecompute.insert(originalColors[random]);
        if (randomCount++ > pixelCount) {
            break;
        }
    }

    int colorPaletteIndex = 0;
    auto centroidSize = static_cast<uint32_t>(centroidsToRecompute.size());
    if (centroidSize < maxColorCount) {
        resultSize = centroidSize;
        for (uint32_t color : centroidsToRecompute) {
            colorPalette[colorPaletteIndex++] = color & 0xFF;
            colorPalette[colorPaletteIndex++] = color >> 8 & 0xFF;
            colorPalette[colorPaletteIndex++] = color >> 16 & 0xFF;
        }
        return centroidSize;
    }

    if (true) {
        auto **means = new uint32_t *[maxColorCount];
        for (uint32_t m = 0; m < maxColorCount; ++m) {
            auto *current = new uint32_t[3]{0};
            means[m] = current;
        }
        int currentMeans = 0;
        for (uint32_t color : centroidsToRecompute) {
            means[currentMeans][0] = color & 0xFF;
            means[currentMeans][1] = color >> 8 & 0xFF;
            means[currentMeans][2] = color >> 16 & 0xFF;
            currentMeans++;
        }

        auto *originalColorRgb = new uint32_t[pixelCount * 3];
        int index = 0;
        for (uint32_t k = 0; k < pixelCount; ++k) {
            uint32_t color = originalColors[k];
            *(originalColorRgb + index++) = color & 0xFF;
            *(originalColorRgb + index++) = color >> 8 & 0xFF;
            *(originalColorRgb + index++) = color >> 16 & 0xFF;
        }

        // recursion
        auto *rgb = new int[3];
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
            auto *kdTree = new KDTree();
            auto **datas = new int *[maxColorCount];
            for (uint32_t l = 0; l < maxColorCount; ++l) {
                auto color = means[l];
                int r = color[0];
                int g = color[1];
                int b = color[2];
                datas[l] = new int[4]{r, g, b, static_cast<int>(l)};
            }
            KDTree::Node *tree = kdTree->createKDTree(datas, maxColorCount, 0);
            for (uint32_t i = 0; i < pixelCount; i++) {
                for (int j = 0; j < 3; j++) {
                    rgb[j] = originalColorRgb[i * 3 + j];
                }
                currCost += kdTree->searchNN(tree, rgb, -1);
                label = kdTree->nearest.data[3];
                counts[label]++;
                for (int k = 0; k < 3; k++) {
                    nextMeans[label][k] += rgb[k];
                }
            }
            kdTree->freeKDTree(tree);
            delete[] datas;
            delete kdTree;

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
            if (iterateNum >= 12 || unchanged >= 3) {
                loop = false;
            }

//            stringstream ss;
//            ss << "iterateNum: " << iterateNum << " lastCost: " << lastCost << " currCost: " << currCost
//               << " diff:" << diff;
//            GifLogger::log(true, ss.str());
        }
        resultSize = maxColorCount;
        for (uint32_t l = 0; l < maxColorCount; ++l) {
            auto color = means[l];
            colorPalette[colorPaletteIndex++] = color[0];
            colorPalette[colorPaletteIndex++] = color[1];
            colorPalette[colorPaletteIndex++] = color[2];
        }
        delete[] rgb;
        delete[] counts;
        for (uint32_t i = 0; i < maxColorCount; i++) {
            delete[] means[i];
            delete[] nextMeans[i];
        }
        delete[] means;
        delete[] nextMeans;
    } else {
        map<uint32_t, vector<uint32_t>> clustersByCentroid;
        map<uint32_t, vector<uint32_t>>::iterator iter;

        for (uint32_t color : centroidsToRecompute) {
            clustersByCentroid.insert(
                    pair<uint32_t, vector<uint32_t>>(color, *new vector<uint32_t>()));
        }

        // distribution of each pixel to the closest point
        for (uint32_t i = 0; i < pixelCount; ++i) {
            uint32_t color = originalColors[i];
            uint32_t nearestColor = getNearestColor(color, centroidsToRecompute);
            iter = clustersByCentroid.find(nearestColor);
            if (!(iter == clustersByCentroid.end())) {
                iter->second.push_back(nearestColor);
            }
        }

        while (!centroidsToRecompute.empty()) {
            // new cluster center
            for (uint32_t oldCentroid : centroidsToRecompute) {
                iter = clustersByCentroid.find(oldCentroid);
                vector<uint32_t> cluster = iter->second;
                if (!cluster.empty()) {
                    uint32_t newCentroid = getCentroid(cluster);
                    clustersByCentroid.erase(oldCentroid);
                    clustersByCentroid.insert(
                            pair<uint32_t, vector<uint32_t>>(newCentroid, cluster));
                }
            }
            centroidsToRecompute.clear();

            set<uint32_t> allCentroids; // at the center of each iteration
            iter = clustersByCentroid.begin();
            while (iter != clustersByCentroid.end()) {
                allCentroids.insert(iter++->first);
            }
            // calculate the distance each point into the center of each Cluster, add it to the nearest the Cluster
            for (uint32_t centroid :allCentroids) {
                vector<uint32_t> cluster = clustersByCentroid.find(centroid)->second;

                for (auto clusterIter = cluster.begin(); clusterIter != cluster.end();) {
                    uint32_t color = *clusterIter;
                    uint32_t newCentroid = getNearestColor(color, allCentroids);
                    if (newCentroid != centroid) {
                        iter = clustersByCentroid.find(newCentroid);
                        iter->second.push_back(color);
                        cluster.erase(clusterIter);
                        centroidsToRecompute.insert(centroid);
                        centroidsToRecompute.insert(newCentroid);
                    } else {
                        ++clusterIter;
                    }
                }
            }
        }

        resultSize = clustersByCentroid.size();
        for (iter = clustersByCentroid.begin(); iter != clustersByCentroid.end(); iter++) {
            uint32_t color = iter->first;
            colorPalette[colorPaletteIndex++] = color & 0xFF;
            colorPalette[colorPaletteIndex++] = color >> 8 & 0xFF;
            colorPalette[colorPaletteIndex++] = color >> 16 & 0xFF;
        }
    }
    return resultSize;
}
