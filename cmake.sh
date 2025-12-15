#!/bin/sh

CC=/usr/bin/clang
CXX=/usr/bin/clang++
cmake\
    -B build\
    -G Ninja\
    --preset=default\
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++\
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold"\
    -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold"\
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5\
    -DCMAKE_BUILD_TYPE=Debug\
    -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
