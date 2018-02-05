//
// Created by succlz123 on 2017/10/14.
//

#include <algorithm>
#include <queue>
#include <iostream>
#include <cstring>
#include "MedianCutQuantizer.h"

using namespace std;
using namespace blk;

struct Cluster {
    int start = 0;
    int end = 0;
    int pixelSize = 0;
    int componentWithLargestSpread = 0;

    static bool cmpR(const RGB &i, const RGB &j) { return (i.r < j.r); }

    static bool cmpG(const RGB &i, const RGB &j) { return (i.g < j.g); }

    static bool cmpB(const RGB &i, const RGB &j) { return (i.b < j.b); }

    bool (*cmp[3])(const RGB &, const RGB &) = {cmpR, cmpG, cmpB};

    bool operator<(const Cluster &cluster) const {
        return (pixelSize < cluster.pixelSize);
    }

    void setStartAndEnd(int s, int e) {
        start = s;
        end = e;
        pixelSize = end - start;
    }

    int getComponentRSpread(RGB *pixels) {
        uint8_t minCount = 0;
        uint8_t maxCount = 0;
        for (int i = start; i < end; i++) {
            uint8_t componentColor = pixels[i].r;
            minCount = std::min(minCount, componentColor);
            maxCount = std::max(maxCount, componentColor);
        }
        return maxCount - minCount;
    }

    int getComponentGSpread(RGB *pixels) {
        uint8_t minCount = 0;
        uint8_t maxCount = 0;
        for (int i = start; i < end; i++) {
            uint8_t componentColor = pixels[i].g;
            minCount = std::min(minCount, componentColor);
            maxCount = std::max(maxCount, componentColor);
        }
        return maxCount - minCount;
    }

    int getComponentBSpread(RGB *pixels) {
        uint8_t minCount = 0;
        uint8_t maxCount = 0;
        for (int i = start; i < end; i++) {
            uint8_t componentColor = pixels[i].b;
            minCount = std::min(minCount, componentColor);
            maxCount = std::max(maxCount, componentColor);
        }
        return maxCount - minCount;
    }

    void calculateSpread(RGB *pixels) {
        int largestSpread = -1;
        int componentSpread = getComponentRSpread(pixels);
        if (componentSpread > largestSpread) {
            largestSpread = componentSpread;
            componentWithLargestSpread = 0;
        }
        componentSpread = getComponentGSpread(pixels);
        if (componentSpread > largestSpread) {
            largestSpread = componentSpread;
            componentWithLargestSpread = 1;
        }
        componentSpread = getComponentBSpread(pixels);
        if (componentSpread > largestSpread) {
            componentWithLargestSpread = 2;
        }
    }

    bool split(RGB *pixels, Cluster *cluster1, Cluster *cluster2) {
        if (pixelSize < 2) {
            return false;
        }

//        std::sort(pixels + start, pixels + end, cmp[componentWithLargestSpread]);
        switch (componentWithLargestSpread) {
            case 0:
                countingSortR(pixels, start, end);
                break;
            case 1:
                countingSortG(pixels, start, end);
                break;
            case 2:
                countingSortB(pixels, start, end);
                break;
            default:
                break;
        }

        int medianIndex = (pixelSize + 1) / 2;
        cluster1->setStartAndEnd(start, start + medianIndex);
        cluster2->setStartAndEnd(start + medianIndex, end);
        return true;
    }

    void countingSortR(RGB *a, int start, int end) {
        vector<RGB> c[256];
        for (int i = start; i < end; i++) {
            RGB rgb = a[i];
            int count = rgb.r;
            c[count].push_back(rgb);
        }
        int index = 0;
        for (const auto &res : c) {
            for (const auto out : res) {
                a[index++] = out;
            }
        }
    }

    void countingSortG(RGB *a, int start, int end) {
        vector<RGB> c[256];
        for (int i = start; i < end; i++) {
            RGB rgb = a[i];
            int count = rgb.g;
            c[count].push_back(rgb);
        }
        int index = 0;
        for (const auto &res : c) {
            for (const auto out : res) {
                a[index++] = out;
            }
        }
    }

    void countingSortB(RGB *a, int start, int end) {
        vector<RGB> c[256];
        for (int i = start; i < end; i++) {
            RGB rgb = a[i];
            int count = rgb.b;
            c[count].push_back(rgb);
        }
        int index = 0;
        for (const auto &res : c) {
            for (const auto out : res) {
                a[index++] = out;
            }
        }
    }
};

int32_t MedianCutQuantizer::quantize(RGB *pixels, uint32_t pixelCount, uint32_t maxColorCount, RGB out[]) {
    std::priority_queue<Cluster> clusters;
    Cluster cluster;
    cluster.setStartAndEnd(0, pixelCount);
    clusters.push(cluster);
    auto *sortPixels = new RGB[pixelCount];
    memcpy(sortPixels, pixels, pixelCount * sizeof(RGB));
    for (uint32_t k = 0; k < maxColorCount - 1; ++k) {
        Cluster top, cluster1, cluster2;
        top = clusters.top();
        clusters.pop();
        top.calculateSpread(sortPixels);
        bool success = top.split(sortPixels, &cluster1, &cluster2);
        if (!success) {
            continue;
        }
        cluster1.calculateSpread(sortPixels);
        cluster1.calculateSpread(sortPixels);
        clusters.push(cluster1);
        clusters.push(cluster2);
    }
    resultSize = static_cast<int32_t>(clusters.size());
    int index = 0;
    for (int i = 0; i < resultSize; i++) {
        Cluster top = clusters.top();
        clusters.pop();
        uint32_t sumR = 0;
        uint32_t sumG = 0;
        uint32_t sumB = 0;
        for (int j = top.start; j < top.end; j++) {
            sumR += sortPixels[j].r;
            sumG += sortPixels[j].g;
            sumB += sortPixels[j].b;
        }
        out[index].r = static_cast<uint8_t>(sumR / top.pixelSize);
        out[index].g = static_cast<uint8_t>(sumG / top.pixelSize);
        out[index].b = static_cast<uint8_t>(sumB / top.pixelSize);
        out[index].index = static_cast<uint8_t>(index);
        index++;
    }
    delete[] sortPixels;
    return resultSize;
}
