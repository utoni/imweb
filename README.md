# What?

A fusion of Dear Imgui, Implot and WebGui.
My use cases: [nDPId](https://github.com/utoni/nDPId) and my [Cheat SDK](https://github.com/utoni/KernelCheatFramework).

# Build and Run (emscripten)

To create JS/WASM output compatible with modern web browsers, emscripten is used as CC/CXX wrapper.

## Using helper scripts

```shell
./build.sh
./run.sh
```

## Using CMake

```shell
mkdir ./my-build-dir && cd ./my-build-dir
cmake -DCMAKE_TOOLCHAIN_FILE:-/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake ..
cmake --build .
make run
```

# But Toni, I do not have emscripten installed. Can I still run this crap?

Sure. As of the nature on how emscripten works, you're still able to compile&run C++ applications natively w/o any web browser.
