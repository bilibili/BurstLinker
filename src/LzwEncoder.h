//
// Created by succlz123 on 17-9-9.
//

#ifndef BURSTLINKER_LZWENCODER_H
#define BURSTLINKER_LZWENCODER_H

#include <fstream>
#include <cstring>
#include <list>
#include <vector>

namespace blk {

    class LzwEncoder {

    public:

        explicit LzwEncoder(int32_t paddedColorCount);

        ~LzwEncoder();

        void encode(uint8_t indices[], uint16_t width, uint16_t height, std::vector<uint8_t> &content);

    private:

        std::list<uint8_t *> datas;

        uint8_t *current;

        int pos;

        int remain;

        int32_t numColors;

        void writeBits(uint32_t src, int32_t bit);

        int write(std::vector<uint8_t> &content, uint8_t minimumCodeSize);
    };

}

#endif //BURSTLINKER_LZWENCODER_H
