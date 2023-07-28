## magiskboot_build

a simple CMake-based build system for magiskboot

for static version please use the [static](../../tree/static) branch

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
3. [LLD][LLD] (optional)
4. [Rust][Rust] (nightly)
5. [Cargo][Cargo]
6. [CMake][CMake]

for Linux systems, please install [libbsd][libbsd] as well

please ensure you have installed the above softwares before building

there are examples for some popular operating systems/distributions:

#### Ubuntu 22.04 (jammy)

install nightly Rust via [rustup][rustup] first

````shell
sudo apt update
sudo apt upgrade  # upgrade all existing packages (optional)
# replace clang-15 with clang if your Ubuntu is too old, do the same for lld if you want to use it
sudo apt install build-essential lzma-dev liblzma-dev liblz4-dev libbz2-dev libfdt-dev \
                 zlib1g-dev pkgconf clang-15 cmake ninja-build libbsd-dev  # optional: lld-15
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

#### Alpine Linux (edge)

install nightly Rust via [rustup][rustup] (can be installed with `apk`) first

````shell
sudo apk update
sudo apk upgrade  # upgrade all existing packages (recommended)
sudo apk add build-base xz-dev lz4-dev bzip2-dev dtc-dev zlib-dev \
        pkgconf clang cmake samurai libbsd-dev  # optional: lld
rustup component add rust-src  # install STD library source
````

#### archlinux

install nightly Rust via [rustup][rustup] (can be installed with `pacman`) first

````shell
sudo pacman -Su  # sync and upgrade all existing packages
sudo pacman -S --needed base-dev xz lz4 bzip2 dtc zlib pkgconf clang cmake ninja libbsd  # optional: lld
rustup component add rust-src  # install STD library source
````

#### Termux

````shell
apt update
apt upgrade  # upgrade all existing packages (optional)
apt install tur-repo  # for nightly Rust package
apt install build-essentials liblzma liblz4 libbz2 dtc zlib pkg-config \
            clang rustc-nightly rust-src-nightly cmake ninja libbsd  # optional: lld
````

### Build & Install

````shell
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release  # configure
cmake --build build -j $(nproc)  # build
./build/magiskboot  # running
# install to system (or to a directory specified by the `DESTDIR' environment variable)
sudo cmake --install install
````

#### LTO

If you want to perform LTO at the final link time, pass `-DUSE_LTO_LINKER_PLUGIN=ON` to CMake during configuring.

And you will need to install LLD.

Note: you may need to make sure your LLVM and LLD are sharing the same LLVM version as your nightly Rust.

### Generating source tarball

````shell
cmake -G Ninja -B build  # configure
cmake --build build -t package_source  # make a source package
````

you should be able to find your source package under the `build` folder

### Special thanks to

- [android-tools][android-tools] developers for many code and inspiration of this repository
- [Magisk][Magisk] developers for the magiskboot utility
- all other used projects' developers (mentioned in the [Requirements](#Requirements) section)

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

### See also

[svoboda18/magiskboot](https://github.com/svoboda18/magiskboot.git): a dirty Windows port with custom GNU Make based build system
