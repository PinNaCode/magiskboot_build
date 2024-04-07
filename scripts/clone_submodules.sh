#!/bin/bash

# fail fast
set -e

set -x
git submodule update --init --depth=1 --single-branch -j $(nproc) "$@" \
    src/{Magisk,android_{core,fmtlib,libbase,logging},corrosion,mman-win32,msvc_getline}/
git -C src/Magisk submodule update --init --depth=1 --single-branch -j $(nproc) "$@" native/src/external/{cxx-rs,zopfli}/
set +x
