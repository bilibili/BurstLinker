//
// Created by succlz123 on 2017/12/28.
//

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <cassert>
#include <algorithm>
#include "KDTree.h"

using namespace std;

// euclideanDistance
// int distance = nr * nr + ng * ng + nb * nb;
// manhattanDistance
// int distance = abs(nr) + abs(ng) + abs(nb);
static int calculateDist(KDTree::Node *node, const int *target) {
    int tmp = 0;
    for (int i = 0; i < 3; i++) {
        int diff = node->data[i] - target[i];
        if (i == 0) {
            tmp += 2 * diff * diff;
        } else if (i == 1) {
            tmp += 4 * diff * diff;
        } else {
            tmp += 3 * diff * diff;
        }
    }
    return tmp;
}

static int getDimension(int *data[], int dataSize) {
    if (dataSize == 0) {
        return 0;
    }
    int sumR = 0;
    int sumG = 0;
    int sumB = 0;

    for (int i = 0; i < dataSize; ++i) {
        auto color = data[i];
        int qr = color[0];
        int qg = color[1];
        int qb = color[2];
        sumR += qr;
        sumG += qg;
        sumB += qb;
    }

    int rAve = sumR / dataSize;
    int gAve = sumG / dataSize;
    int bAve = sumB / dataSize;

    for (int i = 0; i < dataSize; ++i) {
        auto color = data[i];
        int qr = color[0];
        int qg = color[1];
        int qb = color[2];
        sumR += (qr - rAve) * (qr - rAve);
        sumG += (qg - gAve) * (qg - gAve);
        sumB += (qb - bAve) * (qb - bAve);
    }

    rAve = sumR / dataSize;
    gAve = sumG / dataSize;
    bAve = sumB / dataSize;

    int maxVariance = rAve;
    if (gAve > maxVariance) {
        maxVariance = gAve;
    }
    if (bAve > maxVariance) {
        maxVariance = bAve;
    }

    int dimension = 0;
    if (maxVariance == rAve) {
        dimension = 0;
    } else if (maxVariance == gAve) {
        dimension = 1;
    } else if (maxVariance == bAve) {
        dimension = 2;
    }
    return dimension;
}

KDTree::Node *KDTree::createKDTree(int **data, int size, int split) {
    if (size == 0) {
        return nullptr;
    }

    auto *node = new Node;

    if (size == 1) {
        node->data = data[0];
        node->split = split;
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }

    sort(data, data + size, Compare(split));

    int splitSize = size / 2;

    auto **leftData = new int *[splitSize];
    auto **rightData = new int *[splitSize];

    node->data = data[splitSize];
    node->split = split;

    int leftIndex = 0;
    int rightIndex = 0;
    for (int i = 0; i < size; ++i) {
        if (i < splitSize) {
            leftData[leftIndex++] = data[i];
        } else if (i > splitSize) {
            rightData[rightIndex++] = data[i];
        }
    }

    // (split + 1) % 3
    int leftSplit = getDimension(leftData, leftIndex);
    int rightSplit = getDimension(rightData, rightIndex);

    node->left = createKDTree(leftData, leftIndex, leftSplit);
    node->right = createKDTree(rightData, rightIndex, rightSplit);
    return node;
}

int KDTree::searchNN(KDTree::Node *node, int *target, int dis) {
    if (node == nullptr) {
        return dis;
    }
    if (node->left == nullptr && node->right == nullptr) {
        if (dis < 0) {
            nearest = *node;
            dis = calculateDist(node, target);
            return dis;
        }
    }
    if (target[node->split] <= node->data[node->split]) {
        dis = searchNN(node->left, target, dis);
        int tmp = calculateDist(node, target);
        if (tmp < dis || dis == -1) {
            nearest = *node;
            dis = tmp;
        }
    } else {
        dis = searchNN(node->right, target, dis);
        int tmp = calculateDist(node, target);
        if (tmp < dis || dis == -1) {
            nearest = *node;
            dis = tmp;
        }
    }
    return dis;
}

void KDTree::freeKDTree(KDTree::Node *tree) {
    if (tree == nullptr) {
        return;
    }
    freeKDTree(tree->left);
    freeKDTree(tree->right);
    tree->split = 0;
    delete[] tree->data;
    tree->data = nullptr;
    delete tree;
}
