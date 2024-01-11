#!/bin/bash

#
# a very shitty bash script for testing basic functionalities of magiskboot
#
# dependencies: coreutils, file, wget, xxd
#

# fail fast
set -e
set -o pipefail

if [ "$#" -ne 1 -o ! -f "$1" ]; then
    echo "Usage: $0 <magiskboot binary>"
    exit 1
fi

if [[ "$1" == *.js ]]; then
    magiskboot=node\ $1
elif [[ ! -z "${EMULATOR}" ]]; then
    magiskboot=$EMULATOR\ $1
else
    magiskboot=$1
fi
echo -e "\n### magiskboot binary: $1 ###\n"
file $1

__workspace="$(mktemp -d -t magiskboot_test.XXXXXXXXXX)"
echo -e "\n### created temporary working directory: ${__workspace} ###\n"
cd "${__workspace}"
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    # set current directory to case sensitive
    set -x; chattr +C .; set +x
elif [[ "$(uname -s)" == "Windows_NT" ]]; then
    # Busybox-w32?
    set -x; fsutil.exe file setCaseSensitiveInfo . enable; set +x
fi
cleanup() {
    if [ -d "${__workspace}" ]; then
        cd ${TMPDIR:-/tmp}
        (rm -rf "${__workspace}" && echo -e "\n### removed working directory: ${__workspace} ###\n") || true
    fi
}
trap cleanup EXIT

mkdir -pv tmp

# prepare image files for test
echo -e "\n### downloading test images ###\n"
# TODO: add more image with different header version and update method
set -x
wget -nv https://github.com/dandelion64-Archives/redmi_dandelion_dump/raw/dandelion-user-11-RP1A.200720.011-V12.5.11.0.RCZMIXM-release-keys/boot.img.aa
wget -nv https://github.com/dandelion64-Archives/redmi_dandelion_dump/raw/dandelion-user-11-RP1A.200720.011-V12.5.11.0.RCZMIXM-release-keys/boot.img.ab
cat boot.img.aa boot.img.ab > boot.img
set +x
rm -vf boot.img.aa boot.img.ab
file boot.img

# check the downloaded image
sha512sum -c - << EOF
3b3b47a3ff94344c2ad8423fe9f90d1f3df77a9e2bd0b945e8b8c28015290aeb717d53d9c82abd96e9ceeafc982ff26a0e131d6820e54a753b55a07ce1016f5b  boot.img
EOF

# test: unpack
echo -e "\n### testing unpack ###\n"
set -x; $magiskboot unpack -n -h boot.img; set +x
file dtb header kernel ramdisk.cpio

# verify: unpack
check_results() {
    sha512sum -c - << EOF
0cd76b98aa1f55558bebd2cf1b58cdda15bdc4daaad2031e8191e5df9c8283259a071c32adcbb3dda32cb51ee2071d3acf19a6f79421142568a88a4997655abe  dtb
7aa1841bce0ff7ffba254f98c49dcbfefdbe6f3fe6ee86b1303fcee46234007c250ee8e3b36522f799fa3a1383398d2c6a04e7b6348c1788b58279ede823cf3a  header
f6bb3d1340accc23a6db7e74c311d81c6e0dd41da757ccd12c6095a20104314fb8413d3a0498bf141c1830bb9763c3897e4431379a6f88232dc283047e9c4a25  kernel
2bb406bf7665535cc8571205ea933113cdb7e7bf85e9f0773e2810c0ccc20e030729c82659d4b12564df70bf6c2dd46b4c30e833afbf4525ab60ad5b3dad50ea  ramdisk.cpio
EOF
}
check_results

# test: repack
echo -e "\n### testing repack ###\n"
set -x; $magiskboot repack boot.img tmp/new.img; set +x
file tmp/new.img

# verify: repack
pushd tmp
set -x; $magiskboot unpack -n -h new.img 2> /dev/null; set +x
check_results
popd

# test: compress & decompress
if [[ "$OSTYPE" == "darwin"* ]]; then
    set -x; dd if=/dev/urandom of=kitten bs=1m count=8; set +x
else
    set -x; dd if=/dev/urandom of=kitten bs=1M count=8; set +x
fi
__randfile_checksum="$(sha512sum kitten)"
compress_kitten() {
    # compress and capture console output
    local __conout="$((set -x; $magiskboot compress=$1 kitten 2>&1; set +x) | tee >(cat >&2))"
    # find out the output archive name
    if  grep -E -q 'Compressing to \[(.*)\]' <<< "${__conout}"; then
        # extract and return the output archive name
        sed -nE 's/Compressing to \[(.*)\]/\1/p' <<< "${__conout}"
        return 0
    fi

    # maybe some error occurred
    return 1
}
check_kitten() {
    file kitten
    sha512sum -c - <<< "${__randfile_checksum}"
}
check_kitten
for __method in gzip zopfli xz lzma bzip2 lz4 lz4_legacy lz4_lg; do
    echo -e "\n### testing ${__method} compression & decompression ###\n"

    __out="$(compress_kitten ${__method})"
    file "${__out}"
    rm -vf kitten
    set -x; $magiskboot decompress "${__out}"; set +x
    rm -vf "${__out}"
    check_kitten
done

