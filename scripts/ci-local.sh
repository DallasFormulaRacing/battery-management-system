#!/usr/bin/env zsh
set -e

act -j build \
  --container-architecture linux/amd64 \
  --reuse \
  --pull=false \
  "$@"

# make sure to chmod before running this vro