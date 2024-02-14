#!/bin/bash

# fail fast
set -e

set -x
git submodule update --init --depth=1 --single-branch -j $(nproc) "$@" src/*/
git -C src/Magisk submodule update --init --depth=1 --single-branch -j $(nproc) "$@" native/src/external/{cxx-rs,zopfli}/
set +x
