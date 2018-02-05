//
// Created by succlz123 on 17-9-6.
//

#ifndef BURSTLINKER_GIFBLOCKWRITER_H
#define BURSTLINKER_GIFBLOCKWRITER_H

#include <fstream>
#include <list>
#include <vector>
#include "GifEncoder.h"

namespace blk {

    class GifBlockWriter {

    public:

        static void writeHeaderBlock(std::ofstream &file);

        static void
        writeLogicalScreenDescriptorBlock(std::ofstream &file, int32_t logicalScreenWidth,
                                          int32_t logicalScreenHeight,
                                          bool globalColorTable, int32_t colorResolution, bool sort,
                                          int32_t globalColorTableSize,
                                          int32_t backgroundColorIndex, int32_t pixelAspectRatio);

        static void writeNetscapeLoopingExtensionBlock(std::ofstream &file, uint32_t loopCount);

        static void
        writeGraphicsControlExtensionBlock(std::vector<uint8_t> &content, int32_t disposalMethod, bool userInput,
                                           bool transparentColor, int32_t delayCentiseconds,
                                           int32_t transparentColorIndex);

        static void writeImageDescriptorBlock(std::vector<uint8_t> &content, uint16_t imageLeft, uint16_t imageTop,
                                              uint16_t imageWidth,
                                              uint16_t imageHeight,
                                              bool localColorTable, bool interlace, bool sort,
                                              int localColorTableSize);

        static int32_t paddedSize(int32_t size);

        static void writeColorTable(std::vector<uint8_t> &content, RGB quantizerPixels[], int quantizerSize,
                                    int paddedSize);

        static void
        writeImageDataBlock(std::ofstream &file, uint8_t colorDepth, std::list<uint8_t *> lzwData,
                            int lzwDataSize);

        static void writeTerminator(std::ofstream &file);

    };

}

#endif //BURSTLINKER_GIFBLOCKWRITER_H
