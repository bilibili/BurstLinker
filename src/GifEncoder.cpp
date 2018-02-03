//
// Created by succlz123 on 17-9-5.
//

#include <cstdint>
#include <fstream>
#include "GifEncoder.h"
#include "GifBlockWriter.h"
#include "GifLogger.h"
#include "ColorQuantizer.h"
#include "KMeansQuantizer.h"
#include "MedianCutQuantizer.h"
#include "OctreeQuantizer.h"
#include "RandomQuantizer.h"
#include "BayerDitherer.h"
#include "LzwEncoder.h"
#include "FloydSteinbergDitherer.h"
#include "UniformQuantizer.h"
#include "NeuQuantQuantizer.h"
#include "M2Ditherer.h"
#include "DisableDitherer.h"

using namespace std;

#if defined(__Android__)

#include <RenderScript.h>
#include "../android/lib/src/main/cpp/DisableDithererWithRs.h"
#include "../android/lib/src/main/cpp/BayerDithererWithRs.h"

using namespace android::RSC;

sp<RS> rs = nullptr;

#elif defined(__Other__)

#endif

int getColorTableSizeField(int actualTableSize) {
    int size = 0;
    while (1 << (size + 1) < actualTableSize) {
        ++size;
    }
    return size;
}

GifEncoder::~GifEncoder() {
    screenWidth = 0;
    screenHeight = 0;
    debugLog = false;
#if defined(__Android__)
    if (rs != nullptr) {
        rs.clear();
        rs = nullptr;
    }
#endif
    outfile.close();
    delete threadPool;
    delete[] rsCacheDir;
}

bool GifEncoder::init(const char *path, uint16_t width, uint16_t height, uint32_t loopCount,
                      uint32_t threadCount) {
    outfile.open(path, ios::out | ios::binary);
    if (!outfile.is_open()) {
        return false;
    }
    this->screenWidth = width;
    this->screenHeight = height;
    GifBlockWriter::writeHeaderBlock(outfile);
    GifBlockWriter::writeLogicalScreenDescriptorBlock(outfile, screenWidth, screenHeight, false, 1,
                                                      false, 0, 0, 0);
    GifBlockWriter::writeNetscapeLoopingExtensionBlock(outfile, loopCount);
    if (threadCount > 8) {
        threadCount = 8;
    }
    if (threadCount > 1) {
        threadPool = new ThreadPool(threadCount);
    }
    GifLogger::log(debugLog, "Image size is " + GifLogger::toString(width * height));
    return true;
}

