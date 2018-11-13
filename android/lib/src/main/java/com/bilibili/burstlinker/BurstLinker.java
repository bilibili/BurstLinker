package com.bilibili.burstlinker;

import android.content.Context;
import android.graphics.Bitmap;
import android.text.TextUtils;

import java.io.File;
import java.util.List;

/**
 * Created by succlz123 on 2017/9/7.
 */
public class BurstLinker {

    static {
        System.loadLibrary("BurstLinker");
    }

    public final static int UNIFROM_QUANTIZER = 0;
    public final static int MEDIAN_CUT_QUANTIZER = 1;
    public final static int KMEANS_QUANTIZER = 2;
    public final static int RANDOM_QUANTIZER = 3;
    public final static int OCTREE_QUANTIZER = 4;
    public final static int NEU_QUANT_QUANTIZER = 5;

    public final static int NO_DITHER = 0;
    public final static int M2_DITHER = 1;
    public final static int BAYER_DITHER = 2;
    public final static int FLOYD_STEINBERG_DITHER = 3;

    public final static int CPU_COUNT = Runtime.getRuntime().availableProcessors();

    private String mUseRenderScript;

    private int mIgnoreTranslucency = 0;

    private int mWidth;
    private int mHeight;
    private int mThreadCount;
    private long mNative;

    private native long jniInit(String path, int width, int height, int loopCount, int threadCount);

    private native long jniDebugLog(long gifEncoder, boolean debug);

    private native String jniConnect(long gifEncoder, int quantizerType, int ditherType, int ignoreTranslucency,
                                     int left, int top, int delayMs, String cacheDir, Bitmap bitmap);

    private native String jniConnectArray(long gifEncoder, int quantizerType, int ditherType, int ignoreTranslucency,
                                          int left, int top, int delayMs, String cacheDir, Bitmap[] bitmaps);

    private native void jniRelease(long gifEncoder);

    public void init(int width, int height, String path) throws GifEncodeException {
        init(width, height, path, 0, 1, null);
    }

    public void init(int width, int height, String path, int threadCount) throws GifEncodeException {
        init(width, height, path, 0, threadCount, null);
    }

    public void init(int width, int height, String path, int loopCount, int threadCount) throws GifEncodeException {
        init(width, height, path, loopCount, threadCount, null);
    }

    public void init(int width, int height, String path, Context context) throws GifEncodeException {
        init(width, height, path, 0, 1, context);
    }

    public void init(int width, int height, String path, int loopCount, Context context) throws GifEncodeException {
        init(width, height, path, loopCount, 1, context);
    }

    /**
     * @param loopCount 0 = repeat forever
     * @param context   use for enable renderScript
     */
    private void init(int width, int height, String path, int loopCount, int threadCount, Context context) throws GifEncodeException {
        if (mNative != 0) {
            release();
        }
        if (TextUtils.isEmpty(path)) {
            throw new GifEncodeException("init path is empty");
        }
        mWidth = width;
        mHeight = height;
        if (context != null) {
            File cacheDir = context.getCacheDir();
            if (cacheDir != null) {
                mUseRenderScript = cacheDir.getAbsolutePath();
            }
            if (TextUtils.isEmpty(mUseRenderScript)) {
                mUseRenderScript = null;
            }
        }
        if (!TextUtils.isEmpty(mUseRenderScript) || threadCount < 1) {
            threadCount = 1;
        } else if (threadCount > 8) {
            threadCount = 8;
        }
        mThreadCount = threadCount;
        mNative = jniInit(path, width, height, loopCount, threadCount);
        if (mNative == 0) {
            throw new GifEncodeException("miao miao miao !!! init aborted");
        }
    }

    public void debugLog(boolean debug) {
        jniDebugLog(mNative, debug);
    }

    public void setIgnoreTranslucency(boolean ignoreTranslucency) {
        mIgnoreTranslucency = ignoreTranslucency ? 1 : 0;
    }

    public void connect(Bitmap bitmap, int quantizerType, int ditherType, int delayMs) throws GifEncodeException {
        connect(bitmap, quantizerType, ditherType, 0, 0, delayMs);
    }

