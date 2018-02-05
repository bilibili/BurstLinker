//
// Created by succlz123 on 17-9-19.
//

#include <cstdint>
#include "FloydSteinbergDitherer.h"
#include "KDTree.h"

using namespace std;
using namespace blk;

static const int8_t ERROR_COMPONENT_SIZE = 4;
static const float ERROR_COMPONENT_DELTA_X[] = {1.0f, -1.0f, 0.0f, 1.0f};
static const float ERROR_COMPONENT_DELTA_Y[] = {0.0f, 1.0f, 1.0f, 1.0f};
static const float ERROR_COMPONENT_FACTION[] = {7.0f / 16.0f, 3.0f / 16.0f, 5.0f / 16.0f, 1.0f / 16.0f};

void FloydSteinbergDitherer::dither(RGB *originPixels, uint16_t width, uint16_t height,
                                    RGB quantizerPixels[], int32_t quantizerSize,
                                    uint8_t *colorIndices) {
    int32_t totalSize = width * height;
    KDTree kdTree;
    KDTree::Node rootNode;
    kdTree.createKDTree(&rootNode, quantizerPixels, 0, quantizerSize - 1, 0);
    RGB target;
    uint8_t nearestCentroidR = 0;
    uint8_t nearestCentroidG = 0;
    uint8_t nearestCentroidB = 0;
    uint16_t lastR = 256;
    uint16_t lastG = 256;
    uint16_t lastB = 256;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    int position = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto rgb = originPixels[position];
            r = rgb.r;
            g = rgb.g;
            b = rgb.b;
            if (!(lastR == r && lastG == g && lastB == b)) {
                lastR = r;
                lastG = g;
                lastB = b;
                target.r = r;
                target.g = g;
                target.b = b;
                kdTree.searchNNNoBacktracking(&rootNode, target, -1);
            }

            nearestCentroidR = kdTree.nearest.r;
            nearestCentroidG = kdTree.nearest.g;
            nearestCentroidB = kdTree.nearest.b;

            originPixels[position].r = nearestCentroidR;
            originPixels[position].g = nearestCentroidG;
            originPixels[position].b = nearestCentroidB;

            position++;

            int8_t errorR = (r - nearestCentroidR);
            int8_t errorG = (g - nearestCentroidG);
            int8_t errorB = (b - nearestCentroidB);

            for (int directionId = 0; directionId < ERROR_COMPONENT_SIZE; ++directionId) {
                auto siblingX = static_cast<int32_t>(x + ERROR_COMPONENT_DELTA_X[directionId]);
                auto siblingY = static_cast<int32_t>(y + ERROR_COMPONENT_DELTA_Y[directionId]);

                if (siblingX >= 0 && siblingY >= 0 && siblingX < width && siblingY < height) {
                    float errorComponentR = errorR * ERROR_COMPONENT_FACTION[directionId];
                    float errorComponentG = errorG * ERROR_COMPONENT_FACTION[directionId];
                    float errorComponentB = errorB * ERROR_COMPONENT_FACTION[directionId];

                    int ditherPosition = siblingY * width + siblingX;
                    auto siblingRgb = originPixels[ditherPosition];

                    auto siblingR = static_cast<int32_t >(siblingRgb.r + errorComponentR);
                    auto siblingG = static_cast<int32_t>(siblingRgb.g + errorComponentG);
                    auto siblingB = static_cast<int32_t>(siblingRgb.b + errorComponentB);

                    originPixels[ditherPosition].r = static_cast<uint8_t>(min(255, max(0, siblingR)));
                    originPixels[ditherPosition].g = static_cast<uint8_t>(min(255, max(0, siblingG)));
                    originPixels[ditherPosition].b = static_cast<uint8_t>(min(255, max(0, siblingB)));
                }
            }
        }
    }
    for (int i = 0; i < totalSize; ++i) {
        auto rgb = originPixels[i];
        kdTree.searchNNNoBacktracking(&rootNode, rgb, -1);
        colorIndices[i] = kdTree.nearest.index;
    }
    kdTree.freeKDTree(&rootNode);
}
