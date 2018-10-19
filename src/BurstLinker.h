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

        bool init(const char *path, uint16_t width, uint16_t height, uint32_t loopCount,
                  uint32_t threadNum);

        bool connect(std::vector<uint32_t> &image, uint32_t delay,
                     QuantizerType quantizerType, DitherType ditherType, int32_t transparencyOption,
                     uint16_t left, uint16_t top);

        bool connect(std::vector<std::vector<uint32_t >> &images, uint32_t delay,
                     QuantizerType quantizerType, DitherType ditherType, int32_t transparencyOption,
                     uint16_t left, uint16_t top);

        void release();

        void analyzerGifInfo(const char *path);

    private:

        std::unique_ptr<GifEncoder> gifEncoder;

    };

}

#endif //BURSTLINKER_BURSTLINKER_H
