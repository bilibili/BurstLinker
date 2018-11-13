//
// Created by succlz123 on 17-9-12.
//

#include <cstring>
#include <cstdint>
#include <iostream>
#include "OctreeQuantizer.h"

using namespace blk;

static const uint8_t mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void OctreeQuantizer::reduceTree() {
    int32_t i;
    uint32_t redSum = 0;
    uint32_t greenSum = 0;
    uint32_t blueSum = 0;
    uint32_t pixelCount = 0;

    for (i = 7; (i > 0) && (nodeList[i] == nullptr); i--) {
    };

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

bool OctreeQuantizer::addColor(Node **node, uint32_t r, uint32_t g, uint32_t b, int level) {
    if (*node == nullptr) {
        *node = createNode(level);
    }
    if (*node == nullptr) {
        return false;
    }

    if ((*node)->isLeaf) {
        (*node)->pixelCount++;
        (*node)->rSum += r;
        (*node)->gSum += g;
        (*node)->bSum += b;
    } else {
        int shift = 7 - level;
        int index = (((r & mask[level]) >> shift) << 2)
                    | (((g & mask[level]) >> shift) << 1)
                    | ((b & mask[level]) >> shift);
        if (!addColor(&((*node)->child[index]), r, g, b, level + 1)) {
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

void OctreeQuantizer::getColorPalette(Node *tree, int32_t &inIndex, std::vector<ARGB> &out) {
    if (tree == nullptr) {
        return;
    }
    if (tree->isLeaf) {
        if (tree->pixelCount != 1) {
            tree->rSum /= tree->pixelCount;
            tree->gSum /= tree->pixelCount;
            tree->bSum /= tree->pixelCount;
            tree->pixelCount = 1;
        }
        tree->colorIndex = static_cast<uint8_t>(inIndex);
        auto r = static_cast<uint8_t>(tree->rSum);
        auto g = static_cast<uint8_t>(tree->gSum);
        auto b = static_cast<uint8_t>(tree->bSum);
        out.emplace_back(r, g, b, inIndex);
        inIndex++;
    } else {
        for (auto &i : tree->child) {
            if (i != nullptr) {
                getColorPalette(i, inIndex, out);
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
}

int32_t OctreeQuantizer::quantize(const std::vector<ARGB> &in, uint32_t maxColorCount, std::vector<ARGB> &out) {
    leafCount = 0;
    size_t pixelCount = in.size();
    for (int i = 0; i < pixelCount; ++i) {
        auto color = in[i];
        if (!addColor(&octree, color.r, color.g, color.b, 0)) {
            return 0;
        }
        while (leafCount > maxColorCount) {
            reduceTree();
        }
    }
    getColorPalette(octree, resultSize, out);
    return resultSize;
}

void OctreeQuantizer::getColorIndices(const std::vector<ARGB> &pixels, uint8_t *out) {
    size_t size = pixels.size();
    int lastR = 256;
    int lastG = 256;
    int lastB = 256;
    uint8_t lastIndex = 0;
    for (int j = 0; j < size; ++j) {
        auto color = pixels[j];
        uint8_t r = color.r;
        uint8_t g = color.g;
        uint8_t b = color.b;
        if (!(lastR == r && lastG == g && lastB == b)) {
            lastR = r;
            lastG = g;
            lastB = b;
            int32_t current = getColorIndex(r, g, b);
            if (current >= 0) {
                lastIndex = static_cast<uint8_t>(current);
            }
        }
        out[j] = lastIndex;
    }
}
