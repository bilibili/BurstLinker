# BurstLinker

 [ ![Download](https://api.bintray.com/packages/succlz123/maven/burst-linker/images/download.svg) ](https://bintray.com/succlz123/maven/burst-linker/_latestVersion)

Idea from: [square/gifencoder](https://github.com/square/gifencoder)

BurstLinker is a simple and fast C++ GIF encode library. You can easily use it on Android, Windows, Linux or other platforms.

## Android platform

### Download

Gradle:

```
implementation 'com.bilibili:burst-linker:0.0.1'
```

### Build Environment

Android Studio 3.0.1

NDK r16

### Basic usage

``` java
int delayMs = 100;
String filePath = getExternalCacheDir() + File.separator + "out.gif";
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
                BurstLinker.DISABLE_DITHER, 0, 0, delayMs);
    }
} catch (GifEncodeException e) {
    e.printStackTrace();
} finally {
    burstLinker.release();
}
```

### Enable RenderScript Support

> This is an untested experimental feature

1. Git branch `/feature/render-script`
2. Gradle sync, It will generate the required ScriptC.cpp
3. Uncomment the line 64 of the /lib/CMakeLists.txt
4. Run

## Unix-like platform

1. Install [FreeImage](http://freeimage.sourceforge.net/) & [CMake](http://www.cmake.org/)
   - Mac  `brew install freeimage cmake`   
   - Debian `sudo apt-get install libfreeimage-dev cmake`
   - RedHat  `sudo yum install freeimage-devel cmake`
   - ArchLinux `sudo pacman -S freeimage cmake`

2. Build
   - `cd /BurstLinker/src`
   - `mkdir cmake-build-debug; cd cmake-build-debug`
   - `cmake ..`
   - `make`

3. Run
   - `./BurstLinker 100 1.jpg 2.jpg 3.jpg` 
   - See out.gif

## Windows platform

1. Install [Microsoft Visual Studio](https://www.visualstudio.com/) & [CMake](http://www.cmake.org/)

2. Build
   - `cd /BurstLinker/src`
   - `mkdir cmake-build-debug; cd cmake-build-debug`
   - `cmake ..`
   - Open BurstLinker.sln
   - Solution Explorer -> ALL_BUILD -> Build -> INSTALL -> Build

3. Run
   - `cd Debug`
   - `BurstLinker.exe 100 1.jpg 2.jpg 3.jpg` 
   - See out.gif

# Thanks

[square/gifencoder](https://github.com/square/gifencoder)

[waynejo/android-ndk-gif](https://github.com/waynejo/android-ndk-gif)

[lucent1090/MCCQ](https://github.com/lucent1090/MCCQ)

[luxiaoxun/KMeans-GMM-HMM](https://github.com/luxiaoxun/KMeans-GMM-HMM)

[SimonBoorer/Quantize](https://github.com/SimonBoorer/Quantize)

[dali-neuquant](https://code.google.com/archive/p/dali-neuquant)

[FFmpeg/FFmpeg](https://github.com/FFmpeg/FFmpeg)

[progschj/ThreadPool](https://github.com/progschj/ThreadPool)

# License

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