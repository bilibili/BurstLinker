//
// Created by succlz123 on 17-9-6.
//

#include <algorithm>
#include "GifBlockWriter.h"

using namespace std;
using namespace blk;

void GifBlockWriter::writeHeaderBlock(ofstream &file) {
    file.write("GIF89a", 6);
}

static const uint32_t GLOBAL_COLOR_TABLE_FLAG = 1 << 7;
static const uint32_t LSD_SORT_FLAG = 1 << 3;

void
GifBlockWriter::writeLogicalScreenDescriptorBlock(ofstream &file, int32_t logicalScreenWidth,
                                                  int32_t logicalScreenHeight,
                                                  bool globalColorTable, int32_t colorResolution,
                                                  bool sort,
                                                  int32_t globalColorTableSize,
                                                  int32_t backgroundColorIndex,
                                                  int32_t pixelAspectRatio) {
    file.write((char *) (&logicalScreenWidth), 2);
    file.write((char *) (&logicalScreenHeight), 2);

    auto packed = static_cast<uint8_t>(
            (globalColorTable ? GLOBAL_COLOR_TABLE_FLAG : 0)
            | (colorResolution << 4)
            | (sort ? LSD_SORT_FLAG : 0)
            | globalColorTableSize
    );
    file.write((char *) (&packed), 1);
    file.write((char *) (&backgroundColorIndex), 1);
    file.write((char *) (&pixelAspectRatio), 1);
}

static const uint8_t EXTENSION_INTRODUCER = 0x21;
static const uint8_t BLOCK_TERMINATOR = 0x00;

static const uint8_t APPLICATION_EXTENSION = 0xFF;
static const uint8_t APPLICATION_LENGTH = 0x0B;
static const uint8_t APPLICATION[] = "NETSCAPE2.0";
static const uint8_t SUB_BLOCK_SIZE = 0x03;
static const uint8_t SUB_BLOCK_ID = 0x01;

// Application Extension
void GifBlockWriter::writeNetscapeLoopingExtensionBlock(ofstream &file, uint32_t loopCount) {
    file.write((char *) (&EXTENSION_INTRODUCER), 1);
    file.write((char *) (&APPLICATION_EXTENSION), 1);
    // 8+3
    file.write((char *) (&APPLICATION_LENGTH), 1);
    file.write((char *) (&APPLICATION), 11);
    file.write((char *) (&SUB_BLOCK_SIZE), 1);
    file.write((char *) (&SUB_BLOCK_ID), 1);
    file.write((char *) (&loopCount), 1);
    loopCount = (loopCount >> 8);
    file.write((char *) (&loopCount), 1);
    file.write((char *) (&BLOCK_TERMINATOR), 1);
}

static const uint32_t USER_INPUT_FLAG = 1 << 1;
static const uint32_t TRANSPARENT_COLOR_FLAG = 1;

static const uint8_t GRAPHICS_CONTROL_LABEL = 0xF9;
static const uint8_t GRAPHICS_CONTROL_EXTENSION_BLOCK_SIZE = 0x04;

void GifBlockWriter::writeGraphicsControlExtensionBlock(vector<uint8_t> &content, int32_t disposalMethod,
                                                        bool userInput,
                                                        bool transparentColor,
                                                        int32_t delayCentiseconds,
                                                        int32_t transparentColorIndex) {
//    file.write((char *) (&EXTENSION_INTRODUCER), 1);
//    file.write((char *) (&GRAPHICS_CONTROL_LABEL), 1);
//    file.write((char *) (&GRAPHICS_CONTROL_EXTENSION_BLOCK_SIZE), 1);
//
//    uint8_t packed = (disposalMethod << 3
//                      | (userInput ? USER_INPUT_FLAG : 0)
//                      | (transparentColor ? TRANSPARENT_COLOR_FLAG : 0));
//    file.write((char *) (&packed), 1);
//    auto delay1 = static_cast<uint8_t>(delayCentiseconds & 0xFF);
//    auto delay2 = static_cast<uint8_t>(delayCentiseconds >> 8);
//    file.write((char *) (&delay1), 1);
//    file.write((char *) (&delay2), 1);
//    file.write((char *) (&transparentColorIndex), 1);
//    file.write((char *) (&BLOCK_TERMINATOR), 1);

    content.emplace_back(EXTENSION_INTRODUCER);
    content.emplace_back(GRAPHICS_CONTROL_LABEL);
    content.emplace_back(GRAPHICS_CONTROL_EXTENSION_BLOCK_SIZE);

    auto packed = static_cast<uint8_t>(
            disposalMethod << 3
            | (userInput ? USER_INPUT_FLAG : 0)
            | (transparentColor ? TRANSPARENT_COLOR_FLAG : 0)
    );
    content.emplace_back(packed);

    auto delay1 = static_cast<uint8_t>(delayCentiseconds & 0xFF);
    auto delay2 = static_cast<uint8_t>(delayCentiseconds >> 8);

    content.emplace_back(delay1);
    content.emplace_back(delay2);

    content.emplace_back(transparentColorIndex);
    content.emplace_back(BLOCK_TERMINATOR);
}

