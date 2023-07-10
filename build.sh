#!/usr/bin/env sh

set -e

MYDIR="$(realpath $(dirname ${0}))"
JOBCOUNT="${JOBCOUNT:-$(cat /proc/cpuinfo | grep processor | wc -l)}"
EMSCRIPTEN_BUILD_DIR="${EMSCRIPTEN_BUILD_DIR:-${MYDIR}/build-emscripten}"
NATIVE_BUILD_DIR="${NATIVE_BUILD_DIR:-${MYDIR}/build-native}"
CMAKE_COMMAND="$(command -v cmake || printf '%s' 'false')"
CMAKE_ARGS=""

if [ -z "${EMSCRIPTEN_TOOLCHAIN_FILE}" ]; then
    if [ -r /usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake ]; then
        EMSCRIPTEN_TOOLCHAIN_FILE="/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake"
    elif [ -r /usr/share/emscripten/cmake/Modules/Platform/Emscripten.cmake ]; then
        EMSCRIPTEN_TOOLCHAIN_FILE="/usr/share/emscripten/cmake/Modules/Platform/Emscripten.cmake"
    else
        if [ ! -z "$(command -v emcc || printf '')" -a ! -z "$(command -v em++ || printf '')" -a ! -z "$(command -v emar || printf '')" -a ! -z "$(command -v emranlib || printf '')" ]; then
            CMAKE_ARGS="${CMAKE_ARGS} -DCMAKE_C_COMPILER=$(command -v emcc) -DCMAKE_CXX_COMPILER=$(command -v em++) -DCMAKE_AR=$(command -v emar) -DCMAKE_RANLIB=$(command -v emranlib) -DEMSCRIPTEN=1"
            printf '%s: %s\n' "${0}" 'No Emscripten toolchain found or set, using emcc/em++/emar/emranlib from your PATH.'
        else
            printf '%s: %s\n' "${0}" "No Emscripten toolchain found. Please set EMSCRIPTEN_TOOLCHAIN_FILE manually."
            exit 1
        fi
    fi
else
    EMSCRIPTEN_TOOLCHAIN_FILE="$(realpath ${EMSCRIPTEN_TOOLCHAIN_FILE})"
    CMAKE_ARGS="${CMAKE_ARGS} --toolchain ${EMSCRIPTEN_TOOLCHAIN_FILE}"
fi

cd "${MYDIR}"

cat <<EOF


    - ${0}: Building native imweb..
    - ${0}: Build directory '${NATIVE_BUILD_DIR}'


EOF
${CMAKE_COMMAND} -S . -B "${NATIVE_BUILD_DIR}" -DBUILD_EXAMPLES=ON "${MYDIR}"
${CMAKE_COMMAND} --build "${NATIVE_BUILD_DIR}" --parallel "${JOBCOUNT}" --clean-first --config Release --verbose

cat <<EOF


    - ${0}: Building imweb with emscripten toolchain..
    - ${0}: Build directory '${EMSCRIPTEN_BUILD_DIR}'


EOF
if [ -z "${EMSCRIPTEN_TOOLCHAIN_FILE}" ]; then
    printf '%s: %s\n' "${0}" 'Using emcc/em++/emar/emranlib from your PATH.'
else
    printf '%s: %s\n' "${0}" "CMake toolchain file '${EMSCRIPTEN_TOOLCHAIN_FILE}'."
    if [ ! -r "${EMSCRIPTEN_TOOLCHAIN_FILE}" ]; then
        printf '%s: %s\n' "${0}" "Toolchain file '${EMSCRIPTEN_TOOLCHAIN_FILE}' does not exist or is not readable!" >&2
        exit 1
    fi
fi
${CMAKE_COMMAND} -S . -B "${EMSCRIPTEN_BUILD_DIR}" ${CMAKE_ARGS} -DBUILD_EXAMPLES=ON "${MYDIR}"
${CMAKE_COMMAND} --build "${EMSCRIPTEN_BUILD_DIR}" --parallel "${JOBCOUNT}" --clean-first --config Release --verbose
