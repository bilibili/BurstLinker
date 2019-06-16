# BurstLinker

 [ ![Download](https://api.bintray.com/packages/succlz123/maven/burst-linker/images/download.svg) ](https://bintray.com/succlz123/maven/burst-linker/_latestVersion)

Idea from: [square/gifencoder](https://github.com/square/gifencoder)

BurstLinker is a simple C++ GIF encode library.

## Download

### Android

Gradle:

```
implementation 'com.bilibili:burst-linker:latest-version'
```

#### Build Environment

Android Studio 3.4.1

NDK r20

#### Basic usage

``` java
int delayMs = 1000;
String filePath = getCacheDir() + File.separator + "out.gif";
BurstLinker burstLinker = new BurstLinker();

try {
    burstLinker.init(width, height, filePath);
    Bitmap colorBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    Canvas canvas = new Canvas(colorBitmap);
    Paint p = new Paint();
    int[] colors = new int[]{0xFFF00000, 0xFFFFFF00, 0xFFFFFFFF};
    for (int color : colors) {
        p.setColor(color);
        canvas.drawRect(0, 0, width, height, p);
        burstLinker.connect(colorBitmap, BurstLinker.OCTREE_QUANTIZER, 
                BurstLinker.NO_DITHER, 0, 0, delayMs);
    }
} catch (GifEncodeException e) {
    e.printStackTrace();
} finally {
    burstLinker.release();
}
```

#### Enable RenderScript Support

> This is an untested feature.

1. Choose the Git branch "/feature/render-script".
2. Sync Project with Gradle Files, It will generate the required file named "ScriptC_*.cpp".
3. Uncomment the line 64 of the "/lib/CMakeLists.txt".
4. Try this function.

### Linux & Mac

1. Install [CMake](http://www.cmake.org/)
   - Mac  `brew install cmake`   
   - ArchLinux `sudo pacman -S cmake`

2. Build
   - `cd /BurstLinker`
   - `mkdir cmake-build-debug; cd cmake-build-debug`
   - `cmake ..`
   - `make BurstLinker`

3. Run
   - `./BurstLinker 1000 1.jpg 2.jpg 3.jpg` 
   - See the "out.gif"

### Windows

1. Install [Microsoft Visual Studio](https://www.visualstudio.com/) & [CMake](http://www.cmake.org/)

2. Build
   - `cd /BurstLinker`
   - `mkdir cmake-build-debug; cd cmake-build-debug`
   - `cmake ..`
   - Open the "BurstLinker.sln"
   - Solution Explorer -> BurstLinker -> Build

3. Run
   - `cd Debug`
   - `BurstLinker.exe 1000 1.jpg 2.jpg 3.jpg` 
   - See the "out.gif"

## Samples

### Different quantizers & ditherers

- Original

![original](screenshot/lenna-original.png)

- Uniform + No

![uniform](screenshot/uniform.gif)

- MedianCut + No

![media-cut](screenshot/media-cut.gif)

- KMeans + No

![k-means](screenshot/k-means.gif)

- Random + No

![random](screenshot/random.gif)

- Octree + No

![octree](screenshot/octree.gif)

- NeuQuant - 10 + No

![neu-quant-10](screenshot/neu-quant-10.gif)

- NeuQuant - 1 + No

![neu-quant-1](screenshot/neu-quant-1.gif)

- Octree + M2

![uniform](screenshot/octree-m2.gif)

- Octree + Bayer

![octree-bayer](screenshot/octree-bayer.gif) 

- Octree + FloydSteinberg

![octree-floyd-steinberg](screenshot/octree-floyd-steinberg.gif)

### Encodes images with transparent channels

- Original

![bilibili](screenshot/bilibili.png)

- Octree + No + Default

  Display all Alpha channels greater than 0. (ARGB.a != 0)

![bilibili-octree](screenshot/bilibili-octree-default.gif)

- Octree + No + Ignored translucency 

  Display only  the Alpha channels equal to 255. (ARGB.a == 255) 

![bilibili-octree](screenshot/bilibili-octree-ignore.gif)

## Thanks

[square/gifencoder](https://github.com/square/gifencoder)

[waynejo/android-ndk-gif](https://github.com/waynejo/android-ndk-gif)

[lucent1090/MCCQ](https://github.com/lucent1090/MCCQ)

[luxiaoxun/KMeans-GMM-HMM](https://github.com/luxiaoxun/KMeans-GMM-HMM)

[SimonBoorer/Quantize](https://github.com/SimonBoorer/Quantize)

[dali-neuquant](https://code.google.com/archive/p/dali-neuquant)

[FFmpeg/FFmpeg](https://github.com/FFmpeg/FFmpeg)

[progschj/ThreadPool](https://github.com/progschj/ThreadPool)

## License

```
Copyright 2018 Bilibili

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```