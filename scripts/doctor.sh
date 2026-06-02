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
cat <<EOF
Project: $(pwd)
LIBBUTANO: ${LIBBUTANO}
DEVKITPRO: ${DEVKITPRO:-missing}
DEVKITARM: ${DEVKITARM:-missing}
DISPLAY: ${DISPLAY:-unset}
EOF

missing=0

test -f "${LIBBUTANO}/butano.mak" || { echo "ERROR: butano.mak not found. Run scripts/fetch-butano.sh or use Docker."; missing=1; }

if [[ -z "${DEVKITARM:-}" && -z "${WONDERFUL_TOOLCHAIN:-}" ]]; then
    echo "ERROR: DEVKITARM or WONDERFUL_TOOLCHAIN must be set."
    missing=1
fi

if command -v arm-none-eabi-g++ >/dev/null; then
    arm-none-eabi-g++ --version | head -n 1
else
    echo "ERROR: arm-none-eabi-g++ not found in PATH."
    missing=1
fi

if command -v make >/dev/null; then
    make --version | head -n 1
else
    echo "ERROR: make not found in PATH."
    missing=1
fi

if command -v python >/dev/null; then
    python --version
elif command -v python3 >/dev/null; then
    python3 --version
else
    echo "ERROR: python or python3 not found in PATH."
    missing=1
fi

if command -v mgba-qt >/dev/null; then
    echo "mGBA Qt: $(command -v mgba-qt)"
else
    echo "WARN: mGBA Qt not found; scripts/run-rom.sh cannot launch the emulator."
fi

if (( missing != 0 )); then
    exit 1
fi

echo "OK"
