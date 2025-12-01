#!/bin/sh

CC=clang CXX=clang++ cmake -B build -G Ninja -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold" -DCMAKE_POLICY_VERSION_MINIMUM=3.5
