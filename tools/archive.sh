#!/usr/bin/env bash

set -o errexit -o nounset -o pipefail

if [[ $# -lt 1 ]]; then
  echo "usage: ${0} TAG" 1>&2
  exit 1
fi
TAG=$1

root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." &>/dev/null && pwd)

if [[ -z "${GIT_DIR:-}" && -e "${root}/.jj/repo/store/git" ]]; then
  export GIT_DIR=${root}/.jj/repo/store/git
fi

# The prefix is chosen to match what GitHub generates for source archives
PREFIX="bazel-compile-commands-${TAG}"
ARCHIVE="bazel-compile-commands-${TAG}.tar.gz"
git archive --output "${ARCHIVE}" --prefix "${PREFIX}/" "${TAG}"
