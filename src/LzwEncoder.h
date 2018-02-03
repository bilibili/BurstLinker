//
// Created by succlz123 on 17-9-9.
//

#ifndef BURSTLINKER_LZWENCODER_H
#define BURSTLINKER_LZWENCODER_H

#include <fstream>
#include <cstring>
#include <list>
#include <vector>

using namespace std;

class LzwEncoder {

public:

    explicit LzwEncoder(int paddedColorCount);

    ~LzwEncoder();

    void encode(uint32_t indices[], int width, int height, int size, char out[], vector<uint8_t> &content);

private:

    static const int32_t BLOCK_SIZE = 255;

    list<uint8_t *> datas;

    uint8_t *current;

    int pos;

    int remain;

    uint8_t numColors;

    void writeBits(uint32_t src, int32_t bit);

    int write(vector<uint8_t> &content, uint8_t minimumCodeSize);
};


#endif //BURSTLINKER_LZWENCODER_H
