cmake_minimum_required(VERSION 3.4.1)

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

set(srcdir ../../src)

set(SOURCE_FILES
    ${srcdir}/GifEncoder.cpp
    ${srcdir}/GifBlockWriter.cpp
    ${srcdir}/KDTree.cpp
    ${srcdir}/LzwEncoder.cpp
    ${srcdir}/Logger.cpp

    ${srcdir}/NoDitherer.cpp
    ${srcdir}/BayerDitherer.cpp
    ${srcdir}/M2Ditherer.cpp
    ${srcdir}/FloydSteinbergDitherer.cpp

    ${srcdir}/OctreeQuantizer.cpp
    ${srcdir}/UniformQuantizer.cpp
    ${srcdir}/KMeansQuantizer.cpp
    ${srcdir}/MedianCutQuantizer.cpp
    ${srcdir}/RandomQuantizer.cpp
    ${srcdir}/NeuQuant.cpp
    ${srcdir}/NeuQuantQuantizer.cpp

    # ${srcdir}/GifAnalyzer.h
    ${srcdir}/ThreadPool.h

    src/main/cpp/BurstLinker.cpp
    # src/main/cpp/DithererWithRs.cpp
    # src/main/cpp/DisableDithererWithRs.cpp
    # src/main/cpp/BayerDithererWithRs.cpp
    # build/generated/source/rs/debug/ScriptC_linear.cpp
    )

add_library(BurstLinker SHARED ${SOURCE_FILES})

target_link_libraries(BurstLinker
                      log
                      # RScpp_static
                      jnigraphics
                      )

add_definitions (-Wno-format-security)
add_definitions (-Wno-delete-non-virtual-dtor)

# add_definitions (-D__Android__)

add_definitions (-D__AndroidLog__)
add_definitions (-D__SIMD__)