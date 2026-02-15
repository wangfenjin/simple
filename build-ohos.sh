#!/bin/sh

set -eu

current_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
build_dir="${current_dir}/build-ohos"
lib_prefix="${current_dir}/output"
: "${OHOS_SDK:?OHOS_SDK is required}"
CMAKE="${OHOS_SDK}/native/build-tools/cmake/bin/cmake"
TOOLCHAIN_FILE="${OHOS_SDK}/native/build/cmake/ohos.toolchain.cmake"
OHOS_ARCH="${OHOS_ARCH:-arm64-v8a}"

if [ ! -x "${CMAKE}" ]; then
  echo "CMake not found at ${CMAKE}" >&2
  exit 1
fi

if [ ! -f "${TOOLCHAIN_FILE}" ]; then
  echo "OHOS toolchain file not found at ${TOOLCHAIN_FILE}" >&2
  exit 1
fi

mkdir -p "${build_dir}"

"${CMAKE}" \
  -S "${current_dir}" \
  -B "${build_dir}" \
  -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
  -DCMAKE_INSTALL_PREFIX="${lib_prefix}" \
  -DOHOS_ARCH="${OHOS_ARCH}" \
  -DCMAKE_CXX_FLAGS="-Wno-unused-command-line-argument" \
  -DCMAKE_C_FLAGS="-Wno-unused-command-line-argument"

"${CMAKE}" --build "${build_dir}" --parallel
"${CMAKE}" --install "${build_dir}"