    /**
     * @param bitmap only support ANDROID_BITMAP_FORMAT_RGBA_8888 & ANDROID_BITMAP_FORMAT_RGB_565
     */
    public void connect(Bitmap bitmap, int quantizerType, int ditherType, int left, int top, int delayMs) throws GifEncodeException {
        if (mNative == 0) {
            throw new GifEncodeException("please initialize first");
        }
        if (bitmap == null) {
            throw new GifEncodeException("bitmap is null");
        }
        if (delayMs < 0) {
            throw new GifEncodeException("delayMs < 0");
        }
        if (left + bitmap.getWidth() > mWidth || top + bitmap.getHeight() > mHeight) {
            throw new GifEncodeException("image does not fit in screen");
        }
        String nativeMessage = jniConnect(mNative, quantizerType, ditherType, mIgnoreTranslucency, left, top,
                delayMs, mUseRenderScript, bitmap);
        if (!TextUtils.isEmpty(nativeMessage)) {
            throw new GifEncodeException("native -> " + nativeMessage);
        }
    }

    public void connect(List<Bitmap> bitmaps, int quantizerType, int ditherType, int delayMs) throws GifEncodeException {
        connect(bitmaps, quantizerType, ditherType, 0, 0, delayMs);
    }

    public void connect(List<Bitmap> bitmaps, int quantizerType, int ditherType, int left, int top, int delayMs) throws GifEncodeException {
        if (mNative == 0) {
            throw new GifEncodeException("please first initialization");
        }
        if (bitmaps == null || bitmaps.size() <= 0) {
            throw new GifEncodeException("bitmaps is null or bitmaps is empty");
        }
        if (delayMs < 0) {
            throw new GifEncodeException("delayMs < 0");
        }
        for (Bitmap bitmap : bitmaps) {
            if (left + bitmap.getWidth() > mWidth || top + bitmap.getHeight() > mHeight) {
                throw new GifEncodeException("image does not fit in screen");
            }
        }
        String nativeMessage;
        if (mThreadCount <= 1) {
            for (Bitmap bitmap : bitmaps) {
                nativeMessage = jniConnect(mNative, quantizerType, ditherType, mIgnoreTranslucency, left, top,
                        delayMs, mUseRenderScript, bitmap);
                if (!TextUtils.isEmpty(nativeMessage)) {
                    throw new GifEncodeException("native -> " + nativeMessage);
                }
            }
        } else {
            Bitmap[] bitmapArray;
            int bitmapSize = bitmaps.size();
            if (mThreadCount > bitmapSize) {
                bitmapArray = bitmaps.toArray(new Bitmap[0]);
                nativeMessage = jniConnectArray(mNative, quantizerType, ditherType, mIgnoreTranslucency,
                        left, top, delayMs, mUseRenderScript, bitmapArray);
                if (!TextUtils.isEmpty(nativeMessage)) {
                    throw new GifEncodeException("native -> " + nativeMessage);
                }
            } else {
                int count = bitmapSize / mThreadCount;
                int remain = bitmapSize % mThreadCount;
                if (remain > 0) {
                    count++;
                }
                for (int i = 0; i < count; i++) {
                    int start = i * mThreadCount;
                    int end = start + mThreadCount;
                    if (remain > 0 && i == count - 1) {
                        end = start + remain;
                    }
                    List<Bitmap> processBitmaps = bitmaps.subList(start, end);
                    bitmapArray = processBitmaps.toArray(new Bitmap[0]);
                    nativeMessage = jniConnectArray(mNative, quantizerType, ditherType, mIgnoreTranslucency,
                            left, top, delayMs, mUseRenderScript, bitmapArray);
                    if (!TextUtils.isEmpty(nativeMessage)) {
                        throw new GifEncodeException("native -> " + nativeMessage);
                    }
                }
            }
        }
    }

    public void release() {
        if (mNative == 0) {
            return;
        }
        jniRelease(mNative);
        mNative = 0;
    }
}
