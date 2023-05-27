#!/usr/bin/env sh

MYDIR="$(dirname ${0})"
cd "${MYDIR}"

git subtree pull --squash --prefix=imgui 'https://github.com/ocornut/imgui.git' master
