#!/usr/bin/env sh

MYDIR="$(dirname ${0})"
cd "${MYDIR}"

git subtree pull --squash --prefix=webgui 'https://github.com/jnmaloney/WebGui.git' master
