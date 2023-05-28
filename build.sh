#!/usr/bin/env sh

set -e

MYDIR="$(realpath $(dirname ${0}))"
CMAKE_TOOLCHAIN_FILE="${CMAKE_TOOLCHAIN_FILE:-/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake}"
BUILD_DIR="${BUILD_DIR:-${MYDIR}/build-emscripten}"
CMAKE_COMMAND="$(command -v cmake || printf '%s' 'false')"

printf '%s: %s\n' "${0}" "Building imweb with emscripten toolchain.."
printf '%s: %s\n' "${0}" "CMake toolchain file '${CMAKE_TOOLCHAIN_FILE}'."
printf '%s: %s\n' "${0}" "Build directory '${BUILD_DIR}'"

if [ ! -r "${CMAKE_TOOLCHAIN_FILE}" ]; then
    printf '%s: %s\n' "${0}" "Toolchain file '${CMAKE_TOOLCHAIN_FILE}' does not exist or is not readable!" >&2
    exit 1
fi

export CMAKE_TOOLCHAIN_FILE
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
${CMAKE_COMMAND} -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake "${MYDIR}"
${CMAKE_COMMAND} --build .
