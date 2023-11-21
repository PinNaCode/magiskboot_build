## magiskboot_build

a simple portable CMake-based build system for magiskboot

### Requirements

magiskboot itself depends on the following libraries:

1. [LZMA][LZMA]
2. [lz4][lz4]
3. [bzip2][bzip]
4. [libfdt][libfdt]
5. [zlib][zlib]

for build-time dependencies:

1. [pkg-config][pkg-config]
2. [Clang][Clang]
3. [LLD][LLD] (optional on Linux)
4. [Rust][Rust] (**nightly**)
5. [Cargo][Cargo]
6. [CMake][CMake]
7. [Libc++][Libcxx]

for Linux systems, please install [libbsd][libbsd] as well

please ensure you have installed the above softwares before building

there are examples for some popular operating systems/distributions:

<details><summary>Linux</summary>

#### Ubuntu 22.04 (jammy)

install nightly Rust via [rustup][rustup] first

````shell
sudo apt update
sudo apt upgrade  # upgrade all existing packages (optional)
# replace clang-15, libc++-15-dev, libc++abi-15-dev with clang, libc++-dev and libc++abi-dev
#  if your Ubuntu is too old, do the same for lld if you want to use it
sudo apt install build-essential lzma-dev liblzma-dev liblz4-dev libbz2-dev libfdt-dev \
                 zlib1g-dev pkgconf clang-15 libc++-15-dev libc++abi-15-dev cmake \
                 ninja-build libbsd-dev  # optional: lld-15
rustup component add rust-src  # install STD library source
# the following cmds are only for Ubuntu jammy:
mkdir ~/.bin
ln -s `which clang-15` ~/.bin/clang
ln -s `which clang++-15` ~/.bin/clang++

# optional:
# ln -s `which lld-15` ~/.bin/lld

# finally 
export PATH=~/.bin:$PATH
````

#### Alpine Linux (edge)

install nightly Rust via [rustup][rustup] (can be installed with `apk`) first

````shell
sudo apk update
sudo apk upgrade  # upgrade all existing packages (recommended)
sudo apk add build-base xz-dev lz4-dev bzip2-dev dtc-dev zlib-dev \
        pkgconf clang libc++-dev cmake samurai libbsd-dev  # optional: lld
rustup component add rust-src  # install STD library source
````

#### archlinux

install nightly Rust via [rustup][rustup] (can be installed with `pacman`) first

````shell
sudo pacman -S --needed base-devel xz lz4 bzip2 dtc zlib pkgconf clang libc++ cmake ninja libbsd  # optional: lld
rustup component add rust-src  # install STD library source
````

##### See also

[xiaoxindada/magiskboot_ndk_on_linux](https://github.com/xiaoxindada/magiskboot_ndk_on_linux): minimal build system for magiskboot with ondk on Linux

</details>

<details><summary>macOS</summary>

#### macOS Big Sur (or higher verison)

install [Homebrew][Homebrew] first

````shell
brew update
brew upgrade  # upgrade all existing packages (optional)
brew install xz lz4 bzip2 dtc zlib pkg-config cmake ninja rustup-init
rustup-init  # install nightly Rust here
````

</details>

<details><summary>Android</summary>

#### Termux

Building on Termux is no longer supported.

while it's still possible, you will need to compile a nightly Rust toolchain from source, by yourself.

NOTE: the one provided by `tur-repo` is outdated and no longer work in the latest Termux distribution.

Recommend directly using `libmagiskboot.so` extracted from the Magisk APK, it's just a static ELF program.

````shell
apt update
apt upgrade  # upgrade all existing packages (optional)
apt install tur-repo  # for nightly Rust package
apt install build-essentials liblzma liblz4 libbz2 dtc zlib pkg-config \
            clang lld rustc-nightly rust-src-nightly cmake ninja libbsd
````

</details>

<details><summary>Windows</summary>

#### Windows (MinGW)

> **Note**
> A minor amount of POSIX functions in `src/winsup/*_compat.c` are currently stubbed and no-op (e.g. chmod, chown, mknod), but it shouldn't cause too much trouble for magiskboot to work.
>
> However, if you know a better way to do this, please feel free to open a Pull Request to change it :)

Install [MSYS2][MSYS2] first, use the MINGW64 Terminal. Change the setting for `mintty.exe` and give it administrator permission (needed for using native symlinks).

don't forget to set this environtment variable to allow symlinks to work properly: `export MSYS=winsymlinks:native` (required for the build I guess)

install nightly Rust via [rustup][rustup] (Please choose the GNU ABI)

````shell
pacman -Syu  # upgrade all existing packages (optional, you may need to do this for multiple times)
pacman -S base-devel cygpath mingw-w64-x86_64-{xz,lz4,bzip2,dtc,zlib,pkgconf,clang,lld,cmake,libc++,ninja}
rustup component add rust-src  # install STD library source
````

There is also an old MinGW port, it works great:

