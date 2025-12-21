#!/bin/sh

CC=/usr/bin/clang
CXX=/usr/bin/clang++
cmake\
    -B build\
    --preset=default\
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold"\
    -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold"\
    -DCMAKE_BUILD_TYPE=Debug
