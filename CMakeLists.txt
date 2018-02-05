cmake_minimum_required(VERSION 3.7)
project(BurstLinker)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#file(GLOB SOURCE_FILES *.cpp)

set(srcdir src)
set(thirdpartdir thirdpart)
set(exampledir example)

set(SOURCE_FILES
        ${srcdir}/ThreadPool.h
        ${srcdir}/GifEncoder.h ${srcdir}/GifEncoder.cpp
        ${srcdir}/GifAnalyzer.cpp ${srcdir}/GifAnalyzer.h
        ${srcdir}/GifBlockWriter.cpp ${srcdir}/GifBlockWriter.h
        ${srcdir}/KDTree.cpp ${srcdir}/KDTree.h
        ${srcdir}/LzwEncoder.cpp ${srcdir}/LzwEncoder.h
        ${srcdir}/Logger.cpp ${srcdir}/Logger.h

        ${srcdir}/Ditherer.h
        ${srcdir}/NoDitherer.cpp ${srcdir}/NoDitherer.h
        ${srcdir}/BayerDitherer.cpp ${srcdir}/BayerDitherer.h
        ${srcdir}/M2Ditherer.cpp ${srcdir}/M2Ditherer.h
        ${srcdir}/FloydSteinbergDitherer.cpp ${srcdir}/FloydSteinbergDitherer.h

        ${srcdir}/ColorQuantizer.h
        ${srcdir}/UniformQuantizer.cpp ${srcdir}/UniformQuantizer.h
        ${srcdir}/RandomQuantizer.cpp ${srcdir}/RandomQuantizer.h
        ${srcdir}/MedianCutQuantizer.cpp ${srcdir}/MedianCutQuantizer.h
        ${srcdir}/KMeansQuantizer.cpp ${srcdir}/KMeansQuantizer.h
        ${srcdir}/OctreeQuantizer.cpp ${srcdir}/OctreeQuantizer.h
        ${srcdir}/NeuQuant.cpp ${srcdir}/NeuQuant.h
        ${srcdir}/NeuQuantQuantizer.cpp ${srcdir}/NeuQuantQuantizer.h
        ${srcdir}/BurstLinker.cpp ${srcdir}/BurstLinker.h

        ${thirdpartdir}/stb_image.h
        )

set(BURSTLINKER_FILES
        ${srcdir}/Main.cpp
        )

set(EXAMPLE_FILES
        ${exampledir}/Main.cpp
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
