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
exec mgba-qt "$ROM"
