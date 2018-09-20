cmake_minimum_required(VERSION 3.7)
project(BurstLinker)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#file(GLOB SOURCE_FILES *.cpp)

set(src_dir src)
set(third_part_dir third_part)
set(example_dir example)

set(SOURCE_FILES
        ${src_dir}/ThreadPool.h
        ${src_dir}/GifEncoder.h ${src_dir}/GifEncoder.cpp
        ${src_dir}/GifAnalyzer.cpp ${src_dir}/GifAnalyzer.h
        ${src_dir}/GifBlockWriter.cpp ${src_dir}/GifBlockWriter.h
        ${src_dir}/KDTree.cpp ${src_dir}/KDTree.h
        ${src_dir}/LzwEncoder.cpp ${src_dir}/LzwEncoder.h
        ${src_dir}/Logger.cpp ${src_dir}/Logger.h

        ${src_dir}/Ditherer.h
        ${src_dir}/NoDitherer.cpp ${src_dir}/NoDitherer.h
        ${src_dir}/BayerDitherer.cpp ${src_dir}/BayerDitherer.h
        ${src_dir}/M2Ditherer.cpp ${src_dir}/M2Ditherer.h
        ${src_dir}/FloydSteinbergDitherer.cpp ${src_dir}/FloydSteinbergDitherer.h

        ${src_dir}/ColorQuantizer.h
        ${src_dir}/UniformQuantizer.cpp ${src_dir}/UniformQuantizer.h
        ${src_dir}/RandomQuantizer.cpp ${src_dir}/RandomQuantizer.h
        ${src_dir}/MedianCutQuantizer.cpp ${src_dir}/MedianCutQuantizer.h
        ${src_dir}/KMeansQuantizer.cpp ${src_dir}/KMeansQuantizer.h
        ${src_dir}/OctreeQuantizer.cpp ${src_dir}/OctreeQuantizer.h
        ${src_dir}/NeuQuant.cpp ${src_dir}/NeuQuant.h
        ${src_dir}/NeuQuantQuantizer.cpp ${src_dir}/NeuQuantQuantizer.h
        ${src_dir}/BurstLinker.cpp ${src_dir}/BurstLinker.h

        ${third_part_dir}/stb_image.h
        )

set(BURSTLINKER_FILES
        ${src_dir}/Main.cpp
        )

set(EXAMPLE_FILES
        ${example_dir}/Main.cpp
        )

add_executable(BurstLinker ${SOURCE_FILES} ${BURSTLINKER_FILES})
add_executable(Example ${SOURCE_FILES} ${EXAMPLE_FILES})

message(STATUS "CMAKE_SYSTEM_INFO_FILE:   ${CMAKE_SYSTEM_INFO_FILE}")
message(STATUS "CMAKE_SYSTEM_NAME:        ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR:   ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_SYSTEM:             ${CMAKE_SYSTEM}")

message(STATUS "CMAKE_BINARY_DIR:         ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "PROJECT_SOURCE_DIR:       ${PROJECT_SOURCE_DIR}")
message(STATUS "PROJECT_BINARY_DIR:       ${PROJECT_BINARY_DIR}")

IF (CMAKE_SYSTEM MATCHES "Linux")
    find_package(Threads)
    target_link_libraries(BurstLinker ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(Example ${CMAKE_THREAD_LIBS_INIT})
ELSEIF (CMAKE_SYSTEM MATCHES "Darwin")

ELSEIF (CMAKE_SYSTEM MATCHES "Windows")

ENDIF ()