# test: cpio ls
echo -e "\n### testing cpio ls ###\n"
cat > expected_out.txt << EOF
drwxr-xr-x	0	0	0 B	0:0	avb
drwxr-xr-x	0	0	0 B	0:0	debug_ramdisk
drwxr-xr-x	0	0	0 B	0:0	dev
-rw-r-----	0	0	4.57 KB	0:0	fstab.mt6762
-rw-r-----	0	0	4.57 KB	0:0	fstab.mt6765
-rwxr-x---	0	0	1.29 MB	0:0	init
drwxr-xr-x	0	0	0 B	0:0	mnt
drwxr-xr-x	0	0	0 B	0:0	proc
drwxr-xr-x	0	0	0 B	0:0	sys
-rw-r--r--	0	0	524 B	0:0	verity_key
EOF
set -x
$magiskboot cleanup 2> /dev/null
$magiskboot unpack boot.img 2> /dev/null
$magiskboot cpio ramdisk.cpio 'ls' | tee out.txt
diff -u out.txt expected_out.txt
set +x

# test: cpio add
echo -e "\n### testing cpio add ###\n"
set -x
echo 'Hello, world!' > foo
$magiskboot cpio ramdisk.cpio 'add 644 bar foo'
$magiskboot cpio ramdisk.cpio 'ls bar' | grep -E '\-rw-r--r--\s0\s0\s14 B\s0:0\sbar'
set +x

# TODO: add tests for more cpio sub-commands

# test: cpio extract
echo -e "\n### testing cpio extract ###\n"
cat > expected.list << EOF
ext
ext/avb
ext/avb/q-gsi.avbpubkey
ext/avb/r-gsi.avbpubkey
ext/avb/s-gsi.avbpubkey
ext/debug_ramdisk
ext/dev
ext/fstab.mt6762
ext/fstab.mt6765
ext/init
ext/mnt
ext/proc
ext/sys
ext/verity_key
EOF
set -x
$magiskboot cleanup 2> /dev/null
$magiskboot unpack boot.img 2> /dev/null
set +x
mkdir -pv ext && pushd ext
set -x; $magiskboot cpio ../ramdisk.cpio 'extract'; set +x
popd
set -x
find ext | sort > list.txt
diff -u list.txt expected.list
set +x
sha512sum -c - << EOF
ad623b7c491250d8cd001b1c0c07eced9fb79d68e1f208c3556d4d476349e7354c1cef58a27e1ee31fab02cc4f1929b86d04c979b6ac02b61d3c4ca0d3005894  ext/avb/q-gsi.avbpubkey
060f8a0173b53a133499c46fc0237fa24938fd7bfb3aa39b658a18efe0da826e3f0c8cf16c2691ef8a29990a0ed0344ed72164f8acf556ef3598bb944ce4ca6f  ext/avb/r-gsi.avbpubkey
1afb924f33cc62067166f2ab75a6345228fc448250b32f1fe34c2eec247301b1a3761cf1043c456b3979ad4b63df34232b133ae371b36046e5cd20136e1eab2a  ext/avb/s-gsi.avbpubkey
d070818df28c1f8614c8289c43b066fb9f630cb1e21628ca989980ab4838d3d45cc063a2461f1b6916ee2bdbe1d33f43297eaa9b60582b41d5f737c738f66b91  ext/fstab.mt6762
d070818df28c1f8614c8289c43b066fb9f630cb1e21628ca989980ab4838d3d45cc063a2461f1b6916ee2bdbe1d33f43297eaa9b60582b41d5f737c738f66b91  ext/fstab.mt6765
bc461ed94a3d1298d4d6ad310904d1b2e8ba705c2f3ad751d76454109cc7c69f951d33a72eff7f4f0e9e08e053c0fc9335dd6a87f06790098b752a0a3d42a321  ext/init
103c552f302d6d62d4b3973defc80f43eaf10d57dd32cb419f1f8384c4e805032d01aba32d7a2fe8d3e21ab0cd77818f6ecbdb3d0bc2e1d705c2a292d3021b20  ext/verity_key
EOF

# test: hexpatch
if [[ "$1" == *.js ]]; then
    echo -e "\n### FIXME: skipping hexpatch test for wasm ###\n"
else
    echo -e "\n### testing hexpatch ###\n"
    set -x; echo -ne '\xb0\x00\xba\xca\xfe\xd0\x0d' > test.bin; set +x
    file test.bin
    xxd test.bin
    set -x; $magiskboot hexpatch test.bin cafe badd; set +x
    file test.bin
    xxd test.bin
    set -x
    echo -ne '\xb0\x00\xba\xba\xdd\xd0\x0d' > expected.bin
    diff test.bin expected.bin
    set +x
fi

# TODO: add test for payload.bin extraction

# test: dtb
echo -e "\n### testing dtb ###\n"
set -x; $magiskboot dtb dtb test; set +x
# TODO: add test for dtb patch

# TODO: add test for AVB related functions

# test: sha1
echo -e "\n### testing sha1 ###\n"
__randfile_sha1="$(sha1sum kitten | cut -d ' ' -f 1 | tee >(cat >&2))"
set -x; ($magiskboot sha1 kitten | grep "${__randfile_sha1}"); set +x
