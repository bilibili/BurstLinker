//
// Created by succlz123 on 2017/11/30.
//

#ifndef BURSTLINKER_BURSTLINKER_H
#define BURSTLINKER_BURSTLINKER_H

#include <cstdint>
#include "GifEncoder.h"

namespace blk {

    class BurstLinker {

    public:

        ~BurstLinker();

        bool init(const char *path, uint16_t width, uint16_t height, uint32_t loopCount,
                  uint32_t threadNum);

        bool connect(uint32_t *imagePixels, uint32_t delay,
                     QuantizerType quantizerType, DitherType ditherType,
                     uint16_t left, uint16_t top);

        bool connect(std::vector<uint32_t *> imagePixels, uint32_t delay,
                     QuantizerType quantizerType, DitherType ditherType,
                     uint16_t left, uint16_t top);

        void release();

        void analyzerGifInfo(const char *path);

    private:

        GifEncoder *gifEncoder = nullptr;

    };

}

#endif //BURSTLINKER_BURSTLINKER_H
