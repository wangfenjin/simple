#!/bin/sh

set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
toolchain_file="${script_dir}/contrib/ios.toolchain.cmake"
ios_output_root="${script_dir}/output-ios"
final_output_root="${script_dir}/output"
headers_dir="${ios_output_root}/headers"

build_variant() {
  platform="$1"
  build_dir="${script_dir}/build-ios-${platform}"
  install_dir="${ios_output_root}/${platform}"

  cmake -S "${script_dir}" -B "${build_dir}" -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE="${toolchain_file}" \
    -DPLATFORM="${platform}" \
    -DENABLE_BITCODE=1 \
    -DDEPLOYMENT_TARGET=8.0 \
    -DCMAKE_INSTALL_PREFIX=""

  cmake --build "${build_dir}" --config Release
  cmake --install "${build_dir}" --config Release --prefix "${install_dir}"
}

rm -rf "${ios_output_root}" "${final_output_root}/libsimple.xcframework" "${final_output_root}/dict"
mkdir -p "${headers_dir}" "${final_output_root}"
cp "${script_dir}"/src/*.h "${headers_dir}/"

build_variant OS64
build_variant SIMULATOR64
build_variant SIMULATORARM64

xcodebuild -create-xcframework \
  -library "${ios_output_root}/OS64/bin/libsimple.a" -headers "${headers_dir}" \
  -library "${ios_output_root}/SIMULATOR64/bin/libsimple.a" -headers "${headers_dir}" \
  -library "${ios_output_root}/SIMULATORARM64/bin/libsimple.a" -headers "${headers_dir}" \
  -output "${final_output_root}/libsimple.xcframework"

cp -R "${ios_output_root}/OS64/bin/dict" "${final_output_root}/dict"
