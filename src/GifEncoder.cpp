//
// Created by succlz123 on 17-9-5.
//

#include <cstdint>
#include <fstream>
#include <iostream>
#include "GifEncoder.h"
#include "GifBlockWriter.h"
#include "Logger.h"
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
#include "NoDitherer.h"

using namespace std;
using namespace blk;

#if defined(__RenderScript__)

#include <RenderScript.h>
#include "../android/lib/src/main/cpp/NoDithererWithRs.h"
#include "../android/lib/src/main/cpp/BayerDithererWithRs.h"

using namespace android::RSC;

sp<RS> rs = nullptr;

#elif defined(__SIMD__)


#endif

static int getColorTableSizeField(int actualTableSize) {
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
    Logger::log(debugLog, "Image size is " + Logger::toString(width * height));
    return true;
}

vector<uint8_t> GifEncoder::addImage(uint32_t *originalColors, uint32_t delay,
                                     QuantizerType quantizerType, DitherType ditherType,
                                     uint16_t left, uint16_t top,
                                     vector<uint8_t> &content) {
    Logger::log(debugLog, "Get image pixel");

    uint32_t pixelCount = screenWidth * screenHeight;
    unique_ptr<ColorQuantizer> colorQuantizer;
    string quantizerStr;
    switch (quantizerType) {
        case QuantizerType::Uniform:
            colorQuantizer.reset(new UniformQuantizer());
            quantizerStr = "UniformQuantizer";
            break;
        case QuantizerType::MedianCut:
            colorQuantizer.reset(new MedianCutQuantizer());
            quantizerStr = "MedianCutQuantizer";
            break;
        case QuantizerType::KMeans:
            colorQuantizer.reset(new KMeansQuantizer());
            quantizerStr = "KMeansQuantizer";
            break;
        case QuantizerType::Random:
            colorQuantizer.reset(new RandomQuantizer());
            quantizerStr = "RandomQuantizer";
            break;
        case QuantizerType::Octree:
            colorQuantizer.reset(new OctreeQuantizer());
            quantizerStr = "OctreeQuantizer";
            break;
        case QuantizerType::NeuQuant:
            colorQuantizer.reset(new NeuQuantQuantizer());
            quantizerStr = "NeuQuantQuantizer";
            break;
    }

    auto *pixels = reinterpret_cast<RGB *>(originalColors);

    RGB quantizerPixels[256];
    int32_t quantizerSize = 0;
    if (pixelCount > 256) {
        quantizerSize = colorQuantizer->quantize(pixels, pixelCount, 256, quantizerPixels);
    } else {
        quantizerSize = pixelCount;
        memcpy(quantizerPixels, pixels, pixelCount * sizeof(RGB));
    }
    Logger::log(debugLog, quantizerStr + " size is " + Logger::toString(quantizerSize));

    if (quantizerSize <= 0) {
        return content;
    }

    //    int32_t paddedColorCount = GifBlockWriter::paddedSize(quantizerSize);
    int32_t paddedColorCount = 256;
    GifBlockWriter::writeGraphicsControlExtensionBlock(content, 0, false, false, delay / 10, 0);
    GifBlockWriter::writeImageDescriptorBlock(content, left, top, screenWidth, screenHeight, true,
                                              false,
                                              false,
                                              getColorTableSizeField(paddedColorCount));
    GifBlockWriter::writeColorTable(content, quantizerPixels, quantizerSize, paddedColorCount);

    unique_ptr<Ditherer> ditherer;
    unique_ptr<uint8_t[]> colorIndices(new uint8_t[pixelCount]);
    string dithererStr;

#if defined(__RenderScript__)
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
    switch (ditherType) {
        case DitherType::NO:
            if (useRenderScript) {
                ditherer.reset(new NoDithererWithRs());
                dithererStr = "NoDithererWithRs";
            } else {
                ditherer.reset(new NoDitherer());
                dithererStr = "NoDitherer";
            }
            break;
        case DitherType::M2:
            useRenderScript = false;
            ditherer.reset(new M2Ditherer());
            dithererStr = "M2Ditherer";
            break;
        case DitherType::Bayer:
            if (useRenderScript) {
                ditherer.reset(new BayerDithererWithRs());
                dithererStr = "BayerDithererWithRs";
            } else {
                ditherer.reset(new BayerDitherer());
                dithererStr = "BayerDitherer";
            }
            break;
        case DitherType::FloydSteinberg:
            useRenderScript = false;
            ditherer.reset(new FloydSteinbergDitherer());
            dithererStr = "FloydSteinbergDitherer";
            break;
    }
#else
    switch (ditherType) {
        case DitherType::NO:
            ditherer.reset(new NoDitherer());
            dithererStr = "NoDitherer";
            break;
        case DitherType::M2:
            ditherer.reset(new M2Ditherer());
            dithererStr = "M2Ditherer";
            break;
        case DitherType::Bayer:
            ditherer.reset(new BayerDitherer());
            dithererStr = "BayerDitherer";
            break;
        case DitherType::FloydSteinberg:
            ditherer.reset(new FloydSteinbergDitherer());
            dithererStr = "FloydSteinbergDitherer";
            break;
    }
#endif

#if defined(__RenderScript__)
    if (useRenderScript) {
        static_cast<DithererWithRs *>(ditherer.get())->dither(pixels, screenWidth, screenHeight,
                                                              quantizerPixels, quantizerSize,
                                                              colorIndices.get(), rs);
    } else if (quantizerType == QuantizerType::Octree && ditherType == DitherType::NO) {
        static_cast<OctreeQuantizer *>(colorQuantizer.get())->getColorIndices(pixels,
                                                                              colorIndices.get(),
                                                                              pixelCount, nullptr);
    } else {
        ditherer->dither(pixels, screenWidth, screenHeight, quantizerPixels, quantizerSize,
                         colorIndices.get());
    }
#else
    if (quantizerType == QuantizerType::Octree && ditherType == DitherType::NO) {
        static_cast<OctreeQuantizer *>(colorQuantizer.get())->getColorIndices(pixels,
                                                                              colorIndices.get(),
                                                                              pixelCount, nullptr);
    } else {
        ditherer->dither(pixels, screenWidth, screenHeight, quantizerPixels, quantizerSize,
                         colorIndices.get());
    }
#endif

    Logger::log(debugLog, dithererStr);

    LzwEncoder lzwEncoder(paddedColorCount);
    lzwEncoder.encode(colorIndices.get(), screenWidth, screenHeight, content);
    Logger::log(debugLog, "LZW encode");
    return content;
}

void GifEncoder::flush(vector<uint8_t> &content) {
    size_t size = content.size();
    for (int i = 0; i < size; ++i) {
        outfile.write((char *) (&content[i]), 1);
    }
}

void GifEncoder::finishEncoding() {
    GifBlockWriter::writeTerminator(outfile);
    outfile.close();
}
