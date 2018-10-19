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

        void *createKDTree(Node *node, std::vector<ARGB> &quantize, int32_t start, int32_t end, uint8_t split);

        int searchNoBacktracking(Node *node, uint8_t r, uint8_t g, uint8_t b, int32_t dis);

        void freeKDTree(Node *tree);

    };

}

#endif //BURSTLINKER_KDTREE_H
