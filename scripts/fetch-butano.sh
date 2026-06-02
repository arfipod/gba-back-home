#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
BUTANO_REF="${BUTANO_REF:-21.6.0}"
mkdir -p vendor
if [[ ! -d vendor/butano/.git ]]; then
    git clone https://github.com/GValiente/butano.git vendor/butano
fi
cd vendor/butano
git fetch --tags --depth 1 origin "${BUTANO_REF}" || git fetch --tags --depth 1 origin master
git checkout "${BUTANO_REF}" || git checkout master
