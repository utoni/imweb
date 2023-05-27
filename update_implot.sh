#!/usr/bin/env sh

MYDIR="$(dirname ${0})"
cd "${MYDIR}"

git subtree pull --squash --prefix=implot 'https://github.com/epezent/implot.git' master
