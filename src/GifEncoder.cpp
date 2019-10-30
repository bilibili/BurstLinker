//
// Created by succlz123 on 17-9-5.
//

#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
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
    delete[] rsCacheDir;
}

bool GifEncoder::init(const char *path, uint16_t width, uint16_t height, uint32_t loopCount,
                      uint32_t threadCount) {
    outfile.open(path, std::ios::out | std::ios::binary);
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
    if (threadCount >= 1) {
        threadPool = std::make_unique<ThreadPool>(threadCount);
    }
    Logger::log(debugLog, "Image size is " + Logger::toString(width * height));
    return true;
}

std::vector<uint8_t> GifEncoder::addImage(const std::vector<uint32_t> &original, uint32_t delay,
                                          QuantizerType qType, DitherType dType,
                                          int32_t transparencyOption, uint16_t left, uint16_t top,
                                          std::vector<uint8_t> &content) {
    Logger::log(debugLog, "Get image pixel " + Logger::toString(original.size()));

    uint32_t size = screenWidth * screenHeight;
    std::unique_ptr<ColorQuantizer> colorQuantizer;
    std::string quantizerStr;
    switch (qType) {
        case QuantizerType::Uniform:
            colorQuantizer = std::make_unique<UniformQuantizer>();
            quantizerStr = "UniformQuantizer";
            break;
        case QuantizerType::MedianCut:
            colorQuantizer = std::make_unique<MedianCutQuantizer>();
            quantizerStr = "MedianCutQuantizer";
            break;
        case QuantizerType::KMeans:
            colorQuantizer = std::make_unique<KMeansQuantizer>();
            quantizerStr = "KMeansQuantizer";
            break;
        case QuantizerType::Random:
            colorQuantizer = std::make_unique<RandomQuantizer>();
            quantizerStr = "RandomQuantizer";
            break;
        case QuantizerType::Octree:
            colorQuantizer = std::make_unique<OctreeQuantizer>();
            quantizerStr = "OctreeQuantizer";
            break;
        case QuantizerType::NeuQuant:
            colorQuantizer = std::make_unique<NeuQuantQuantizer>();
            quantizerStr = "NeuQuantQuantizer";
            break;
    }

    std::vector<ARGB> quantizeIn;
    quantizeIn.reserve(size);
    bool enableTransparentColor = ((transparencyOption & 0xff) == 1);
    bool ignoreTranslucency = (((transparencyOption >> 8) & 0xff) == 1);
    bool hasTransparentColor = false;
    uint8_t a = 255;
    for (uint32_t i = 0; i < size; i++) {
        auto color = original[i];
        if (enableTransparentColor) {
            a = static_cast<uint8_t>((color >> 24) & 0xff);
            if (!hasTransparentColor) {
                if ((ignoreTranslucency && a != 255) || (!ignoreTranslucency && a == 0)) {
                    hasTransparentColor = true;
                }
            }
        }
        auto b = static_cast<uint8_t>((color >> 16) & 0xff);
        auto g = static_cast<uint8_t>((color >> 8) & 0xff);
        auto r = static_cast<uint8_t>(color & 0xff);
        if (a == 255 || (!ignoreTranslucency && a != 0)) {
            quantizeIn.emplace_back(a, r, g, b, i);
        }
    }

    std::vector<ARGB> quantizeOut;
    quantizeOut.reserve(256);
    int quantizeSize = 0;
    if (size > 256) {
        quantizeSize = colorQuantizer->quantize(quantizeIn, hasTransparentColor ? 255 : 256, quantizeOut);
    } else {
        quantizeSize = size;
        quantizeOut.assign(quantizeIn.begin(), quantizeIn.end());
    }
    Logger::log(debugLog, quantizerStr + " size is " + Logger::toString(quantizeSize));

    if (quantizeSize <= 0) {
        return content;
    }

    uint8_t transparentColorR = 0;
    uint8_t transparentColorG = 0;
    uint8_t transparentColorB = 0;
    if (hasTransparentColor) {
        std::mt19937 generator((uint32_t) time(nullptr));
        std::uniform_int_distribution<uint32_t> sizeDis(0, size);
        std::uniform_int_distribution<uint32_t> rgbDis(0, 255);
        int tryCount = 0;
        while (tryCount < 12) {
            uint32_t random = sizeDis(generator);
            if (tryCount >= 6) {
                transparentColorR = static_cast<uint8_t>(rgbDis(generator));
                transparentColorG = static_cast<uint8_t>(rgbDis(generator));
                transparentColorB = static_cast<uint8_t>(rgbDis(generator));
            } else {
                transparentColorR = quantizeIn[random].r;
                transparentColorG = quantizeIn[random].g;
                transparentColorB = quantizeIn[random].b;
            }
            int repeatCount = 0;
            for (int i = 0; i < quantizeSize; i++) {
                auto qColor = quantizeOut[i];
                if (transparentColorR == qColor.r && transparentColorG == qColor.g &&
                    transparentColorB == qColor.b) {
                    break;
                } else {
                    repeatCount++;
                }
            }
            if (repeatCount == quantizeSize) {
                break;
            }
            tryCount++;
        }
        if (dType == DitherType::FloydSteinberg) {
            dType = DitherType::Bayer;
        }
    }

    //    int32_t paddedColorCount = GifBlockWriter::paddedSize(quantizeSize);
    int32_t paddedColorCount = 256;
    auto transparentColorIndex = static_cast<int32_t>(quantizeSize + 1);
    GifBlockWriter::writeGraphicsControlExtensionBlock(content, 2, false, hasTransparentColor,
                                                       delay / 10,
                                                       hasTransparentColor ? transparentColorIndex
                                                                           : 0);
    GifBlockWriter::writeImageDescriptorBlock(content, left, top, screenWidth, screenHeight, true,
                                              false, false,
                                              getColorTableSizeField(paddedColorCount));
    GifBlockWriter::writeColorTableEntity(content, quantizeOut, paddedColorCount);

    std::unique_ptr<Ditherer> ditherer;
    std::string dithererStr;

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
    switch (dType) {
        case DitherType::No:
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
    switch (dType) {
        case DitherType::No:
            ditherer = std::make_unique<NoDitherer>();
            dithererStr = "NoDitherer";
            break;
        case DitherType::M2:
            ditherer = std::make_unique<M2Ditherer>();
            dithererStr = "M2Ditherer";
            break;
        case DitherType::Bayer:
            ditherer = std::make_unique<BayerDitherer>();
            dithererStr = "BayerDitherer";
            break;
        case DitherType::FloydSteinberg:
            ditherer = std::make_unique<FloydSteinbergDitherer>();
            dithererStr = "FloydSteinbergDitherer";
            break;
    }
#endif

    ditherer->width = screenWidth;
    ditherer->height = screenHeight;
    auto colorIndices = new uint8_t[size];

#if defined(__RenderScript__)
    if (useRenderScript) {
        static_cast<DithererWithRs *>(ditherer.get())->dither(pixels, screenWidth, screenHeight,
                                                              quantizerPixels, quantizerSize,
                                                              colorIndices.get(), rs);
    } else if (qType == QuantizerType::Octree && dType == DitherType::No) {
        static_cast<OctreeQuantizer *>(colorQuantizer.get())->getColorIndices(pixels,
                                                                              colorIndices.get(),
                                                                              pixelCount, nullptr);
    } else {
        ditherer->dither(pixels, screenWidth, screenHeight, quantizerPixels, quantizerSize,
                         colorIndices.get());
    }
#else
    if (qType == QuantizerType::Octree && dType == DitherType::No && !hasTransparentColor) {
        static_cast<OctreeQuantizer *>(colorQuantizer.get())->getColorIndices(quantizeIn,
                                                                              colorIndices);
    } else {
        ditherer->dither(quantizeIn, quantizeOut, colorIndices);
    }
#endif

    if (hasTransparentColor) {
        GifBlockWriter::writeColorTableTransparency(content, transparentColorR, transparentColorG,
                                                    transparentColorB);
        GifBlockWriter::writeColorTableUnpadded(content, transparentColorIndex, paddedColorCount);
    } else {
        GifBlockWriter::writeColorTableUnpadded(content, quantizeSize, paddedColorCount);
    }

    Logger::log(debugLog, dithererStr);

    LzwEncoder lzwEncoder(paddedColorCount);
    lzwEncoder.encode(colorIndices, screenWidth, screenHeight, content);
    delete[] colorIndices;
    Logger::log(debugLog, "LZW encode");
    return content;
}

void GifEncoder::flush(const std::vector<uint8_t> &content) {
    size_t size = content.size();
    for (int i = 0; i < size; ++i) {
        outfile.write((char *) (&content[i]), 1);
    }
}

void GifEncoder::finishEncoding() {
    GifBlockWriter::writeTerminator(outfile);
    outfile.close();
}
