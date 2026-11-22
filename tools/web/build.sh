#!/bin/bash
# Builds the web version of the game (WebAssembly) : <build>/game/bin/index.html (and index.js, .wasm, .data).
# Usage : tools/web/build.sh [build directory, default : build-web]
# Needs the Emscripten SDK : "source <emsdk>/emsdk_env.sh" before, or EMSDK=<emsdk directory>. The first build also
# builds the SDL3 libraries for the web (external/ska/tools/web/build_deps.sh, in <build>/deps).
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
BUILD=$(realpath -m "${1:-$ROOT/build-web}")

if ! command -v emcmake > /dev/null; then
	if [ -n "${EMSDK:-}" ] && [ -f "$EMSDK/emsdk_env.sh" ]; then
		source "$EMSDK/emsdk_env.sh" > /dev/null 2>&1
	else
		echo "Emscripten is not found : source <emsdk>/emsdk_env.sh, or set EMSDK=<emsdk directory>" >&2
		exit 1
	fi
fi

DEPS="$BUILD/deps"
if [ ! -f "$DEPS/lib/libSDL3_mixer.a" ]; then
	"$ROOT/external/ska/tools/web/build_deps.sh" "$DEPS"
fi

emcmake cmake -S "$ROOT" -B "$BUILD/game" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$DEPS" -DCMAKE_FIND_ROOT_PATH="$DEPS" > /dev/null
# The game files are packed when the page is linked : linked again at each build
rm -f "$BUILD/game/bin/index.html"
cmake --build "$BUILD/game" -j"$(nproc)" --target smash_and_dash_web
echo "The game : $BUILD/game/bin/index.html (tools/web/serve.sh to play it from the other devices of the network)"
