#pragma version(1)
#pragma rs java_package_name(com.bilibili.burstlinker)

const static int maxDistance = 255 * 255 * 255;

int quantizerSize;
uchar quantizerColors[768];

uint32_t __attribute__((kernel)) invert(uint32_t rgb) {
	uchar b = (rgb >> 16) & 0xFF;
    uchar g = (rgb >> 8) & 0xFF;
    uchar r = (rgb) & 0xFF;
    int nearestCentroidDistance = maxDistance;
    int nearestIndices = 0;
    int index = 0;
    int size = quantizerSize;
    for (int k = 0; k < size; k = k + 3) {
        int nr = r - quantizerColors[k];
        int ng = g - quantizerColors[k + 1];
        int nb = b - quantizerColors[k + 2];
        int colorDistance = (nr * nr) + (ng * ng) + (nb * nb);
        // int colorDistance = 2 * abs(nr) + 4 * abs(ng) + 3 * abs(nb);
        if (colorDistance < nearestCentroidDistance) {
            nearestCentroidDistance = colorDistance;
            nearestIndices = index;
        };
        index++;
    }
    return nearestIndices;
}
