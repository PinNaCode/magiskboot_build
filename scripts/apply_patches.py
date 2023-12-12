#!/usr/bin/env python3
"""
a cursed script doesn't use argparse
"""
import os
import os.path
import sys


files = []
list(map(lambda x: files.extend(x.strip().split()) if x else None, sys.argv[1:]))

if not files:
    print(f'Usage: {sys.argv[0]} <patch file 0> [patch file 1] ... [patch file N]')

top = os.getcwd()

for patch in filter(bool, map(str.strip, files)):
    os.chdir(top)
    if not os.path.isfile(patch) or not os.access(patch, os.R_OK):
        print(f'{patch!r} is not a file or inaccessible')
        assert False
    if patch.count(os.path.sep) < 2:
        print(f'path {patch!r} is invalid')
        assert False
    print(f'applying {patch!r}')
    pfile = os.path.abspath(patch)
    _p, vndproj, _f = patch.split(os.path.sep)
    os.chdir(os.path.join('vendor', vndproj))
    assert not os.system(f'patch -p1 -i {pfile!r} --no-backup-if-mismatch -f'), f'failed to apply {pfile!r}'

if files:
    print('Done!')
