## magiskboot_build

![Magisk Version Badge](https://img.shields.io/badge/Magisk%20version-v26.4%20Canary%20(26404)-green?logo=magisk)
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

please make sure you have installed the above softwares before building

here are examples for some supported popular operating systems/distributions:

<details><summary>Android</summary>

#### Android

Please check the instructions for [Cross compiling](#cross-compiling) first.

Download [ONDK][ONDK], set environment variable `ANDROID_NDK_HOME` to the extracted directory, and use [vcpkg][vcpkg] to install the dependencies.

Use `/path/to/your/ondk/build/cmake/android.toolchain.cmake` as the toolchain file for vcpkg.

Then, set the CMake variable `ANDROID_PLATFORM` to the desired API level, and `ANDROID_ABI` to the desired archiecture,

for more details about these variables, please refer to the [NDK documentation](https://developer.android.com/ndk/guides/cmake#variables).

Make sure to enable source build for the Rust standard library (STD), then activate the Rust toolchain by append it to your current `PATH`:

````shell
export PATH=/path/to/your/ondk/toolchains/rust/bin:$PATH
````

#### Termux

> **Note**
>
> Termux build is not actively tested by CI, but it might work without problem.
>
> If you find the build is broken, please file a [bug report](../../issues).

````shell
apt update
apt upgrade  # upgrade all existing packages (optional)
apt install build-essential liblzma liblz4 libbz2 zlib pkg-config \
            clang lld rust cmake ninja
````

When configuring, pass `-DCMAKE_INSTALL_PREFIX=$PREFIX` to CMake.

</details>

<details><summary>Linux</summary>

#### Ubuntu 22.04 (jammy)

Make sure to read [this part](#help-my-build-has-failed) before you start.

````shell
sudo apt update
sudo apt upgrade  # upgrade all existing packages (optional)

# replace clang-15, libc++-15-dev, libc++abi-15-dev with
# appropriate version according to your Ubuntu release
# do the same for lld if you want to use it
sudo apt install build-essential lzma-dev liblzma-dev liblz4-dev libbz2-dev \
                 zlib1g-dev pkg-config clang-15 libc++-15-dev libc++abi-15-dev cmake \
                 ninja-build rustc cargo  # optional: lld-15
````

When configuring, set `CC` and `CXX` with correct values, for example: `clang-15` and `clang++-15`.

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

<details><summary>Windows</summary>

#### Windows (MinGW)

> **Note**
>
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

If you are cross-compiling and using vcpkg to manage the dependencies, please make sure CMake variable `MINGW` is set to `TRUE` during configuring.

#### Cygwin (test)

> **Note**
>
> Cygwin build is not actively tested by CI, but it might work without problem.
>
> If you find the build is broken, please file a [bug report](../../issues).

An experimental build is available [here](../../releases/cygwin-test).

To build for Cygwin, you need to compile a Rust toolchain from source, for more info: [Cygwin Rust porting](https://gist.github.com/ookiineko/057eb3a91825313caeaf6d793a33b0b2)

You will also need to compile the LLVM & Clang from source with Cygwin patches, you can pick them from: [Ookiineko's unofficial cygpkgs](https://github.com/orgs/ookiineko-cygpkg/repositories)

Currently Cygwin Rust has no host tools support, so cross compiling is needed, make sure to read the instructions for [Cross compiling](#cross-compiling).

The cross compiler is available on Fedora Linux, provided by the [Fedora Cygwin][fedora-cygwin].

Install these packages: `cygwin`, `cygport` and `cygwin-{filesystem,binutils,gcc,pkg-config}`.

Build and install the [dependencies](#requirements) from source to sysroot using cygport, or download and extract the prebuilt package from a Cygwin mirror. Another way is to extract those files from an actual Cygwin installation.

When configuring, use `cygwin64-cmake` instead of `cmake`, but don't use it for `cmake --build` and other CMake commands.

To use the patched Clang with Fedora Cygwin, set both `CMAKE_C_COMPILER_TARGET` and `CMAKE_CXX_COMPILER_TARGET` to `x86_64-unknown-windows-cygnus` and `CMAKE_SYSROOT` to `/usr/x86_64-pc-cygwin/sys-root` when configuring.

Another issue is `cygwin64-cmake` overrides C and C++ compilers to GCC which is not supported by Magisk, and somehow ignoring the `CC` and `CXX` variables set by us. To workaround this, set `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` manually with the path to the previous Cygwin cross Clang and Clang++.

Cygwin's Libc++ is buggy, if you can't pass linking with Libc++, see [this part](#help-my-build-has-failed) to apply patches for building without Libc++. (You will also need to apply patches to compile with old Rust toolchain.)

Finally, you should be able to pass the build and get it working now :D

Feel free to ask questions about Cygwin support in [Issues](../../issues).

</details>

<details><summary>WebAssembly</summary>

#### Emscripten

> **Note**
>
> Currently this port only support running with NodeJS.
>
> A web frontend wrapper must be written to handle the argument passing and file system management in browsers.
>
> This should be implemented soon in the future, but unfortunately I am not a web developer, any help on this will be welcome QwQ

Please read the [Cross compiling](#cross-compiling) instructions first.

Install the [Emscripten][Emscripten] SDK and a Nightly Rust toolchain using [rustup][rustup].

Use [vcpkg][vcpkg] to install the [depended libraries](#requirements), the triplet is called `wasm32-emscripten`.

When configuring, use `emcmake cmake` instead of `cmake` (but don't use it for `cmake --build` and other CMake commands) , and use `/path/to/your/emsdk/emscripten/cmake/Modules/Platform/Emscripten.cmake` as the toolchain file for vcpkg.

Build Rust standard library from source to make sure things works as expected. (FIXME: why is this needed?)

For NodeJS, make sure to set `CMAKE_EXE_LINKER_FLAGS` to `-sNODERAWFS` to allow using the host filesystem.

finally, you can run the result with [NodeJS][NodeJS] using: `node magiskboot.js`

</details>

### Download

For prebuilt binaries, go check [GitHub Releases](../../releases/latest) for selected CI builds.

### Build & Install

First get and extract the latest source tarball (called `magiskboot_<COMMIT_ID>_<VERCODE>-src.tar.xz`) from [Github Releases](../../releases/latest).

Or clone this repository using Git with this option: `--recurse-submodules`. (If you already have a cloned repository without using that option, run: `git submodule update --init --recursive` instead)

````shell
CC=clang CXX=clang++ cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release  # configure
cmake --build build -j $(nproc)  # build
./build/magiskboot  # running
# install to system (may need sudo, to specify different install dir, set the `DESTDIR' environment variable)
cmake --install build
````

If you prefer a statically linked binary (optional), pass `-DPREFER_STATIC_LINKING=ON` to CMake while configuring, make sure your distribution provided you the static version of the [depended libraries](#requirements), otherwise you'll run into configure errors and you have to change your distribution or try to compile the static version of those libraries yourself and install them to your system for the static build.

#### Rust

You can specify extra arguments passed to Cargo by setting CMake variable `CARGO_FLAGS` while configuring, you can also provide an initial value of environment `RUSTFLAGS` by setting the corresponding CMake variable.

#### Cross compiling

First get a cross-compiler with Libc++ and Clang by either installing from source or downloading it from somewhere (usually your distribution's package manager).

Install Rust using [rustup][rustup] and add your cross target like this: `rustup target add aarch64-unknown-linux-gnu` (replace `aarch64-unknown-linux-gnu` with the Rust target of your target platform).

If the cross Rust target can't be installed using [rustup][rustup], you can still compile if you build the Rust standard library (STD) from source, to do this, pass `-DRUST_BUILD_STD=ON` during configuration (Note this will require Nightly Rust and the STD source code to be installed on your system).

To cross-compile, you may need a [CMake toolchain file][cmake-toolchains] describing your target specs, the location of the cross compiler toolchain and strategy for CMake to seek for the depended libraries.

You can install the depended libraries for your cross target by using [vcpkg][vcpkg], for example:

````shell
vcpkg install --host-triplet=arm64-linux bzip2 lz4 zlib liblzma
````

`arm64-linux` is the triplet of your cross target, pass this value to CMake using the `VCPKG_TARGET_TRIPLET` variable during configuration.

Set variable `RUSTC_TARGET` to the Rust target you wanted to cross compile for, e.g. `aarch64-unknown-linux-gnu`.

To use your toolchain file with `vcpkg`, first pass `-DCMAKE_TOOLCHAIN_FILE=/path/to/your/vcpkg/scripts/buildsystems/vcpkg.cmake` to CMake, and set the variable `VCPKG_CHAINLOAD_TOOLCHAIN_FILE` to the path of your actual toolchain file.

#### LTO

If you want to perform LTO at the final link time, pass `-DUSE_LTO_LINKER_PLUGIN=ON` to CMake during configuring.

And you will need to install LLD.

Note: you may need to make sure your LLVM and LLD are sharing the same LLVM version with Rust.

### Testing

> [!NOTE]
> Features not tested by CI doesn't necessarily mean it can't work.

A very basic shell script is provided under the [`scripts`](scripts/magiskboot_test.sh) directory, currently the following functions are automatically tested with Github CI:

- [x] unpack* (currently not all header versions and variants are tested)
- [x] repack* (same as above)
- [ ] verify
- [ ] sign
- [ ] extract
- [x] hexpatch (TODO: make this test pass on Emscripten)

* cpio* (currently not widely tested on all common ramdisks)
  - [ ] cpio exists
  - [x] cpio ls
  - [ ] cpio rm
  - [ ] cpio mkdir
  - [ ] cpio ln
  - [ ] cpio mv
  - [x] cpio add
  - [x] cpio extract
  - [ ] cpio test
  - [ ] cpio patch
  - [ ] cpio backup
  - [ ] cpio restore

* dtb
  - [ ] dtb print
  - [x] dtb test
  - [ ] dtb patch

- [ ] split
- [x] sha1
- [ ] cleanup
- [x] compress
- [x] decompress

### Generating source tarball

````shell
cmake -G Ninja -B build -DNO_TARGETS_OR_DEPS=ON  # configure
cmake --build build -t package_source  # make a source package
````

Make sure you passed `-DNO_TARGETS_OR_DEPS=ON` to CMake while configuring, which will allow creating source tarball without installing the [build dependencies](#requirements).

you should be able to find your source package under the `build` folder

### FAQ

#### Something isn't working

Please use debug builds and paste your information (like crash or error logs) in a new [Issue](../../issues).

#### Help, my build has failed

A recent version of Rust is needed since the upstream is actually using a custom nightly Rust compiler,

if your distribution's Rust compiler is too old (e.g. Debian/Ubuntu based), manually applying some extra patches is probably needed,

you can try to pick some known ones from [here](patches-contrib), or report it by filing an [Issue](../../issues) if that doesn't help.

Or you can try installing Rust via [rustup][rustup] instead.

If you need to build with [Libstdc++][Libstdcxx] instead of [Libc++][Libcxx] (not recommended), pass `-DWITHOUT_LIBCXX=ON` to CMake during configuring, also apply [this patch](patches-contrib/Magisk/0029-workaround-support-building-with-libstdcxx.patch).

#### Is this thing using the latest Magisk source?

Check the version status badges at [the top of README](#magiskboot_build).

This project is very similiar to [android-tools][android-tools] which just maintains a set of patches on top of a specific upstream Magisk commit and require manual adaption for compiling with newer source.

Although I may update the version once in a while, [Pull requests](../../pulls) are welcome.

#### Can you add ... platform support?

This project aims to be portable, it should be possible to port it to new platforms with some efforts, as long as your platform meets the the above [requirements](#requirements).

Feel free to add an [Issue](../../issues) about support on your new platform.

### Development

#### Clean builds

To quickly discard the current `build` directory and dirty `vendor/` submodule changes, please run `make clean`.

#### Tweaking patches

To temporarily disable `vendor/` projects patching, re-configure with `-DPATCH_VENDOR_PROJECTS=OFF` (useful if you are patching them manually).
To enable it again, use `-DPATCH_VENDOR_PROJECTS=ON` (Note this will clean up changes in `vendor/` modules and re-apply all the patches).

#### Rust stuffs

If you modify something in the Rust part, you will have to perform `rm build/libmagiskboot-rs.a` manually before rebuilding. (TODO: let CMake detect source changes)

#### Debug builds

Pass `-DCMAKE_BUILD_TYPE=Debug` to CMake instead of `Release` during configuring to make some error log more verbose.

The result `magiskboot` executable will have a `_debug` suffix.

#### Updating sources

For `vendor/` submodules with their name starting with `android_`, most patches are imported from [android-tools][android-tools], and don't always require updating.

When syncing upstream `vendor/{android_libbase,Magisk}` changes, here is a few things to check:
  * `build.py` changes
  * `{,{base,boot}/}Android.mk` updates
  * if `external` projects were added, deleted, or moved, update the list of excluded directories at the bottom of `CMakeLists.txt` accordingly
  * switch to `winsup` stub for MinGW for if any new file or spot reference to any of the following standard libc identifiers:
    * struct DIR
    * struct dirent
    * mkdir
    * {open,read,close,rewind,seek,tell}dir
    * creat
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
[rustup]: https://rustup.rs/
[Homebrew]: https://brew.sh/
[Libcxx]: https://libcxx.llvm.org/
[Libstdcxx]: https://gcc.gnu.org/onlinedocs/libstdc++/
[MSYS2]: https://www.msys2.org/
[fedora-cygwin]: https://copr.fedorainfracloud.org/coprs/yselkowitz/cygwin/
[cmake-toolchains]: https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html
[vcpkg]: https://vcpkg.io/
[Emscripten]: https://emscripten.org/
[NodeJS]: https://nodejs.org/
[ONDK]: https://github.com/topjohnwu/ondk
