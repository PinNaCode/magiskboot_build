## magiskboot_build

![Magisk Version Badge](https://img.shields.io/badge/Magisk%20version-v26.4-green?logo=magisk)
![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/ookiineko/magiskboot_build/ci.yaml?logo=githubactions&logoColor=orange&label=CI%20build)
![Last Upstream Sync Date](https://img.shields.io/github/gist/last-commit/e6601e2e396dfb74e5ae289fcf2ef16b?logo=git&label=Last%20upstream%20sync&color=blue)

a simple portable CMake-based build system for magiskboot

### Requirements

magiskboot itself depends on the following libraries:

1. [LZMA][LZMA]
2. [lz4][lz4]
3. [bzip2][bzip]
4. [zlib][zlib]

for build-time dependencies:

1. [pkg-config][pkg-config]
2. [Clang][Clang]
3. [LLD][LLD] (optional)
4. [Rust][Rust]
5. [Cargo][Cargo]
6. [CMake][CMake]
7. [Libc++][Libcxx] (optional, see [this part](#help-my-build-has-failed))

please ensure you have installed the above softwares before building

there are examples for some popular operating systems/distributions:

<details><summary>Linux</summary>

#### Ubuntu 22.04 (jammy)

Make sure to read [this part](#help-my-build-has-failed) before you start.

````shell
sudo apt update
sudo apt upgrade  # upgrade all existing packages (optional)
# replace clang-15, libc++-15-dev, libc++abi-15-dev with clang, libc++-dev and libc++abi-dev
#  if your Ubuntu is too old, do the same for lld if you want to use it
sudo apt install build-essential lzma-dev liblzma-dev liblz4-dev libbz2-dev \
                 zlib1g-dev pkg-config clang-15 libc++-15-dev libc++abi-15-dev cmake \
                 ninja-build rustc cargo  # optional: lld-15
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

````shell
sudo apk update
sudo apk upgrade  # upgrade all existing packages (recommended)
sudo apk add build-base linux-headers xz-dev lz4-dev bzip2-dev \
             zlib-dev pkgconf clang libc++-dev cmake \
             samurai rust cargo  # optional: lld
````

#### archlinux

````shell
sudo pacman -S --needed base-devel xz lz4 bzip2 zlib pkgconf \
                        clang libc++ cmake ninja rust  # optional: lld
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
brew install xz lz4 bzip2 zlib pkg-config cmake ninja rust
````

</details>

<details><summary>Android</summary>

#### Termux

Termux build is not actively tested by CI.

````shell
apt update
apt upgrade  # upgrade all existing packages (optional)
apt install build-essentials liblzma liblz4 libbz2 zlib pkg-config \
            clang lld rust cmake ninja
````

You can also directly use the `libmagiskboot.so` extracted from the Magisk APK, it's just a static ELF program.

</details>

<details><summary>Windows</summary>

#### Windows (MinGW)

> **Note**
> A minor amount of POSIX functions in `src/winsup/*_compat.c` are currently stubbed and no-op (e.g. chmod, chown, mknod), but it shouldn't cause too much trouble for magiskboot to work.
>
> However, if you know a better way to do this, please feel free to open a Pull Request to change it :)

Install [MSYS2][MSYS2] first, and change the setting of `mintty.exe` to grant it with administrator privileges (needed for using native symlinks in some conditions).

don't forget to set this environtment variable to allow symlinks to work properly: `export MSYS=winsymlinks:native` (required for the build I guess)

````shell
pacman -Syu  # upgrade all existing packages (optional, you may need to do this for multiple times)
pacman -S --needed base-devel pactoys
pacboy -S --needed {xz,lz4,bzip2,zlib,pkgconf,clang,lld,cmake,libc++,ninja,rust}:p
````

There is also an old MinGW port, it works great:

[svoboda18/magiskboot](https://github.com/svoboda18/magiskboot.git): a dirty Windows port with custom GNU Make based build system

#### Cygwin (early test)

> **Note**
> Cygwin support is not actively tested currently

To build for Cygwin, you need to compile a Rust toolchain from source, for more info: [Cygwin Rust porting](https://gist.github.com/ookiineko/057eb3a91825313caeaf6d793a33b0b2)

Currently Cygwin Rust has no host tools support, so you have patch the CMakeLists and make it cross-compile for Cygwin.

NOTE: This project doesn't support cross-compiling for now, but it should be easy to get patched to support that.

You will also need to compile the LLVM/Clang from source to add Cygwin target, see: [my unofficial cygports](https://github.com/orgs/ookiineko-cygpkg/repositories)

Finally, read [this part](#help-my-build-has-failed) before you start.

</details>

### Download

For prebuilt binaries, go check [GitHub Releases](../../releases) for selected CI builds (TODO: run some automated testsuites in CI).

### Build & Install

First get and extract the latest source tarball (called `magiskboot_<COMMIT_ID>_<VERCODE>-src.tar.xz`) from [Github Releases](../../releases).

Or clone this repository using Git with this option: `--recurse-submodules`. (If you already have a cloned repository without using that option, run: `git submodule update --init --recursive` instead)

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

Note: you may need to make sure your LLVM and LLD are sharing the same LLVM version as Rust.

### Generating source tarball

````shell
cmake -G Ninja -B build -DNO_TARGETS_OR_DEPS=ON  # configure
cmake --build build -t package_source  # make a source package
````

Make sure you passed `-DNO_TARGETS_OR_DEPS=ON` to CMake while configuring, which will allow creating source tarball without installing the [build dependencies](#requirements).

you should be able to find your source package under the `build` folder

### FAQ

#### Help, my build has failed

A recent version of Rust is needed since the upstream is actually using a custom nightly Rust compiler,

if your distribution's Rust compiler is too old (e.g. Debian/Ubuntu based), manually applying some extra patches is probably needed,

you can try to pick some known ones from [here](patches-contrib), or report it by filing an [Issue](../../issues) if that doesn't help.

If you need to build with [Libstdc++][Libstdcxx] instead of [Libc++][Libcxx], pass `-DWITHOUT_LIBCXX=ON` to CMake during configuring, also apply [this patch](patches-contrib/0029-workaround-support-building-with-libstdcxx.patch).

#### Is this thing using the latest Magisk source?

This project is very similiar to [android-tools][android-tools] which just maintains a set of patches on top of a specific upstream Magisk commit and require manual adaption for compiling with newer version source.

Although I may update the version once in a while, [Pull requests](../../pulls) are welcome.

#### Eh, so my platform is not supported by your sh*t

This project aims to be portable, and it should be possible to port it to new platforms, as long as your platform meets the the above [requirements](#requirements).

Not? Check out the [Cygwin](#cygwin-wip) platform for example, maybe you can try to port those dependencies yourself.

Or, try to make a port without the missing dependencies (and you may need to rewrite some stuffs, this may not always be easy, for example the upstream Rust codebase is likely to grow). However, these are not covered by this project, just some friendly suggestions.

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
  * if `external` projects were added, deleted, or moved, update the list of excluded directories at the bottom of `CMakeLists.txt` accordingly
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
[zlib]: https://zlib.net/
[Clang]: https://clang.llvm.org/
[LLD]: https://lld.llvm.org/
[Rust]: https://www.rust-lang.org/
[Cargo]: https://doc.rust-lang.org/cargo/
[CMake]: https://cmake.org/
[Magisk]: https://github.com/topjohnwu/Magisk.git
[android-tools]: https://github.com/nmeum/android-tools
[Homebrew]: https://brew.sh/
[Libcxx]: https://libcxx.llvm.org/
[Libstdcxx]: https://gcc.gnu.org/onlinedocs/libstdc++/
[MSYS2]: https://www.msys2.org/
