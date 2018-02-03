//
// Created by succlz123 on 2017/12/28.
//

#ifndef BURSTLINKER_KDTREE_H
#define BURSTLINKER_KDTREE_H

#include <fstream>

class KDTree {

public:

    struct Compare {

        int split;

        explicit Compare(int split) : split(split) {};

        bool operator()(const int *a, const int *b) {
            return a[split] < b[split];
        }
    };

    struct Node {
        int *data = nullptr;
        int split = 0;
        Node *left = nullptr;
        Node *right = nullptr;
    };

    Node nearest;

    Node *createKDTree(int **data, int size, int split);

    int searchNN(Node *node, int *target, int dis);

    void freeKDTree(Node *tree);

};


#endif //BURSTLINKER_KMEANS_H
