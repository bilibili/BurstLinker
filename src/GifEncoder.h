//
// Created by succlz123 on 17-9-5.
//

#include <cstdint>
#include <fstream>
#include <vector>
#include "ThreadPool.h"

#ifndef BURSTLINKER_GIFENCODER_H
#define BURSTLINKER_GIFENCODER_H


namespace blk {

    struct RGB {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t index = 0;

        bool operator==(const RGB &rgb) const {
            return rgb.r == r && rgb.g == g && rgb.b == b;
        }

        bool operator<(const RGB &rgb) const {
            return (r + g + b) < (rgb.r + rgb.g + rgb.b);
        }
    };

    struct Compare {
        uint8_t split = 0;

        explicit Compare(uint8_t split) : split(split) {};

        bool operator()(const RGB &a, const RGB &b) {
            switch (split) {
                case 0:
                default:
                    return a.r > b.r;
                case 1:
                    return a.g > b.g;
                case 2:
                    return a.b > b.b;
            }
        }
    };

    enum class QuantizerType {
        Uniform = 0,
        MedianCut = 1,
        KMeans = 2,
        Random = 3,
        Octree = 4,
        NeuQuant = 5
    };

    enum class DitherType {
        NO = 0,
        M2 = 1,
        Bayer = 2,
        FloydSteinberg = 3
    };

    class GifEncoder {

    public:

        uint16_t screenWidth;

        uint16_t screenHeight;

        bool debugLog = false;

        const char *rsCacheDir = nullptr;

        ThreadPool *threadPool = nullptr;

        ~GifEncoder();

        bool
        init(const char *path, uint16_t width, uint16_t height, uint32_t loopCount,
             uint32_t threadCount);

        std::vector<uint8_t>
        addImage(uint32_t *originalColors, uint32_t delay,
                 QuantizerType quantizerType, DitherType ditherType,
                 uint16_t left, uint16_t top, std::vector<uint8_t> &content);

        void flush(std::vector<uint8_t> &content);

        void finishEncoding();

    private:

        std::ofstream outfile;

    };

}

#endif //BURSTLINKER_GIFENCODER_H
