//
// Created by succlz123 on 17-9-12.
//

#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include "OctreeQuantizer.h"

const uint8_t mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void OctreeQuantizer::reduceTree() {
    int32_t i;
    uint32_t redSum = 0;
    uint32_t greenSum = 0;
    uint32_t blueSum = 0;
    uint32_t pixelCount = 0;

    // Find the deepest level containing at least one reducible node
    for (i = 7; (i > 0) && (nodeList[i] == nullptr); i--) {
    };

    // Reduce the node most recently added to the list at level i
    Node *tmpNode = nodeList[i];
    nodeList[i] = tmpNode->next;

    for (i = 0; i < 8; i++) {
        if (tmpNode->child[i] != nullptr) {
            redSum += tmpNode->child[i]->rSum;
            greenSum += tmpNode->child[i]->gSum;
            blueSum += tmpNode->child[i]->bSum;
            pixelCount += tmpNode->child[i]->pixelCount;

            free(tmpNode->child[i]);
            tmpNode->child[i] = nullptr;
            leafCount--;
        }
    }

    tmpNode->isLeaf = true;
    tmpNode->rSum = redSum;
    tmpNode->gSum = greenSum;
    tmpNode->bSum = blueSum;
    tmpNode->pixelCount = pixelCount;

    leafCount++;
}

OctreeQuantizer::Node *OctreeQuantizer::createNode(int inLevel) {
    auto *node = new Node();
    memset(node, 0, sizeof(Node));
    node->isLeaf = (inLevel == 8);
    if (node->isLeaf) {
        leafCount++;
    } else {
        node->next = nodeList[inLevel];
        nodeList[inLevel] = node;
    }
    return node;
}

bool OctreeQuantizer::addColor(Node *&node, uint32_t r, uint32_t g, uint32_t b, int level) {
    int index, shift;

    if (node == nullptr) {
        node = createNode(level);
    }
    if (node == nullptr) {
        return false;
    }

    if (node->isLeaf) {
        node->pixelCount++;
        node->rSum += r;
        node->gSum += g;
        node->bSum += b;
    } else {
        shift = 7 - level;
        index = (((r & mask[level]) >> shift) << 2)
                | (((g & mask[level]) >> shift) << 1)
                | ((b & mask[level]) >> shift);
        if (!addColor(node->child[index], r, g, b, level + 1)) {
            return false;
        }
    }
    return true;
}

int32_t OctreeQuantizer::getColorIndex(uint8_t r, uint8_t g, uint8_t b) const {
    int level = 0;
    Node *currentTree = octree;
    while (!currentTree->isLeaf) {
        int shift = 7 - level;
        int index = (((r & mask[level]) >> shift) << 2) |
                    (((g & mask[level]) >> shift) << 1) |
                    ((b & mask[level]) >> shift);
        Node *child = currentTree->child[index];
        if (child == nullptr) {
            return -1;
        }
        currentTree = child;
        level++;
    }
    return currentTree->colorIndex;
}

void OctreeQuantizer::getColorPalette(Node *tree, int &inIndex) {
    if (tree->isLeaf) {
        int colorPaletteIndex = inIndex * 3;

        if (tree->pixelCount != 1) {
            tree->rSum = tree->rSum / tree->pixelCount;
            tree->gSum = tree->gSum / tree->pixelCount;
            tree->bSum = tree->bSum / tree->pixelCount;
            tree->pixelCount = 1;
        }
        tree->colorIndex = inIndex;

        *(colorPalette + colorPaletteIndex) = static_cast<uint8_t>(tree->rSum);
        *(colorPalette + colorPaletteIndex + 1) = static_cast<uint8_t>(tree->gSum);
        *(colorPalette + colorPaletteIndex + 2) = static_cast<uint8_t>(tree->bSum);

        inIndex++;
    } else {
        for (auto &i : tree->child) {
            if (i != nullptr) {
                getColorPalette(i, inIndex);
            }
        }
    }
}

void OctreeQuantizer::freeTree(Node *&tree) {
    if (!tree) {
        return;
    }
    for (auto &i : tree->child) {
        if (i) {
            freeTree(i);
        }
    }
    free(tree);
    tree = nullptr;
}

OctreeQuantizer::~OctreeQuantizer() {
    freeTree(octree);
    leafCount = 0;
    for (auto &node : nodeList) {
        node = nullptr;
    }
}

int32_t
OctreeQuantizer::quantize(uint32_t *originalColors, uint32_t pixelCount, uint32_t maxColorCount) {
    if (height == 0) {
        return 0;
    }
    leafCount = 0;
    Node *node = nullptr;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int position = y * width + x;
            uint32_t color = originalColors[position];
            uint32_t b = color >> 16 & 0xFF;
            uint32_t g = color >> 8 & 0xFF;
            uint32_t r = color & 0xFF;
            if (!addColor(node, r, g, b, 0)) {
                return 0;
            }
            while (leafCount > maxColorCount) {
                reduceTree();
            }
        }
    }
    octree = node;
    getColorPalette(node, resultSize);
    return resultSize;
}

void
OctreeQuantizer::getColorIndices(uint32_t *originalColors, uint32_t *out, int size,
                                 int (*getOffset)(int, int)) {
    int lastR = 256;
    int lastG = 256;
    int lastB = 256;
    int lastIndex = 0;
    for (int j = 0; j < size; ++j) {
        uint32_t color = originalColors[j];
        int b = color >> 16 & 0xFF;
        int g = color >> 8 & 0xFF;
        int r = color & 0xFF;
        if (getOffset) {
            int offset = getOffset(j, color);
            r = (min(255, max(0, r + offset)));
            g = (min(255, max(0, g + offset)));
            b = (min(255, max(0, b + offset)));
        }
        if (!(lastR == r && lastG == g && lastB == b)) {
            lastR = r;
            lastG = g;
            lastB = b;
            int current = getColorIndex(r, g, b);
            if (current >= 0) {
                lastIndex = current;
            }
        }
        out[j] = lastIndex;
    }
}
