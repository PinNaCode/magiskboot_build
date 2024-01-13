#!/bin/bash

# fail fast
set -e
set -o pipefail

if [ "$#" -lt 1 ]; then
    echo "No patch file specified, Stop."
    exit 0
fi

for _patch in "$@"; do
    echo "### applying ${_patch} ###"

    __vndproj="$(dirname ${_patch} | cut -d'/' -f2-)"
    __patch="$(realpath ${_patch})"

    pushd "src/$__vndproj"

    patch -p1 -i "${__patch}" --no-backup-if-mismatch -f

    popd
done

echo "Done!"
