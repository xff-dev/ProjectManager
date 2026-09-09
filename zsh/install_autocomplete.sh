#!/usr/bin/env bash

set -e

COMP_DIR="${HOME}/.zfunc"
COMP_FILE="${COMP_DIR}/_pm"
ZSHRC="${HOME}/.zshrc"

mkdir -p "${COMP_DIR}"

if [[ ! -f "_pm" ]]; then
  echo "Error: _pm file not found in current directory."
  exit 1
fi

cp "_pm" "${COMP_FILE}"

if ! grep -q "fpath.*${COMP_DIR}" "${ZSHRC}"; then
  echo "" >>"${ZSHRC}"
  echo "fpath=(\"${COMP_DIR}\" \$fpath)" >>"${ZSHRC}"
  echo "autoload -Uz compinit && compinit" >>"${ZSHRC}"
fi

echo "Installation complete. Apply changes with: source ${ZSHRC}"
