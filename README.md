## magiskboot_build

a simple CMake-based build system for magiskboot

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
3. [LLD][LLD]
4. [Rust][Rust]
5. [Cargo][Cargo]
6. [cxxbridge][cxx-rs]
7. [CMake][CMake]

please ensure you have installed the above softwares before building

### Build & Install

````shell
cmake -B build -DCMAKE_BUILD_TYPE=Release  # configure
cmake --build build -j $(nproc)  # build
./build/magiskboot  # running
# install to system (probably needs super-user privileges,
# or to a directory specified by the `DESTDIR' environment variable)
sudo cmake --install install
````

### Generating source tarball

````shell
cmake -B build  # configure
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
[cxx-rs]: https://github.com/dtolnay/cxx.git
[CMake]: https://cmake.org/
[Magisk]: https://github.com/topjohnwu/Magisk.git
[android-tools]: https://github.com/nmeum/android-tools
