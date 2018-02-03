//
// Created by succlz123 on 2017/10/14.
//

#include "MedianCutQuantizer.h"

struct RGB {
    int rgb[3];
};

RGB *pixels = nullptr;

struct Cluster {
    int start;
    int end;
    int pixelSize;
    int componentWithLargestSpread;

    static bool cmpG(const RGB &i, const RGB &j) { return (i.rgb[1] < j.rgb[1]); }

    static bool cmpR(const RGB &i, const RGB &j) { return (i.rgb[0] < j.rgb[0]); }

    static bool cmpB(const RGB &i, const RGB &j) { return (i.rgb[2] < j.rgb[2]); }

    bool (*cmp[3])(const RGB &, const RGB &) = {cmpR, cmpG, cmpB};

    bool operator<(const Cluster &cluster) const {
        return (pixelSize < cluster.pixelSize);
    }

    void setStartAndEnd(int s, int e) {
        start = s;
        end = e;
        pixelSize = end - start;
    }

    int getComponentSpread(int component) {
        int minCount = 0;
        int maxCount = 0;
        RGB p{};
        for (int i = start; i < end; i++) {
            p = pixels[i];
            int componentColor = p.rgb[component];
            minCount = min(minCount, componentColor);
            maxCount = max(maxCount, componentColor);
        }
        return maxCount - minCount;
    }

    void calculateSpread() {
        int largestSpread = -1;
        for (int component = 0; component < 3; ++component) {
            int componentSpread = getComponentSpread(component);
            if (componentSpread > largestSpread) {
                largestSpread = componentSpread;
                componentWithLargestSpread = component;
            }
        }
    }

    bool split(RGB *pixels, Cluster *cluster1, Cluster *cluster2) {
        if (pixelSize < 2) {
            return false;
        }
        sort(pixels + start, pixels + end, cmp[componentWithLargestSpread]);
        int medianIndex = (pixelSize + 1) / 2;
        cluster1->setStartAndEnd(start, start + medianIndex);
        cluster2->setStartAndEnd(start + medianIndex, end);
        return true;
    }
};

MedianCutQuantizer::~MedianCutQuantizer() {
    delete[] pixels;
    pixels = nullptr;
}

int32_t MedianCutQuantizer::quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) {
    pixels = new RGB[pixelCount];
    RGB pixel{};

    for (uint32_t i = 0; i < pixelCount; ++i) {
        pixel.rgb[0] = originalColors[i] & 0xFF;
        pixel.rgb[1] = originalColors[i] >> 8 & 0xFF;
        pixel.rgb[2] = originalColors[i] >> 16 & 0xFF;
        pixels[i] = pixel;
    }

    priority_queue<Cluster> clusters;
    Cluster cluster;
    cluster.setStartAndEnd(0, pixelCount);
    clusters.push(cluster);
    for (uint32_t k = 0; k < maxColorCount - 1; ++k) {
        Cluster top, cluster1, cluster2;
        top = clusters.top();
        clusters.pop();
        top.calculateSpread();
        bool success = top.split(pixels, &cluster1, &cluster2);
        if (!success) {
            continue;
        }
        cluster1.calculateSpread();
        cluster1.calculateSpread();
        clusters.push(cluster1);
        clusters.push(cluster2);
    }

    resultSize = clusters.size();
    int index = 0;
    for (int i = 0; i < resultSize; i++) {
        Cluster top = clusters.top();
        clusters.pop();
        uint32_t sumR = 0;
        uint32_t sumG = 0;
        uint32_t sumB = 0;
        for (int j = top.start; j < top.end; j++) {
            pixel = pixels[j];
            sumR += pixel.rgb[0];
            sumG += pixel.rgb[1];
            sumB += pixel.rgb[2];
        }
        colorPalette[index++] = sumR / top.pixelSize;
        colorPalette[index++] = sumG / top.pixelSize;
        colorPalette[index++] = sumB / top.pixelSize;
    }

    return resultSize;
}
