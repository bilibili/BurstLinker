//
// Created by succlz123 on 2017/11/4.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "GifAnalyzer.h"
#include "Logger.h"

using namespace std;
using namespace blk;

static const size_t BUF_SIZE = 1024;

static void analyzeError() {
    fprintf(stderr, "Parse failed\n");
    exit(1);
}

static void readNbytes(FILE *file, uint8_t *buf, size_t nbytes) {
    size_t retCode = fread(buf, 1, nbytes, file);
    if (retCode == nbytes) {
        buf[nbytes] = 0;
    } else {
        analyzeError();
    }
}

static void analyzeColorTable(FILE *file, unsigned table_size) {
    uint8_t buf[BUF_SIZE] = {0};
    printf("%8s%8s%8s%8s\n", "INDEX", "RED", "GREEN", "BLUE");
    printf("--------------------------------\n");
    for (unsigned index = 0; index < table_size; ++index) {
        readNbytes(file, buf, 3);
        printf("%8u%8u%8u%8u\n", index, buf[0], buf[1], buf[2]);
    }
}

static unsigned analyzeDataBlock(FILE *file) {
    unsigned totalSize = 0;
    uint8_t buf[BUF_SIZE] = {0};
    for (;;) {
        readNbytes(file, buf, 1);
        unsigned blockSize = *buf;
        if (blockSize) {
            readNbytes(file, buf, blockSize);
            totalSize += blockSize;
        } else {
            break;
        }
    }
    if (totalSize) {
        printf("Data Block Size: %u (%.2fKB)\n", totalSize, (float) totalSize / 1024);
    }
    return totalSize;
}

static void analyzeApplicationExtension(FILE *file) {
    uint8_t buf[BUF_SIZE] = {0};

    readNbytes(file, buf, 1);
    unsigned blockSize = *buf;
    if (blockSize != 11) {
        analyzeError();
    }

    readNbytes(file, buf, 8);
    const char *applicationIdentifier = (const char *) buf;
    printf("Application Identifier: %s\n", applicationIdentifier);
    int isNetscapeExtension = memcmp(applicationIdentifier, "NETSCAPE", strlen(applicationIdentifier)) == 0;

    readNbytes(file, buf, 3);
    printf("Application Authentication Code: 0x%02X%02X%02X\n", buf[0], buf[1], buf[2]);

    if (isNetscapeExtension) {
        uint8_t buf[BUF_SIZE] = {0};
        readNbytes(file, buf, 1);
        unsigned blockSize = *buf;
        if (blockSize != 3) {
            analyzeError();
        }

        readNbytes(file, buf, 1);
        unsigned loopEnabled = *buf;
        printf("Loop Enabled: %u\n", loopEnabled);

        readNbytes(file, buf, 2);
        unsigned loopCount = buf[0] + (buf[1] << 8);
        printf("Loop Count: %u\n", loopCount);
    }
    analyzeDataBlock(file);
}

static void analyzeGraphicControlExtension(FILE *file) {
    uint8_t buf[BUF_SIZE] = {0};

    readNbytes(file, buf, 1);
    unsigned blockSize = *buf;
    if (blockSize != 4) {
        analyzeError();
    }

    readNbytes(file, buf, 1);
    unsigned disposalMethod = (*buf & 0b00011100) >> 2;
    printf("Disposal Method: %u\n", disposalMethod);
    unsigned userInputFlag = (*buf & 0b00000010) >> 1;
    printf("User Input Flag: %u\n", userInputFlag);
    unsigned transparentColorFlag = *buf & 0b00000001;
    printf("Transparent Color Flag: %u\n", transparentColorFlag);

    readNbytes(file, buf, 2);
    unsigned delayTime = buf[0] + (buf[1] << 8);
    printf("Delay Time: %u (%.2fs)\n", delayTime, (float) delayTime / 100);

    readNbytes(file, buf, 1);
    unsigned transparencyIndex = *buf;
    printf("Transparency Index: %u\n", transparencyIndex);

    analyzeDataBlock(file);
}

static void analyzeCommentExtension(FILE *file) {
    analyzeDataBlock(file);
}

static void analyzeImage(FILE *file, unsigned rawImageWidth, unsigned rawImageHeight) {
    uint8_t buf[BUF_SIZE] = {0};

    readNbytes(file, buf, 2);
    unsigned imageLeftPosition = buf[0] + (buf[1] << 8);
    printf("Image left Position: %u\n", imageLeftPosition);

    readNbytes(file, buf, 2);
    unsigned imageTopPosition = buf[0] + (buf[1] << 8);
    printf("Image Top Position: %u\n", imageTopPosition);

    readNbytes(file, buf, 2);
    unsigned imageWidth = buf[0] + (buf[1] << 8);
    printf("Image Width: %u\n", imageWidth);

    readNbytes(file, buf, 2);
    unsigned imageHeight = buf[0] + (buf[1] << 8);
    printf("Image Height: %u\n", imageHeight);

    readNbytes(file, buf, 1);
    unsigned localColorTableFlag = (*buf & 0b10000000) >> 7;
    printf("Local Color Table Flag: %u\n", localColorTableFlag);
    unsigned interlaceFlag = (*buf & 0b01000000) >> 6;
    printf("Interlace Flag: %u\n", interlaceFlag);
    unsigned sortFlag = (*buf & 0b00100000) >> 5;
    printf("Sort Flag: %u\n", sortFlag);
    unsigned sizeOfLocalColorTable = *buf & 0b00000111;
    unsigned realSizeOfLocalColorTable = 2 << sizeOfLocalColorTable;
    printf("Size of Local Color Table: %u (%u)\n", sizeOfLocalColorTable, realSizeOfLocalColorTable);

    if (localColorTableFlag) {
        printf("Local Color Table:\n");
        printf("\n");
        analyzeColorTable(file, realSizeOfLocalColorTable);
        printf("\n");
    }

    readNbytes(file, buf, 1);
    unsigned lzwMinimumCodeSize = *buf;
    printf("LZW Minimum Code Size: %u\n", lzwMinimumCodeSize);

    unsigned rawSizeImageDataLength = rawImageWidth * rawImageHeight * lzwMinimumCodeSize / 8;
    unsigned uncompressedImageDataLength = imageWidth * imageHeight * lzwMinimumCodeSize / 8;
    unsigned compressedImageDataLength = analyzeDataBlock(file);
    printf("Compression Rate: %.2f (%.2f)\n",
           (float) uncompressedImageDataLength / compressedImageDataLength,
           (float) rawSizeImageDataLength / compressedImageDataLength);
}

