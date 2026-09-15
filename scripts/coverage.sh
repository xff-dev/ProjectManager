#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build-coverage"
REPORT_DIR="${BUILD_DIR}/coverage"

GCOVR="${GCOVR:-gcovr}"
if ! command -v "$GCOVR" >/dev/null 2>&1 && [ -x "$HOME/.local/bin/gcovr" ]; then
  GCOVR="$HOME/.local/bin/gcovr"
fi

cmake -S "$REPO_ROOT" -B "$BUILD_DIR" -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug >/dev/null
make -C "$BUILD_DIR" -j"$(nproc)" unit_tests pm >/dev/null

cd "$REPO_ROOT"
HOME="$(mktemp -d)" TERM=ghostty "$BUILD_DIR/pm" help >/dev/null 2>&1 || true
"$BUILD_DIR/unit_tests" "~[terminal]"

mkdir -p "$REPORT_DIR"
"$GCOVR" -r "$REPO_ROOT" --object-directory "$BUILD_DIR" --filter 'src/' --html --html-details --output "$REPORT_DIR/index.html"
"$GCOVR" -r "$REPO_ROOT" --object-directory "$BUILD_DIR" --filter 'src/' --print-summary

echo
echo "HTML report: $REPORT_DIR/index.html"