static const uint8_t IMAGE_SEPARATOR = 0x2C;
static const uint32_t LOCAL_COLOR_TABLE_FLAG = 1 << 7;
static const uint32_t INTERLACE_FLAG = 1 << 6;
static const uint32_t ID_SORT_FLAG = 1 << 5;

void
GifBlockWriter::writeImageDescriptorBlock(vector<uint8_t> &content, uint16_t imageLeft, uint16_t imageTop,
                                          uint16_t imageWidth,
                                          uint16_t imageHeight,
                                          bool localColorTable, bool interlace, bool sort,
                                          int localColorTableSize) {
    content.emplace_back(IMAGE_SEPARATOR);
    content.emplace_back(imageLeft & 0xFF);
    content.emplace_back(imageLeft >> 8);
    content.emplace_back(imageTop & 0xFF);
    content.emplace_back(imageTop >> 8);
    content.emplace_back(imageWidth & 0xFF);
    content.emplace_back(imageWidth >> 8);
    content.emplace_back(imageHeight & 0xFF);
    content.emplace_back(imageHeight >> 8);
    auto packed = static_cast<uint8_t>(
            (localColorTable ? LOCAL_COLOR_TABLE_FLAG : 0)
            | (interlace ? INTERLACE_FLAG : 0)
            | (sort ? ID_SORT_FLAG : 0)
            | localColorTableSize
    );
    content.emplace_back(packed);
//    file.write((const char *) (&IMAGE_SEPARATOR), 1);
//    file.write((const char *) (&imageLeft), 2);
//    file.write((const char *) (&imageTop), 2);
//    file.write((const char *) (&imageWidth), 2);
//    file.write((const char *) (&imageHeight), 2);
//    uint8_t packed = ((localColorTable ? LOCAL_COLOR_TABLE_FLAG : 0)
//                      | (interlace ? INTERLACE_FLAG : 0)
//                      | (sort ? ID_SORT_FLAG : 0)
//                      | localColorTableSize);
//    file.write((const char *) (&packed), 1);

}

int32_t GifBlockWriter::paddedSize(int32_t size) {
    int32_t n = size;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

void GifBlockWriter::writeColorTable(vector<uint8_t> &content, RGB quantizerPixels[],
                                     int quantizerSize, int paddedSize) {
    int32_t unpaddedSize = quantizerSize;
    for (int k = 0; k < quantizerSize; k++) {
        content.emplace_back(quantizerPixels[k].r);
        content.emplace_back(quantizerPixels[k].g);
        content.emplace_back(quantizerPixels[k].b);
//        file.write((char *) (&r), 1);
//        file.write((char *) (&g), 1);
//        file.write((char *) (&b), 1);
    }
    for (int32_t i = unpaddedSize; i < paddedSize; ++i) {
//        file.write("\0\0\0", 3);
        content.emplace_back('\0');
        content.emplace_back('\0');
        content.emplace_back('\0');
    }
}

void
GifBlockWriter::writeImageDataBlock(ofstream &file, uint8_t colorDepth, list<uint8_t *> lzwData,
                                    int lzwDataSize) {
    file.write((const char *) &colorDepth, 1);
    int index = 0;
    for (auto data : lzwData) {
        int subBlockLength = min(lzwDataSize - index, 255);
        file.write((const char *) &subBlockLength, 1);
        file.write((const char *) data, subBlockLength);
        index += subBlockLength;
    }
    file.write((char *) (&BLOCK_TERMINATOR), 1);
}

static const uint8_t GIF_TERMINATOR = 0x3B;

void
GifBlockWriter::writeTerminator(ofstream &file) {
    // GIF_TERMINATOR
    file.write((char *) (&GIF_TERMINATOR), 1);
}
