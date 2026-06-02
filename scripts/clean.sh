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
make clean
