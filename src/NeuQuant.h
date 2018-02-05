/* NeuQuant Neural-Net Quantization Algorithm Interface
 * ----------------------------------------------------
 *
 * Copyright (c) 1994 Anthony Dekker
 *
 * NEUQUANT Neural-Net quantization algorithm by Anthony Dekker, 1994.
 * See "Kohonen neural networks for optimal colour quantization"
 * in "Network: Computation in Neural Systems" Vol. 5 (1994) pp 351-367.
 * for a discussion of the algorithm.
 * See also  http://members.ozemail.com.au/~dekker/NEUQUANT.HTML
 *
 * Any party obtaining a copy of these files from the author, directly or
 * indirectly, is granted, free of charge, a full and unrestricted irrevocable,
 * world-wide, paid up, royalty-free, nonexclusive right and license to deal
 * in this software and documentation files (the "Software"), including without
 * limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons who receive
 * copies from any such party to do so, with the only requirement being
 * that this copyright notice remain intact.
 */

#include "GifEncoder.h"

namespace blk {

/* Program Skeleton
 * ----------------
 * [select samplefac in range 1..30]
 * pic = (unsigned char*) malloc(3*width*height);
 * [read image from input file into pic]
 *
 * initnet(pic,3*width*height,samplefac);
 * learn();
 * unbiasnet();
 *
 * [write output image header, using getColourMap(f),possibly editing the loops in that function]
 *
 * inxbuild();
 *
 * [write output image using inxsearch(b,g,r)]
 */

    // For 256 colours, fixed arrays need 8kb, plus space for the image
    static const int netsize = 256;                     // number of colours used

    /* Four primes near 500 - assume no image has a length so large that it is divisible by all four primes */
    static const int prime1 = 499;
    static const int prime2 = 491;
    static const int prime3 = 487;
    static const int prime4 = 503;

    static const int minpicturebytes = (3 * prime4);    // minimum size for input image

    /* Network Definitions */
    static const int maxnetpos = 255;                   // netsize - 1
    static const int netbiasshift = 4;                  // bias for colour values
    static const int ncycles = 100;                     // no. of learning cycles

    /* Defs for freq and bias */
    static const int intbiasshift = 16;                 // bias for fractions
    static const int intbias = 65536;                   // 1 << intbiasshift
    static const int gammashift = 10;                   // gamma = 1024
    static const int gamma = 1024;                      // 1 << gammashift
    static const int betashift = 10;
    static const int beta = 64;                         // intbias >> betashift beta = 1/1024
    static const int betagamma = 65536;                 // intbias << (gammashift - betashift)

    /* Defs for decreasing radius factor */
    static const int initrad = 32;                      // netsize >> 3 for 256 cols, radius starts
    static const int radiusbiasshift = 6;               // at 32.0 biased by 6 bits
    static const int radiusbias = 64;                   // 1 << radiusbiasshift
    static const int initradius = 2048;                 // initrad * radiusbias and decreases by a
    static const int radiusdec = 30;                    // factor of 1/30 each cycle

    /* Defs for decreasing alpha factor */
    static const int alphabiasshift = 10;               // alpha starts at 1.0
    static const int initalpha = 1024;                  // 1 << alphabiasshift

    /* Radbias and alpharadbias used for radpower calculation */
    static const int radbiasshift = 8;
    static const int radbias = 256;                     // 1 << radbiasshift
    static const int alpharadbshift = 18;               // alphabiasshift + radbiasshift
    static const int alpharadbias = 262144;

    class NeuQuant {

    public:

        int getNetwork(int i, int j);

        // Initialise network in range (0,0,0) to (255,255,255) and set parameters
        void initnet(unsigned char *thepic, int len, int sample);

        // Unbias network to give byte values 0..255 and record position i to prepare for sort */
        void unbiasnet();

        // Output colour dither
        int getColourMap(RGB colorPalette[]);

        // Insertion sort of network and building of netindex[0..255] (to do after unbias)
        void inxbuild();

        // Search for BGR values 0..255 (after net is unbiased) and return colour index
        int inxsearch(int b, int g, int r);

        // Main Learning Loop
        void learn();

    private:

        int alphadec;                              // biased by 10 bits

        /* Types and Global Variables */

        unsigned char *thepicture;                // the input image itself
        int lengthcount;                          // lengthcount = H*W*3
        int samplefac;                            // sampling factor 1..30 */
        typedef int pixel_bgr[4];                 // BGRc
        pixel_bgr network[netsize];               // the network itself
        int netindex[256];                        // for network lookup - really 256
        int bias[netsize];                        // bias and freq arrays for learning
        int freq[netsize];                        // frequency array for learning
        int radpower[initrad];                    // radpower for precomputation

        int contest(int b, int g, int r);

        void altersingle(int alpha, int i, int b, int g, int r);

        void alterneigh(int rad, int i, int b, int g, int r);
    };
}
