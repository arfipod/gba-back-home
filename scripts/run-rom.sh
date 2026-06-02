#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
: "${LIBBUTANO:=/opt/butano/butano}"
export LIBBUTANO
ROM="${1:-gba-back-home.gba}"
if [[ ! -f "$ROM" ]]; then
    scripts/build.sh
fi
export DISPLAY="${DISPLAY:-:1}"
if command -v mgba-qt >/dev/null; then
    exec mgba-qt "$ROM"
elif [[ -x /usr/games/mgba-qt ]]; then
    exec /usr/games/mgba-qt "$ROM"
elif command -v mgba >/dev/null; then
    exec mgba "$ROM"
elif [[ -x /usr/games/mgba ]]; then
    exec /usr/games/mgba "$ROM"
else
    echo "ERROR: mGBA emulator not found. Install mgba-qt or use the Docker image." >&2
    exit 1
fi
