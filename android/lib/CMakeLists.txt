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

set(src_dir ../../src)

set(SOURCE_FILES
    ${src_dir}/GifEncoder.cpp
    ${src_dir}/GifBlockWriter.cpp
    ${src_dir}/KDTree.cpp
    ${src_dir}/LzwEncoder.cpp
    ${src_dir}/Logger.cpp

    ${src_dir}/NoDitherer.cpp
    ${src_dir}/BayerDitherer.cpp
    ${src_dir}/M2Ditherer.cpp
    ${src_dir}/FloydSteinbergDitherer.cpp

    ${src_dir}/OctreeQuantizer.cpp
    ${src_dir}/UniformQuantizer.cpp
    ${src_dir}/KMeansQuantizer.cpp
    ${src_dir}/MedianCutQuantizer.cpp
    ${src_dir}/RandomQuantizer.cpp
    ${src_dir}/NeuQuant.cpp
    ${src_dir}/NeuQuantQuantizer.cpp

    # ${src_dir}/GifAnalyzer.h
    ${src_dir}/ThreadPool.h

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