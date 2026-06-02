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
if command -v mgba-qt >/dev/null; then
    echo "mGBA Qt: $(command -v mgba-qt)"
elif [[ -x /usr/games/mgba-qt ]]; then
    echo "mGBA Qt: /usr/games/mgba-qt"
elif command -v mgba >/dev/null; then
    echo "mGBA: $(command -v mgba)"
elif [[ -x /usr/games/mgba ]]; then
    echo "mGBA: /usr/games/mgba"
else
    echo "mGBA not found"
fi
echo "OK"
