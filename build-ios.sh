#!/bin/sh

current_dir=$(pwd)/$(dirname "$0")
build_dir="${current_dir}/build-ios"
lib_prefix="${current_dir}/output"

cmake "$current_dir" -G Xcode -DCMAKE_TOOLCHAIN_FILE=contrib/ios.toolchain.cmake \
 -DPLATFORM=OS64COMBINED -DENABLE_BITCODE=1 \
 -DCMAKE_INSTALL_PREFIX="" -B "$build_dir" \
 -DDEPLOYMENT_TARGET=8.0

cd "$build_dir" || exit

cmake --build "$build_dir" --config Release
cmake --install "$build_dir" --config Release --prefix "${lib_prefix}"
