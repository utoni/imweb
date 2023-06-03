#!/usr/bin/env sh

set -e

MYDIR="$(realpath $(dirname ${0}))"
BUILD_DIR="${BUILD_DIR:-${MYDIR}/build-emscripten}"
PYTHON3_COMMAND="$(command -v python3 || true)"

if [ ! -d "${BUILD_DIR}" ]; then
    printf '%s: %s\n' "${0}" "Directory '${BUILD_DIR}' does not exist or is not readable.." >&2
    exit 1
fi

if [ ! -r "${BUILD_DIR}/imweb.html" ]; then
    printf '%s: %s\n' "${0}" "File '${BUILD_DIR}/imweb.html' not found. You need to build it first!"
    exit 1
fi

if [ ! -x "${PYTHON3_COMMAND}" ]; then
    printf '%s: %s\n' "${0}" "Python executable is not in your PATH."
    exit
fi

{ sleep 1; $(command -v xdg-open || printf '%s' 'true') 'http://127.0.0.1:9999/'; } &

printf '%s: %s' "${0}" "Running Python3 HTTP Server inside '${BUILD_DIR}'."
cd "${BUILD_DIR}"
${PYTHON3_COMMAND} -m http.server 9999 --bind 127.0.0.1
