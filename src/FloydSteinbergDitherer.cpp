//
// Created by succlz123 on 17-9-19.
//

#include <cstdint>
#include <cmath>
#include <map>
#include "FloydSteinbergDitherer.h"
#include "KDTree.h"

using namespace std;

static const int8_t ERROR_COMPONENT_SIZE = 4;
static const float ERROR_COMPONENT_DELTA_X[] = {1.0f, -1.0f, 0.0f, 1.0f};
static const float ERROR_COMPONENT_DELTA_Y[] = {0.0f, 1.0f, 1.0f, 1.0f};
static const float ERROR_COMPONENT_FACTION[] = {7.0f / 16.0f, 3.0f / 16.0f, 5.0f / 16.0f, 1.0f / 16.0f};

void FloydSteinbergDitherer::dither(uint32_t *originalColors, int width, int height, uint8_t *quantizerColors,
                                    int quantizerSize) {
    int32_t totalSize = width * height;
    colorIndices = new uint32_t[totalSize];

    map<uint32_t, uint32_t> colorTable;

    auto **datas = new int *[quantizerSize];
    for (int k = 0; k < quantizerSize * 3; k = k + 3) {
        int qr = quantizerColors[k] & 0xFF;
        int qg = quantizerColors[k + 1] & 0xFF;
        int qb = quantizerColors[k + 2] & 0xFF;
        int index = k / 3;
        auto *data = new int[4]{qr, qg, qb, index};
        datas[index] = data;
        colorTable.insert(pair<uint32_t, uint32_t>(qr << 16 | qg << 8 | qb, index));
    }

    auto *kdTree = new KDTree();
    KDTree::Node *tree = kdTree->createKDTree(datas, quantizerSize, 0);
    int target[3];
    int nearestCentroidR = 0;
    int nearestCentroidG = 0;
    int nearestCentroidB = 0;
    int lastR = 256;
    int lastG = 256;
    int lastB = 256;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int position = y * width + x;
            uint32_t rgb = originalColors[position];

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
            }

            nearestCentroidR = kdTree->nearest.data[0];
            nearestCentroidG = kdTree->nearest.data[1];
            nearestCentroidB = kdTree->nearest.data[2];

            originalColors[position] = nearestCentroidR << 16 | nearestCentroidG << 8 | nearestCentroidB;

            int errorR = (r - nearestCentroidR);
            int errorG = (g - nearestCentroidG);
            int errorB = (b - nearestCentroidB);

            for (int directionId = 0; directionId < ERROR_COMPONENT_SIZE; ++directionId) {
                auto siblingX = static_cast<int>(x + ERROR_COMPONENT_DELTA_X[directionId]);
                auto siblingY = static_cast<int>(y + ERROR_COMPONENT_DELTA_Y[directionId]);

                if (siblingX >= 0 && siblingY >= 0 && siblingX < width && siblingY < height) {

                    float errorComponentR = errorR * ERROR_COMPONENT_FACTION[directionId];
                    float errorComponentG = errorG * ERROR_COMPONENT_FACTION[directionId];
                    float errorComponentB = errorB * ERROR_COMPONENT_FACTION[directionId];

                    int ditherPosition = siblingY * width + siblingX;
                    int siblingRgb = originalColors[ditherPosition];

                    auto siblingR = static_cast<int>((siblingRgb >> 16 & 0xFF) + errorComponentR);
                    auto siblingG = static_cast<int>((siblingRgb >> 8 & 0xFF) + errorComponentG);
                    auto siblingB = static_cast<int>((siblingRgb & 0xFF) + errorComponentB);

                    originalColors[ditherPosition] = (min(255, max(0, siblingR))) << 16
                                                     | (min(255, max(0, siblingG))) << 8
                                                     | (min(255, max(0, siblingB)));
                }
            }
        }
    }

    int lastPixel = -1;
    uint32_t lastColorIndices = 0;
    for (int i = 0; i < totalSize; ++i) {
        const uint32_t pixel = originalColors[i];
        if (pixel != lastPixel) {
            lastColorIndices = colorTable.find(pixel)->second;
        }
        colorIndices[i] = lastColorIndices;
    }
    kdTree->freeKDTree(tree);
    delete[] datas;
    delete kdTree;
}
