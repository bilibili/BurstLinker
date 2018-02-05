//
// Created by succlz123 on 2017/12/28.
//

#ifndef BURSTLINKER_KDTREE_H
#define BURSTLINKER_KDTREE_H

#include "GifEncoder.h"

namespace blk {

    class KDTree {

    public:

        struct Node {
            uint8_t r = 0;
            uint8_t g = 0;
            uint8_t b = 0;
            uint8_t index = 0;
            uint8_t split = 0;
            Node *left = nullptr;
            Node *right = nullptr;
        };

        Node nearest;

        void *createKDTree(Node *node, RGB rgb[], int32_t start, int32_t end, uint8_t split);

        int searchNNNoBacktracking(Node *node, RGB target, int32_t dis);

        void freeKDTree(Node *tree);

    };

}

#endif //BURSTLINKER_KDTREE_H
