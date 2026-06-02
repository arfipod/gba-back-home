#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ -z "${LIBBUTANO:-}" ]]; then
    if [[ -f vendor/butano/butano/butano.mak ]]; then
        LIBBUTANO=vendor/butano/butano
    else
        LIBBUTANO=/opt/butano/butano
    fi
fi
export LIBBUTANO
ROM="${1:-gba-back-home.gba}"
if [[ ! -f "$ROM" ]]; then
    scripts/build.sh
fi
if ! command -v mgba-qt >/dev/null; then
    echo "ERROR: mgba-qt not found in PATH. Install mGBA or run through the Docker/noVNC setup."
    exit 1
fi
export DISPLAY="${DISPLAY:-:1}"
exec mgba-qt "$ROM"
