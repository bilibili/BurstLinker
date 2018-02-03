# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# add_library(cpufeatures STATIC ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)

# SET(RENDER_SCRIPT_HEADERS_PATH
#    ${ANDROID_NDK}/toolchains/renderscript/prebuilt/${ANDROID_HOST_TAG}/platform/rs
# )

# INCLUDE_DIRECTORIES(
#    ${RENDER_SCRIPT_HEADERS_PATH}/cpp
#    ${RENDER_SCRIPT_HEADERS_PATH}/scriptc
#    ${RENDER_SCRIPT_HEADERS_PATH}
#    )

# link_directories(${ANDROID_NDK}/toolchains/renderscript/prebuilt/${ANDROID_HOST_TAG}/platform/${ANDROID_SYSROOT_ABI})

add_library( # Sets the name of the library.
             BurstLinker

             # Sets the library as a shared library.
             SHARED

             ${SOURCE_FILES}
             # Provides a relative path to your source file(s).
             src/main/cpp/BurstLinker.cpp

             ../../src/GifEncoder.cpp
             ../../src/GifBlockWriter.cpp
             ../../src/KDTree.cpp
             ../../src/LzwEncoder.cpp
             ../../src/GifLogger.cpp

             ../../src/Ditherer.cpp
             ../../src/DisableDitherer.cpp
             ../../src/BayerDitherer.cpp
             ../../src/M2Ditherer.cpp
             ../../src/FloydSteinbergDitherer.cpp

             ../../src/ColorQuantizer.cpp
             ../../src/OctreeQuantizer.cpp
             ../../src/UniformQuantizer.cpp
             ../../src/KMeansQuantizer.cpp
             ../../src/MedianCutQuantizer.cpp
             ../../src/RandomQuantizer.cpp
             ../../src/NeuQuant.cpp
             ../../src/NeuQuantQuantizer.cpp

             # ../../src/GifAnalyzer.h
             ../../src/ThreadPool.h

             # src/main/cpp/DithererWithRs.cpp
             # src/main/cpp/DisableDithererWithRs.cpp
             # src/main/cpp/BayerDithererWithRs.cpp
             # build/generated/source/rs/debug/ScriptC_linear.cpp
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log
              )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       BurstLinker

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       # RScpp_static
                       jnigraphics
                       )

add_definitions (-Wno-format-security)
add_definitions (-Wno-delete-non-virtual-dtor)

# add_definitions (-D__Android__)

add_definitions (-D__AndroidLog__)
add_definitions (-D__Other__)