cmake_minimum_required(VERSION 3.7)
project(BurstLinker)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#file(GLOB SOURCE_FILES *.cpp)

set(SOURCE_FILES
        Main.cpp
        ThreadPool.h
        GifEncoder.cpp GifEncoder.h
        GifAnalyzer.cpp GifAnalyzer.h
        GifBlockWriter.cpp GifBlockWriter.h
        KDTree.cpp KDTree.h
        LzwEncoder.cpp LzwEncoder.h
        GifLogger.cpp GifLogger.h

        Ditherer.cpp Ditherer.h
        DisableDitherer.cpp DisableDitherer.h
        BayerDitherer.cpp BayerDitherer.h
        M2Ditherer.cpp M2Ditherer.h
        FloydSteinbergDitherer.cpp FloydSteinbergDitherer.h

        ColorQuantizer.cpp ColorQuantizer.h
        UniformQuantizer.cpp UniformQuantizer.h
        RandomQuantizer.cpp RandomQuantizer.h
        MedianCutQuantizer.cpp MedianCutQuantizer.h
        KMeansQuantizer.cpp KMeansQuantizer.h
        OctreeQuantizer.cpp OctreeQuantizer.h
        NeuQuant.cpp NeuQuant.h
        NeuQuantQuantizer.cpp NeuQuantQuantizer.h
        BurstLinker.cpp BurstLinker.h)

add_executable(BurstLinker ${SOURCE_FILES})

message(STATUS "CMAKE_SYSTEM_INFO_FILE:   ${CMAKE_SYSTEM_INFO_FILE}")
message(STATUS "CMAKE_SYSTEM_NAME:        ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR:   ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_SYSTEM:             ${CMAKE_SYSTEM}")

message(STATUS "CMAKE_BINARY_DIR:         ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "PROJECT_SOURCE_DIR:       ${PROJECT_SOURCE_DIR}")
message(STATUS "PROJECT_BINARY_DIR:       ${PROJECT_BINARY_DIR}")

find_package(Threads)
target_link_libraries(BurstLinker ${CMAKE_THREAD_LIBS_INIT})

IF (CMAKE_SYSTEM MATCHES "Linux")
    target_link_libraries(BurstLinker freeimageplus)

    #    find_package(OpenCV REQUIRED)
    #    target_link_libraries(BurstLinker ${OpenCV_LIBS})
ELSEIF (CMAKE_SYSTEM MATCHES "Darwin")
    include_directories(/usr/local/Cellar/freeimage/3.17.0_1/include/)
    target_link_libraries(BurstLinker /usr/local/Cellar/freeimage/3.17.0_1/lib/libfreeimageplus.3.dylib)

    #set(${OpenCV_INCLUDE_DIRS} /usr/local/Cellar/opencv3/3.2.0/include/)
    #set(${OpenCV_LIBS} /usr/local/Cellar/opencv3/3.2.0/lib/)

    #    set(OpenCV_DIR /usr/local/Cellar/opencv3/3.2.0/share/OpenCV/)
    #    find_package(OpenCV REQUIRED)
    #    target_link_libraries(BurstLinker ${OpenCV_LIBS})
ELSEIF (CMAKE_SYSTEM MATCHES "Windows")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdpart/FreeImage/x86/include)

    target_link_libraries(BurstLinker ${CMAKE_CURRENT_SOURCE_DIR}/../thirdpart/FreeImage/x86/lib/FreeImage.lib)
    target_link_libraries(BurstLinker ${CMAKE_CURRENT_SOURCE_DIR}/../thirdpart/FreeImage/x86/lib/FreeImagePlus.lib)

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../thirdpart/FreeImage/x86/bin/ DESTINATION "${CMAKE_BINARY_DIR}/Debug")
ENDIF ()

add_definitions(-D__Other__)
