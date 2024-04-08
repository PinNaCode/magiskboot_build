cmake_minimum_required(VERSION 3.22)
project(magiskboot_build LANGUAGES C)

if (NOT WITHOUT_BUILD)
    enable_language(CXX)

    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_EXTENSIONS ON)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_EXTENSIONS ON)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("
        int main () {
            struct { int x; } __attribute__ ((packed)) foo = { 1337 };
            [] (int &a) { (void) a; } (foo.x);
        }
    " CXX_COMPILER_BIND_PACKED_FIELD_WORKS)

    if (NOT CXX_COMPILER_BIND_PACKED_FIELD_WORKS)
        message(FATAL_ERROR "Your C++ compiler is not capable of building magiskboot, please check FAQ in README for more details.")
    endif()

    if (WITH_LIBCXX)
        message(STATUS "Libc++ is enabled")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    else()
        message(STATUS "Libc++ is disabled")
    endif()
endif()

# Option to enable or disable patching vendor projects using patches directory.
# This helps to build vendor projects with or without any patching. Also if any
# files are changed in vendor projects those can be retained with this option.
option(PATCH_VENDORED_PROJECTS "Patch vendored projects using patches directory" ON)

option(WITHOUT_BUILD "Turn this on if you are creating source tarball only" OFF)

option(FULL_RUST_LTO "Keep LLVM bitcode in Rust libraries for LTO (requires LLD)" OFF)

# Option to enable or disable building static linked version of magiskboot.
# If enabled, the build result will be a standalone binary which you can run
# on any device with the same operating system and architecture.
# This requires installing depended static libraries on the host.
option(PREFER_STATIC_LINKING "Prefer static libraries when linking" OFF)

option(WITH_LIBCXX "Link libcxx instead of libstdc++" ON)
option(TRY_USE_LLD "Prefer LLD for linking when available" ON)

# Rust related options
set(RUSTFLAGS "" CACHE STRING "extra flags passed to rustc")
set(CARGO_FLAGS "" CACHE STRING "extra flags passed to cargo")
set(RUSTC_BOOTSTRAP "winsup,base,magiskboot_rs" CACHE STRING "")

# useful for cross-compiling
option(RUST_BUILD_STD "Build Rust standard library" OFF)

include(CMakeLists.magisk.txt)
include(CMakeLists.patch.txt)

if (NOT WITHOUT_BUILD)
    include(CheckLinkerFlag)
    if (TRY_USE_LLD)
        check_linker_flag(CXX -fuse-ld=lld FUSE_LD_EQ_LLD_WORKS)

        # prefer LLD when it is available
        if (FUSE_LD_EQ_LLD_WORKS)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
            set(CMAKE_REQUIRED_LINK_OPTIONS ${CMAKE_REQUIRED_LINK_OPTIONS} -fuse-ld=lld)
        endif()
    endif()

    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(MAGISKBOOT_TARGET magiskboot_debug)
        message(STATUS "Debug (unoptimized) build")
    else()
        set(MAGISKBOOT_TARGET magiskboot)
        message(STATUS "Release (optimized) build")
    endif()

    include(CMakeLists.compat.txt)

    find_package(PkgConfig REQUIRED)
    if (PREFER_STATIC_LINKING)
        message(STATUS "Static/standalone build")
        list(INSERT CMAKE_FIND_LIBRARY_PREFIXES 0 ${CMAKE_STATIC_LIBRARY_PREFIX})
        list(INSERT CMAKE_FIND_LIBRARY_SUFFIXES 0 ${CMAKE_STATIC_LIBRARY_SUFFIX})
        list(APPEND PKG_CONFIG_EXECUTABLE "--static")
        include(CheckLinkerFlag)
        check_linker_flag(CXX -static STATIC_LINK_WORKS)
        if (STATIC_LINK_WORKS)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
        endif()
        check_linker_flag(CXX -static-libgcc STATIC_LIBGCC_LINK_WORKS)
        if (STATIC_LIBGCC_LINK_WORKS)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
        endif()
        check_linker_flag(CXX -static-libstdc++ STATIC_LIBSTDCXX_LINK_WORKS)
        if (STATIC_LIBSTDCXX_LINK_WORKS)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
        endif()
    else()
        message(STATUS "Dynamic build")
    endif()

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    find_package(LibLZMA REQUIRED)
    pkg_check_modules(liblz4 REQUIRED IMPORTED_TARGET liblz4)
    find_package(BZip2 REQUIRED)
    find_package(ZLIB REQUIRED)

    add_compile_options(-Wall)

    include(CMakeLists.rust.txt)
    include(CMakeLists.magiskboot.txt)
    include(CMakeLists.stub.txt)
endif()

# CPack configuration for creating source tarballs which already include
# patched versions of the vendored dependencies.
set(CPACK_SOURCE_GENERATOR "TXZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "magiskboot_${MAGISK_VERSION}_${MAGISK_VER_CODE}-src")
set(CPACK_SOURCE_IGNORE_FILES "/patches/" "/build/" "/.git/" "/src/Magisk/app/"
    "/src/Magisk/buildSrc/" "/src/Magisk/docs/" "/src/Magisk/gradle/"
    "/src/Magisk/scripts/" "/src/Magisk/stub/" "/src/Magisk/tools/ndk-bins"
    "\\\\.so$" "\\\\.dll$" "\\\\.zip$" "\\\\.orig" "\\\\.rej" "\\\\.tar$"
    "\\\\.tar\\\\..*$" "\\\\.tgz$" "\\\\.data$" "aes_128_gcm.txt" "aes_256_gcm.txt")
include(CPack)
