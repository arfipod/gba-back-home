#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
: "${LIBBUTANO:=/opt/butano/butano}"
cat <<EOF
Project: $(pwd)
LIBBUTANO: ${LIBBUTANO}
DEVKITPRO: ${DEVKITPRO:-missing}
DEVKITARM: ${DEVKITARM:-missing}
DISPLAY: ${DISPLAY:-unset}
EOF

test -f "${LIBBUTANO}/butano.mak" || { echo "ERROR: butano.mak not found. Run scripts/fetch-butano.sh or use Docker."; exit 1; }
command -v arm-none-eabi-g++ >/dev/null && arm-none-eabi-g++ --version | head -n 1
command -v make >/dev/null && make --version | head -n 1
command -v python >/dev/null && python --version
(command -v mgba-qt >/dev/null && echo "mGBA Qt: $(command -v mgba-qt)") || echo "mGBA Qt not found"
echo "OK"
