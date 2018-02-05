/* NeuQuant Neural-Net Quantization Algorithm
 * ------------------------------------------
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


#include <cstdint>
#include "NeuQuant.h"

using namespace blk;

int NeuQuant::getNetwork(int i, int j) {
    return network[i][j];
}

/* Initialise network in range (0,0,0) to (255,255,255) and set parameters */
void NeuQuant::initnet(unsigned char *thepic, int len, int sample) {
    int i;
    int *p;

    thepicture = thepic;
    lengthcount = len;
    samplefac = sample;

    for (i = 0; i < netsize; i++) {
        p = network[i];
        p[0] = p[1] = p[2] = (i << (netbiasshift + 8)) / netsize;
        freq[i] = intbias / netsize;    /* 1/netsize */
        bias[i] = 0;
    }
}

/* Unbias network to give byte values 0..255 and record position i to prepare for sort */
void NeuQuant::unbiasnet() {
    int i, j, temp;

    for (i = 0; i < netsize; i++) {
        for (j = 0; j < 3; j++) {
            /* OLD CODE: network[i][j] >>= netbiasshift; */
            /* Fix based on bug report by Juergen Weigert jw@suse.de */
            temp = (network[i][j] + (1 << (netbiasshift - 1))) >> netbiasshift;
            if (temp > 255) temp = 255;
            network[i][j] = temp;
        }
        network[i][3] = i;            /* record colour no */
    }
}

/* Output colour dither */
int NeuQuant::getColourMap(RGB out[]) {
    int index[netsize];
    for (int i = 0; i < netsize; i++) {
        index[network[i][3]] = i;
    }
    int k = 0;
    for (int j : index) {
        out[k].r = static_cast<uint8_t>(network[j][0]);
        out[k].g = static_cast<uint8_t>(network[j][1]);
        out[k].b = static_cast<uint8_t>(network[j][2]);
        out[k].index = static_cast<uint8_t>(k);
        k++;
    }
    return k;
}

/* Insertion sort of network and building of netindex[0..255] (to do after unbias) */
void NeuQuant::inxbuild() {
    int i, j, smallpos, smallval;
    int *p, *q;
    int previouscol, startpos;

    previouscol = 0;
    startpos = 0;
    for (i = 0; i < netsize; i++) {
        p = network[i];
        smallpos = i;
        smallval = p[1];            /* index on g */
        /* find smallest in i..netsize-1 */
        for (j = i + 1; j < netsize; j++) {
            q = network[j];
            if (q[1] < smallval) {        /* index on g */
                smallpos = j;
                smallval = q[1];    /* index on g */
            }
        }
        q = network[smallpos];
        /* swap p (i) and q (smallpos) entries */
        if (i != smallpos) {
            j = q[0];
            q[0] = p[0];
            p[0] = j;
            j = q[1];
            q[1] = p[1];
            p[1] = j;
            j = q[2];
            q[2] = p[2];
            p[2] = j;
            j = q[3];
            q[3] = p[3];
            p[3] = j;
        }
        /* smallval entry is now in position i */
        if (smallval != previouscol) {
            netindex[previouscol] = (startpos + i) >> 1;
            for (j = previouscol + 1; j < smallval; j++) netindex[j] = i;
            previouscol = smallval;
            startpos = i;
        }
    }
    netindex[previouscol] = (startpos + maxnetpos) >> 1;
    for (j = previouscol + 1; j < 256; j++) netindex[j] = maxnetpos; /* really 256 */
}

/* Search for BGR values 0..255 (after net is unbiased) and return colour index */
int NeuQuant::inxsearch(int b, int g, int r) {
    int i, j, dist, a, bestd;
    int *p;
    int best;

    bestd = 1000;        /* biggest possible dist is 256*3 */
    best = -1;
    i = netindex[g];    /* index on g */
    j = i - 1;        /* start at netindex[g] and work outwards */

    while ((i < netsize) || (j >= 0)) {
        if (i < netsize) {
            p = network[i];
            dist = p[1] - g;        /* inx key */
            if (dist >= bestd) i = netsize;    /* stop iter */
            else {
                i++;
                if (dist < 0) dist = -dist;
                a = p[0] - b;
                if (a < 0) a = -a;
                dist += a;
                if (dist < bestd) {
                    a = p[2] - r;
                    if (a < 0) a = -a;
                    dist += a;
                    if (dist < bestd) {
                        bestd = dist;
                        best = p[3];
                    }
                }
            }
        }
        if (j >= 0) {
            p = network[j];
            dist = g - p[1]; /* inx key - reverse dif */
            if (dist >= bestd) j = -1; /* stop iter */
            else {
                j--;
                if (dist < 0) dist = -dist;
                a = p[0] - b;
                if (a < 0) a = -a;
                dist += a;
                if (dist < bestd) {
                    a = p[2] - r;
                    if (a < 0) a = -a;
                    dist += a;
                    if (dist < bestd) {
                        bestd = dist;
                        best = p[3];
                    }
                }
            }
        }
    }
    return (best);
}