[svoboda18/magiskboot](https://github.com/svoboda18/magiskboot.git): a dirty Windows port with custom GNU Make based build system

Or you can try the Cygwin port below (it already kinda works).

#### Cygwin (WIP)

To build for Cygwin, you need to compile a nightly Rust toolchain from source, for more info: [Cygwin Rust porting](https://gist.github.com/ookiineko/057eb3a91825313caeaf6d793a33b0b2)

Currently Cygwin Rust has no host tools support, so you have patch the CMakeLists and make it cross-compile for Cygwin.

NOTE: This project doesn't support cross-compiling for now, but it should be easy to get patched to support that.

You will also need to compile the LLVM/Clang from source to add Cygwin target, see: [my unofficial cygports](https://github.com/orgs/ookiineko-cygpkg/repositories)

</details>

### Download

For prebuilt binaries, go check [GitHub Releases](../../releases) for selected CI builds.

### Build & Install

Clone this repository using Git with this option: `--recurse-submodules`

Or you if already have a cloned repository without using that option, run: `git submodule update --init --recursive`

````shell
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release  # configure
cmake --build build -j $(nproc)  # build
./build/magiskboot  # running
# install to system (or to a directory specified by the `DESTDIR' environment variable)
sudo cmake --install install
````

If you prefer a statically linked binary (optional), pass `-DPREFER_STATIC_LINKING=ON` to CMake while configuring, make sure your distribution provided you the static version of the [depended libraries](#requirements), otherwise you'll run into configure errors and you have to change your distribution or try to compile the static version of those libraries yourself and install them to your system for the static build.

#### LTO

If you want to perform LTO at the final link time, pass `-DUSE_LTO_LINKER_PLUGIN=ON` to CMake during configuring.

And you will need to install LLD.

Note: you may need to make sure your LLVM and LLD are sharing the same LLVM version as your nightly Rust.

### Generating source tarball

````shell
cmake -G Ninja -B build -DNO_TARGETS_OR_DEPS=ON  # configure
cmake --build build -t package_source  # make a source package
````

Make sure you passed `-DNO_TARGETS_OR_DEPS=ON` to CMake while configuring, which will allow creating source tarball without installing the [build dependencies](#requirements).

you should be able to find your source package under the `build` folder

### FAQ

#### Help, my Rust build has failed

Since we are using nightly, random breakage is expected. Report it by filing an [Issue](../../issues).

However, if your build fails with something like this:

````text
[32/62] Building Rust static library libmagiskboot-rs.a
error: the `-Z` flag is only accepted on the nightly channel of Cargo, but this is the `stable` channel
See https://doc.rust-lang.org/book/appendix-07-nightly-rust.html for more information about Rust release channels.
````

That means you have installed a non-nightly Rust toolchain, and that is not supported currently, since the upstream uses nightly.

You may also run into issues if you don't have `rust-src` (STD library sources) installed, just install it like in the [Requirements](#requirements) section or simply follow the hint cargo gives you.

#### Is this thing using the latest Magisk source?

This project is very similiar to [android-tools][android-tools] which just maintains a set of patches on top of a specific upstream Magisk commit and require manual adaption for compiling with newer version source.

Although I may update the version once in a while, [Pull requests](../../pulls) are welcome.

#### Eh, so my platform is not supported by your sh*t

This project aims to be portable, and it should possible to get ported to new platforms, as long as your platform have the stuffs mentioned in the [Requirements](#requirements) section.

Not? Check out the [Cygwin](#cygwin-wip) platform, maybe you can try porting those dependencies yourself.
Or, check the dirty MinGW port mentioned in [Windows (MinGW)](#windows-mingw) section if you prefer that way. But these are not covered by this project.

### Development

To quickly discard the current `build` directory and dirty `vendor/` submodule changes, please run `make clean`.

To temporarily disable `vendor/` projects patching, re-configure with `-DPATCH_VENDOR_PROJECTS=OFF` (useful if you are patching them manually).
To enable it again, use `-DPATCH_VENDOR_PROJECTS=ON` (Note this will clean up changes in `vendor/` modules and re-apply all the patches).

If you modify something in the Rust part, you will have to perform `rm build/libmagiskboot-rs.a` manually before rebuilding. (TODO: let CMake detect source changes)

Pass `-DCMAKE_BUILD_TYPE=Debug` to CMake instead of `Release` during configuring to make some error log more verbose.

For `vendor/` submodules with their name starting with `android_`, most of the patches are imported from [android-tools][android-tools], and don't always require updating.

When syncing upstream `vendor/{android_libbase,Magisk}` changes, here is a few things to check:
  * `build.py` changes
  * `{,{base,boot}/}Android.mk` updates
  * whether if `external` projects are deleted or moved, update accordingly for excluded directory in the end of `CMakeLists.txt`
  * switch to `winsup` stub for MinGW for if any new file or spot reference to any of the following standard libc identifiers:
    * struct DIR
    * struct dirent
    * mkdir
    * {open,read,close,rewind,seek,tell}dir
    * open
    * fopen
    * fstat

### Special thanks to

- [android-tools][android-tools] developers for many code and inspiration of this repository
- [Magisk][Magisk] developers for the magiskboot utility
- all other used projects' developers (mentioned in the [Requirements](#requirements) section)

[pkg-config]: https://www.freedesktop.org/wiki/Software/pkg-config/
[LZMA]: https://tukaani.org/lzma/
[lz4]: https://lz4.github.io/lz4/
[bzip]: http://www.bzip.org/
[libfdt]: https://github.com/kernkonzept/libfdt.git
[zlib]: https://zlib.net/
[Clang]: https://clang.llvm.org/
[LLD]: https://lld.llvm.org/
[Rust]: https://www.rust-lang.org/
[Cargo]: https://doc.rust-lang.org/cargo/
[CMake]: https://cmake.org/
[Magisk]: https://github.com/topjohnwu/Magisk.git
[android-tools]: https://github.com/nmeum/android-tools
[libbsd]: https://libbsd.freedesktop.org/
[rustup]: https://rustup.rs/
[Homebrew]: https://brew.sh/
[Libcxx]: https://libcxx.llvm.org/
[MSYS2]: https://www.msys2.org/
