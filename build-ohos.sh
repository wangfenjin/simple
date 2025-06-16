#!/bin/sh

current_dir=$(pwd)/$(dirname "$0")
build_dir="${current_dir}/build-ohos"
lib_prefix="${current_dir}/output" 
CMAKE="${OHOS_SDK}/native/build-tools/cmake/bin/cmake"
TOOLCHAIN_FILE="${OHOS_SDK}/native/build/cmake/ohos.toolchain.cmake"
OHOS_ARCH="arm64-v8a"

mkdir -p "$build_dir" && cd "$build_dir" || exit

"$CMAKE" \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
  -DCMAKE_INSTALL_PREFIX="$lib_prefix" \
  -DOHOS_ARCH="$OHOS_ARCH" \
  -DCMAKE_CXX_FLAGS="-Wno-unused-command-line-argument" \
  -DCMAKE_C_FLAGS="-Wno-unused-command-line-argument" \
  "$current_dir"

make
make install
