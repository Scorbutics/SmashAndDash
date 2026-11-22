#!/bin/bash
# Serves the web version of the game (tools/web/build.sh) to the devices of the local network.
# Usage : tools/web/serve.sh [build directory, default : build-web] (PORT=8000 by default)
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
BUILD=$(realpath -m "${1:-$ROOT/build-web}")
PORT=${PORT:-8000}
if [ ! -f "$BUILD/game/bin/index.html" ]; then
	echo "No web build in $BUILD : tools/web/build.sh first" >&2
	exit 1
fi
echo "Smash & Dash : http://localhost:$PORT on this computer"
# The address of this computer on the network (the interface of the default route)
address=$(ip -4 route get 1.1.1.1 2>/dev/null | awk '{ for (i = 1; i < NF; i++) if ($i == "src") print $(i + 1) }')
if [ -n "$address" ]; then
	echo "               http://$address:$PORT from the other devices of the network"
fi
exec python3 -m http.server "$PORT" --bind 0.0.0.0 --directory "$BUILD/game/bin"
