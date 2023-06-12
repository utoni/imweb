#!/usr/bin/env sh

set -e

MYDIR="$(realpath $(dirname ${0}))"
JOBCOUNT="${JOBCOUNT:-$(cat /proc/cpuinfo | grep processor | wc -l)}"
EMSCRIPTEN_BUILD_DIR="${EMSCRIPTEN_BUILD_DIR:-${MYDIR}/build-emscripten}"
NATIVE_BUILD_DIR="${NATIVE_BUILD_DIR:-${MYDIR}/build-native}"
CMAKE_COMMAND="$(command -v cmake || printf '%s' 'false')"

if [ -z "${EMSCRIPTEN_TOOLCHAIN_FILE}" ]; then
    if [ -r /usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake ]; then
        EMSCRIPTEN_TOOLCHAIN_FILE="/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake"
    elif [ -r /usr/share/emscripten/cmake/Modules/Platform/Emscripten.cmake ]; then
        EMSCRIPTEN_TOOLCHAIN_FILE="/usr/share/emscripten/cmake/Modules/Platform/Emscripten.cmake"
    else
        printf '%s: %s\n' "${0}" "No Emscripten toolchain found. Please set EMSCRIPTEN_TOOLCHAIN_FILE manually."
        exit 1
    fi
fi

printf '%s: %s\n' "${0}" "Building native imweb.."

mkdir -p "${NATIVE_BUILD_DIR}"
cd "${NATIVE_BUILD_DIR}"
${CMAKE_COMMAND} -DBUILD_EXAMPLES=ON "${MYDIR}"
${CMAKE_COMMAND} --build . --parallel "${JOBCOUNT}"
cd "${MYDIR}"

printf '%s: %s\n' "${0}" "Building imweb with emscripten toolchain.."
printf '%s: %s\n' "${0}" "CMake toolchain file '${EMSCRIPTEN_TOOLCHAIN_FILE}'."
printf '%s: %s\n' "${0}" "Build directory '${EMSCRIPTEN_BUILD_DIR}'"

if [ ! -r "${EMSCRIPTEN_TOOLCHAIN_FILE}" ]; then
    printf '%s: %s\n' "${0}" "Toolchain file '${EMSCRIPTEN_TOOLCHAIN_FILE}' does not exist or is not readable!" >&2
    exit 1
fi

mkdir -p "${EMSCRIPTEN_BUILD_DIR}"
cd "${EMSCRIPTEN_BUILD_DIR}"
CMAKE_C_FLAGS="${CMAKE_C_FLAGS} -DEMSCRIPTEN=1" ${CMAKE_COMMAND} --toolchain "${EMSCRIPTEN_TOOLCHAIN_FILE}" -DBUILD_EXAMPLES=ON "${MYDIR}"
${CMAKE_COMMAND} --build . --parallel "${JOBCOUNT}"
cd "${MYDIR}"
