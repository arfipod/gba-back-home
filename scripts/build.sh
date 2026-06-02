#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
: "${LIBBUTANO:=/opt/butano/butano}"
export LIBBUTANO
make -j"$(nproc)"