/* Search for biased BGR values */
int NeuQuant::contest(int b, int g, int r) {
    /* finds closest neuron (min dist) and updates freq */
    /* finds best neuron (min dist-bias) and returns position */
    /* for frequently chosen neurons, freq[i] is high and bias[i] is negative */
    /* bias[i] = gamma*((1/netsize)-freq[i]) */

    int i, dist, a, biasdist, betafreq;
    int bestpos, bestbiaspos, bestd, bestbiasd;
    int *p, *f, *n;

    bestd = ~(((int) 1) << 31);
    bestbiasd = bestd;
    bestpos = -1;
    bestbiaspos = bestpos;
    p = bias;
    f = freq;

    for (i = 0; i < netsize; i++) {
        n = network[i];
        dist = n[0] - b;
        if (dist < 0) {
            dist = -dist;
        }
        a = n[1] - g;
        if (a < 0) {
            a = -a;
        }
        dist += a;
        a = n[2] - r;
        if (a < 0) {
            a = -a;
        }
        dist += a;
        if (dist < bestd) {
            bestd = dist;
            bestpos = i;
        }
        biasdist = dist - ((*p) >> (intbiasshift - netbiasshift));
        if (biasdist < bestbiasd) {
            bestbiasd = biasdist;
            bestbiaspos = i;
        }
        betafreq = (*f >> betashift);
        *f++ -= betafreq;
        *p++ += (betafreq << gammashift);
    }
    freq[bestpos] += beta;
    bias[bestpos] -= betagamma;
    return (bestbiaspos);
}

/* Move neuron i towards biased (b,g,r) by factor alpha */
void NeuQuant::altersingle(int alpha, int i, int b, int g, int r) {
    int *n;

//	printf("New point %d: ", i);

    n = network[i];                /* alter hit neuron */
    *n -= (alpha * (*n - b)) / initalpha;
//	printf("%f, ", *n / 16.0);
    n++;
    *n -= (alpha * (*n - g)) / initalpha;
//  printf("%f, ", *n / 16.0);
    n++;
    *n -= (alpha * (*n - r)) / initalpha;
//  printf("%f\n", *n / 16.0);
}

/* Move adjacent neurons by precomputed alpha*(1-((i-j)^2/[r]^2)) in radpower[|i-j|] */
void NeuQuant::alterneigh(int rad, int i, int b, int g, int r) {
    int j, k, lo, hi, a;
    int *p, *q;

    lo = i - rad;
    if (lo < -1) {
        lo = -1;
    }
    hi = i + rad;
    if (hi > netsize) {
        hi = netsize;
    }

    j = i + 1;
    k = i - 1;
    q = radpower;
    while ((j < hi) || (k > lo)) {
        a = (*(++q));
        if (j < hi) {
//		  printf("New point %d: ", j);
            p = network[j];
            *p -= (a * (*p - b)) / alpharadbias;
//		  printf("%f, ", *p / 16.0);
            p++;
            *p -= (a * (*p - g)) / alpharadbias;
//		  printf("%f, ", *p / 16.0);
            p++;
            *p -= (a * (*p - r)) / alpharadbias;
//		  printf("%f\n", *p / 16.0);
            j++;
        }
        if (k > lo) {
//      printf("New point %d: ", k);
            p = network[k];
            *p -= (a * (*p - b)) / alpharadbias;
//      printf("%f, ", *p / 16.0);
            p++;
            *p -= (a * (*p - g)) / alpharadbias;
//      printf("%f, ", *p / 16.0);
            p++;
            *p -= (a * (*p - r)) / alpharadbias;
//      printf("%f\n", *p / 16.0);
            k--;
        }
    }
}

/* Main Learning Loop */
void NeuQuant::learn() {
    int i, j, b, g, r;
    int radius, rad, alpha, step, delta, samplepixels;
    unsigned char *p;
    unsigned char *lim;

    alphadec = 30 + ((samplefac - 1) / 3);
    p = thepicture;
    lim = thepicture + lengthcount;
    samplepixels = lengthcount / (3 * samplefac);
    delta = samplepixels / ncycles;
    alpha = initalpha;
    radius = initradius;

    rad = radius >> radiusbiasshift;
    if (rad <= 1) {
        rad = 0;
    }
    for (i = 0; i < rad; i++) {
        radpower[i] = alpha * (((rad * rad - i * i) * radbias) / (rad * rad));
    }

//	fprintf(stderr,"beginning 1D learning: initial radius=%d\n", rad);

    if ((lengthcount % prime1) != 0) {
        step = 3 * prime1;
    } else {
        if ((lengthcount % prime2) != 0) {
            step = 3 * prime2;
        } else {
            if ((lengthcount % prime3) != 0) {
                step = 3 * prime3;
            } else {
                step = 3 * prime4;
            }
        }
    }

    i = 0;
    while (i < samplepixels) {
        b = p[0] << netbiasshift;
        g = p[1] << netbiasshift;
        r = p[2] << netbiasshift;
        j = contest(b, g, r);

        altersingle(alpha, j, b, g, r);
        if (rad) {
            alterneigh(rad, j, b, g, r); /* alter neighbours */
        }

        p += step;
        if (p >= lim) {
            p -= lengthcount;
        }

        i++;
        if (i % delta == 0) {
            alpha -= alpha / alphadec;
            radius -= radius / radiusdec;
            rad = radius >> radiusbiasshift;
            if (rad <= 1) {
                rad = 0;
            }
            for (j = 0; j < rad; j++) {
                radpower[j] = alpha * (((rad * rad - j * j) * radbias) / (rad * rad));
            }
        }
    }
//	fprintf(stderr,"finished 1D learning: final alpha=%f !\n",((float)alpha)/initalpha);
}