vector<uint8_t> GifEncoder::addImage(uint32_t *originalColors, uint32_t delay,
                                     QuantizerType quantizerType, DitherType ditherType,
                                     float scale, uint16_t left, uint16_t top,
                                     vector<uint8_t> &content) {
    GifLogger::log(debugLog, "Get image pixel");

    ColorQuantizer *colorQuantizer = nullptr;
    string quantizerStr;
    switch (quantizerType) {
        case Uniform:
        default:
            colorQuantizer = new UniformQuantizer();
            quantizerStr = "UniformQuantizer";
            break;
        case MedianCut:
            colorQuantizer = new MedianCutQuantizer();
            quantizerStr = "MedianCutQuantizer";
            break;
        case KMeans:
            colorQuantizer = new KMeansQuantizer();
            quantizerStr = "KMeansQuantizer";
            break;
        case Random:
            colorQuantizer = new RandomQuantizer();
            quantizerStr = "RandomQuantizer";
            break;
        case Octree:
            colorQuantizer = new OctreeQuantizer();
            quantizerStr = "OctreeQuantizer";
            break;
        case NeuQuant:
            colorQuantizer = new NeuQuantQuantizer();
            quantizerStr = "NeuQuantQuantizer";
            break;
    }

    uint32_t srcWidth;
    uint32_t srcHeight;
    uint32_t processSize = 0;
    uint32_t *processColor = nullptr;
    if (scale <= 0 || scale > 1.0f) {
        scale = 1.0f;
    }
    if (scale != 1.0f) {
        srcWidth = static_cast<uint32_t>(screenWidth * scale);
        srcHeight = static_cast<uint32_t>(screenHeight * scale);

        uint32_t xr = (srcWidth << 16) / screenWidth + 1;
        uint32_t yr = (srcHeight << 16) / screenHeight + 1;

        processSize = srcWidth * srcHeight;
        processColor = new uint32_t[processSize];

        for (uint16_t y = 0; y < srcHeight; ++y) {
            for (uint16_t x = 0; x < srcWidth; ++x) {
                uint32_t srcX = (x * xr) >> 16;
                uint32_t srcY = (y * yr) >> 16;
                processColor[y * srcWidth + x] = originalColors[srcY * screenHeight + srcX];
            }
        }
        GifLogger::log(debugLog, "Scale");
    } else {
        srcWidth = screenWidth;
        srcHeight = screenHeight;
        processSize = 0;
    }

    colorQuantizer->width = srcWidth;
    colorQuantizer->height = srcHeight;

    size_t colorSize = screenWidth * screenHeight;

    uint8_t *quantizerColors = nullptr;
    int32_t quantizerSize = 0;
    if (colorSize > 256) {
        if (processSize > 256) {
            quantizerSize = colorQuantizer->quantize(processColor, processSize, 256);
        } else {
            quantizerSize = colorQuantizer->quantize(originalColors, colorSize, 256);
        }
        quantizerColors = new uint8_t[(quantizerSize + 1) * 3];
        colorQuantizer->getColorPalette(quantizerColors);
    } else {
        int quantizerIndex = 0;
        quantizerColors = new uint8_t[(colorSize + 1) * 3];
        for (uint32_t i = 0; i < colorSize; ++i) {
            uint32_t color = originalColors[colorSize];
            quantizerColors[quantizerIndex++] = static_cast<uint8_t>((color) & 0xFF);
            quantizerColors[quantizerIndex++] = static_cast<uint8_t>((color >> 8) & 0xFF);
            quantizerColors[quantizerIndex++] = static_cast<uint8_t>((color >> 16) & 0xFF);
        }
    }
    GifLogger::log(debugLog, quantizerStr + " size is " + GifLogger::toString(quantizerSize));

    if (quantizerSize <= 0) {
        return content;
    }

    Ditherer *ditherer = nullptr;

    string dithererStr;

#if defined(__Android__)
    bool useRenderScript = false;
    if (rsCacheDir != nullptr) {
        if (rs == nullptr) {
            rs = new RS();
        }
        if (!rs.get()->getContext()) {
            useRenderScript = rs->init(rsCacheDir);
        } else {
            useRenderScript = true;
        }
    }
#endif

#if defined(__Android__)
    switch (ditherType) {
        case Disable:
        default:
            if (useRenderScript) {
                ditherer = new DisableDithererWithRs();
                dithererStr = "DisableDithererWithRs";
            } else {
                ditherer = new DisableDitherer();
                dithererStr = "DisableDitherer";
            }
            break;
        case M2:
            useRenderScript = false;
            ditherer = new M2Ditherer();
            dithererStr = "M2Ditherer";
            break;
        case Bayer:
            if (useRenderScript) {
                ditherer = new BayerDithererWithRs();
                dithererStr = "BayerDithererWithRs";
            } else {
                ditherer = new BayerDitherer();
                dithererStr = "BayerDitherer";
            }
            break;
        case FloydSteinberg:
            useRenderScript = false;
            ditherer = new FloydSteinbergDitherer();
            dithererStr = "FloydSteinbergDitherer";
            break;
    }
#elif defined(__Other__)
    switch (ditherType) {
        default:
        case Disable:
            ditherer = new DisableDitherer();
            dithererStr = "DisableDitherer";
            break;
        case M2:
            ditherer = new M2Ditherer();
            dithererStr = "M2Ditherer";
            break;
        case Bayer:
            ditherer = new BayerDitherer();
            dithererStr = "BayerDitherer";
            break;
        case FloydSteinberg:
            ditherer = new FloydSteinbergDitherer();
            dithererStr = "FloydSteinbergDitherer";
            break;
    }
#endif

    ditherer->quantizerType = quantizerType;
    ditherer->colorQuantizer = colorQuantizer;

#if defined(__Android__)
    if (useRenderScript) {
        static_cast<DithererWithRs *>(ditherer)->dither(originalColors, screenWidth, screenHeight,
                                                        quantizerColors,
                                                        quantizerSize, rs);
    } else {
        ditherer->dither(originalColors, screenWidth, screenHeight, quantizerColors,
                         quantizerSize);
    }
#elif defined(__Other__)
    ditherer->dither(originalColors, screenWidth, screenHeight, quantizerColors,
                     quantizerSize);
#endif

    auto *colorIndices = new uint32_t[colorSize];
    ditherer->getColorIndices(colorIndices, colorSize);
    delete colorQuantizer;
    delete ditherer;

    GifLogger::log(debugLog, dithererStr);

//    int32_t paddedColorCount = GifBlockWriter::paddedSize(quantizerSize);
    int32_t paddedColorCount = 256;

    GifBlockWriter::writeGraphicsControlExtensionBlock(content, 0, false, false, delay, 0);
    GifBlockWriter::writeImageDescriptorBlock(content, left, top, screenWidth, screenHeight, true,
                                              false,
                                              false,
                                              getColorTableSizeField(paddedColorCount));
    GifBlockWriter::writeColorTable(content, quantizerColors, quantizerSize, paddedColorCount);
    delete[] quantizerColors;

    auto *lzwData = new char[colorSize]{0};
    LzwEncoder lzwEncoder(paddedColorCount);
    lzwEncoder.encode(colorIndices, screenWidth, screenHeight, colorSize, lzwData, content);
    GifLogger::log(debugLog, "LZW encode");
    delete[] colorIndices;
    delete[] lzwData;
    return content;
}

void GifEncoder::flush(vector<uint8_t> &content) {
    uint64_t size = content.size();
    for (int i = 0; i < size; ++i) {
        outfile.write((char *) (&content[i]), 1);
    }
}

void GifEncoder::finishEncoding() {
    GifBlockWriter::writeTerminator(outfile);
}