static void analyzeGif(FILE *file) {
    uint8_t buf[BUF_SIZE] = {0};

    // fixed-length header

    printf("================================\n");
    printf("Header\n");
    printf("================================\n");
    printf("\n");

    readNbytes(file, buf, 3);
    printf("Signature: %s\n", buf);

    readNbytes(file, buf, 3);
    printf("Version: %s\n", buf);

    printf("\n");

    // fixed-length Logical Screen Descriptor

    printf("================================\n");
    printf("Logical Screen Descriptor\n");
    printf("================================\n");
    printf("\n");

    readNbytes(file, buf, 2);
    unsigned logicalScreenWidth = buf[0] + (buf[1] << 8);
    printf("Logical Screen Width: %u\n", logicalScreenWidth);

    readNbytes(file, buf, 2);
    unsigned logicalScreenHeight = buf[0] + (buf[1] << 8);
    printf("Logical Screen Height: %u\n", logicalScreenHeight);

    readNbytes(file, buf, 1);
    unsigned globalColorTableFlag = (*buf & 0b10000000) >> 7;
    printf("Global Color Table Flag: %u\n", globalColorTableFlag);
    unsigned colorResolution = (*buf & 0b01110000) >> 4;
    unsigned realColorResolution = colorResolution + 1;
    printf("Color Resolution: %u (%u)\n", colorResolution, realColorResolution);
    unsigned sortFlag = (*buf & 0b00001000) >> 3;
    printf("Sort Flag: %u\n", sortFlag);
    unsigned sizeOfGlobalColorTable = *buf & 0b00000111;
    unsigned realSizeOfGlobalColorTable = 2 << sizeOfGlobalColorTable;
    printf("Size of Global Color Table: %u (%u)\n", sizeOfGlobalColorTable, realSizeOfGlobalColorTable);

    readNbytes(file, buf, 1);
    unsigned backgroundColorIndex = *buf;
    printf("Background Color Index: %u\n", backgroundColorIndex);

    readNbytes(file, buf, 1);
    unsigned pixelAspectRatio = *buf;
    double realPixelAspectRatio = pixelAspectRatio ? ((double) pixelAspectRatio + 15.0) / 64.0 : 1.0;
    printf("RGB Aspect Ratio: %u (%lf)\n", pixelAspectRatio, realPixelAspectRatio);

    unsigned rawImageDataLength = realColorResolution * logicalScreenWidth * logicalScreenHeight / 8;
    printf("Raw Image Size: %u (%.2fKB)\n", rawImageDataLength, (float) rawImageDataLength / 1024);

    if (globalColorTableFlag) {
        printf("Local Color Table:\n");
        printf("\n");
        analyzeColorTable(file, realSizeOfGlobalColorTable);
        printf("\n");
    }

    printf("\n");

    // Data Stream

    printf("================================\n");
    printf("Data Stream\n");
    printf("================================\n");
    printf("\n");

    for (;;) {
        readNbytes(file, buf, 1);
        unsigned identifier = *buf;
        switch (identifier) {
            case 0x21: {
                readNbytes(file, buf, 1);
                unsigned extensionLabel = *buf;
                switch (extensionLabel) {
                    case 0xFF:
                        printf("# Application Extension\n");
                        analyzeApplicationExtension(file);
                        printf("\n");
                        break;
                    case 0xF9:
                        printf("# Graphic Control Extension\n");
                        analyzeGraphicControlExtension(file);
                        printf("\n");
                        break;
                    case 0xFE:
                        printf("# Comment Extension\n");
                        analyzeCommentExtension(file);
                        printf("\n");
                        break;
                    default:
                        printf("# Unknown Extension: 0x%02X\n", extensionLabel);
                        break;
                }
                break;
            }
            case 0x2C: {
                printf("# Image\n");
                analyzeImage(file, logicalScreenWidth, logicalScreenHeight);
                printf("\n");
                break;
            }
            case 0x3B:
                printf("# Trailer\n");
                return;
            default:
                printf("# Unknown Block: 0x%02X\n", identifier);
                break;
        }
    }
}

void GifAnalyzer::showGifInfo(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        Logger::log(true, "GifAnalyzer : Could not open the file");
        exit(1);
    }
    analyzeGif(file);
    fclose(file);
}
