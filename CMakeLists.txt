cmake_minimum_required(VERSION 3.28)
project(code)

set(CMAKE_CXX_STANDARD 26)

add_executable(code main.cpp
        Decoder.h
        ALUs.h
        Memory.h
        RS.h
        Rob.h
        LSB.h
        RF.h
        Predictor.h
        clock.h
        CDB.h
        Rob.cpp
        RF.cpp
        RS.cpp
        Ins_Cache.h
        CDB.cpp
        Ins_Cache.cpp
        LSB.cpp
        clock.cpp)
