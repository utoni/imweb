#!/usr/bin/env sh

set -e

MYDIR="$(realpath $(dirname ${0}))"
EMSCRIPTEN_BUILD_DIR="${EMSCRIPTEN_BUILD_DIR:-${MYDIR}/build-emscripten}"
NATIVE_BUILD_DIR="${NATIVE_BUILD_DIR:-${MYDIR}/build-native}"
PYTHON3_COMMAND="$(command -v python3 || true)"

if [ ! -d "${EMSCRIPTEN_BUILD_DIR}" ]; then
    printf '%s: %s\n' "${0}" "Directory '${EMSCRIPTEN_BUILD_DIR}' does not exist or is not readable.." >&2
    exit 1
fi

if [ ! -r "${EMSCRIPTEN_BUILD_DIR}/example.html" ]; then
    printf '%s: %s\n' "${0}" "File '${EMSCRIPTEN_BUILD_DIR}/example.html' not found. You need to build the examples it first!"
    exit 1
fi

if [ ! -x "${PYTHON3_COMMAND}" ]; then
    printf '%s: %s\n' "${0}" "Python executable is not in your PATH."
    exit
fi

{ cd "${NATIVE_BUILD_DIR}" && ./example; } &
NATIVE_PID=$!
{ sleep 1; $(command -v xdg-open || printf '%s' 'true') 'http://127.0.0.1:9999/'; } &

printf '%s: %s' "${0}" "Running Python3 HTTP Server inside '${EMSCRIPTEN_BUILD_DIR}'."
cd "${EMSCRIPTEN_BUILD_DIR}"
${PYTHON3_COMMAND} -m http.server 9999 --bind 127.0.0.1
