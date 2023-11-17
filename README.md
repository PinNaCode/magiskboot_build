## magiskboot_build

a simple CMake-based build system for magiskboot

~~for static version please use the [static](../../tree/static) branch~~

Static builds is deprecated, while it's still possible, you need to patch the CMakeLists yourself if you want to build static version.

I just no longer have the interest to maintain the static version.

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
4. [Rust][Rust] (nightly)
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
sudo pacman -Su  # sync and upgrade all existing packages
sudo pacman -S --needed base-devel xz lz4 bzip2 dtc zlib pkgconf clang libc++ cmake ninja libbsd  # optional: lld
rustup component add rust-src  # install STD library source
````

</details>

<details><summary>macOS</summary>

#### macOS Big Sur (or higher verison)

install [Homebrew][Homebrew] first

````shell
brew update
brew upgrade  # upgrade all existing packages (optional)
brew install binutils xz lz4 bzip2 dtc zlib pkgconf llvm cmake ninja rustup-init
rustup-init  # install nightly Rust here
mkdir ~/.bin
for x in $HOMEBREW_PREFIX/opt/binutils/bin/*; do ln -s $x ~/.bin; done
unlink ~/.bin/strip  # avoids "Killed 9" error
export PATH=$HOMEBREW_PREFIX/opt/llvm/bin:~/.bin:$PATH
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

NOTE: MinGW port is not yet 100% complete, currently it can build and most functions are working.\
However, some POSIX functions in `src/winsup/*_compat.c` are still unimplemented, feel free to contribute by opening a Pull Request :)

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

#### LTO on Linux

If you want to perform LTO at the final link time, pass `-DUSE_LTO_LINKER_PLUGIN=ON` to CMake during configuring.

And you will need to install LLD.

Note: you may need to make sure your LLVM and LLD are sharing the same LLVM version as your nightly Rust.

### Generating source tarball

````shell
cmake -G Ninja -B build  # configure
cmake --build build -t package_source  # make a source package
````

you should be able to find your source package under the `build` folder

### Development

To quickly discard the current `build` directory and dirty `vendor/` submodule changes, please run `make clean`.

If you modify something in the Rust part, you will have to perform `rm build/libmagiskboot-rs.a` manually before rebuilding. (TODO: let CMake detect source changes)

Pass `-DCMAKE_BUILD_TYPE=Debug` to CMake instead of `Release` during configuring to make some error log more verbose.

For `vendor/` submodules with their name starting with `android_`, most of the patches are imported from [android-tools][android-tools], and don't always require updating.

### Special thanks to

- [android-tools][android-tools] developers for many code and inspiration of this repository
- [Magisk][Magisk] developers for the magiskboot utility
- all other used projects' developers (mentioned in the [Requirements](#requirements) section)

### See also

[xiaoxindada/magiskboot_ndk_on_linux][magiskboot_ndk_on_linux]: minimal build system for magiskboot with ondk on Linux

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
[Git-for-Windows]: https://gitforwindows.org/
[magiskboot_ndk_on_linux]: https://github.com/xiaoxindada/magiskboot_ndk_on_linux
