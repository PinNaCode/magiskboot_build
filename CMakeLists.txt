cmake_minimum_required(VERSION 3.22)
project(magiskboot_build LANGUAGES C)

if (NOT NO_TARGETS_OR_DEPS)
    enable_language(CXX)

    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_EXTENSIONS ON)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_EXTENSIONS ON)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "Clang is required to build magiskboot, please set CXX or CMAKE_CXX_COMPILER to clang++ and reconfigure.")
    endif()

    if (WITHOUT_LIBCXX)
        message(STATUS "Libc++ is disabled")
    else()
        message(STATUS "Libc++ is enabled")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    endif()

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fomit-frame-pointer")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer")
    set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -fomit-frame-pointer")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -fomit-frame-pointer")
endif()

# Option to enable or disable patching vendor projects using patches directory.
# This helps to build vendor projects with or without any patching. Also if any
# files are changed in vendor projects those can be retained with this option.
option(PATCH_VENDORED_PROJECTS "Patch vendored projects using patches directory" ON)

option(NO_TARGETS_OR_DEPS "Turn this on if you are creating source tarball only" OFF)

option(USE_LTO_LINKER_PLUGIN "Keep LLVM bitcode in objects for LTO (requires LLD)" OFF)

# Option to enable or disable building static linked version of magiskboot.
# If enabled, the build result will be a standalone binary which you can run
# on any device with the same operating system and architecture.
# This requires installing depended static libraries on the host.
option(PREFER_STATIC_LINKING "Prefer static libraries when linking" OFF)

option(WITHOUT_LIBCXX "Don't link libcxx instead of libstdc++" OFF)

# Rust related options
set(RUSTFLAGS "" CACHE STRING "extra flags passed to rustc")
set(CARGO_FLAGS "" CACHE STRING "extra flags passed to cargo")
set(RUSTC_BOOTSTRAP "winsup,base,magiskboot" CACHE STRING "HACK: enable nightly features on stable channel for crates")

# useful for cross-compiling
option(RUST_BUILD_STD "Build Rust standard library" OFF)
set(RUSTC_TARGET "" CACHE STRING "Rust target")

include(CMakeLists.magisk.txt)

if (NOT NO_TARGETS_OR_DEPS)
    include(CheckLinkerFlag)
    check_linker_flag(CXX -fuse-ld=lld FUSE_LD_EQ_LLD_WORKS)

    # prefer LLD when it is available
    if (FUSE_LD_EQ_LLD_WORKS)
        set(CMAKE_REQUIRED_LINK_OPTIONS ${CMAKE_REQUIRED_LINK_OPTIONS} -fuse-ld=lld)

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=lld -Wno-unused-command-line-argument")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld -Wno-unused-command-line-argument")
    endif()

    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-flto" LTO_WORKS)

    # Enable LTO by default for Release builds
    if (LTO_WORKS AND (USE_LTO_LINKER_PLUGIN OR NOT CMAKE_BUILD_TYPE MATCHES "Debug"))
        message(STATUS "LTO is enabled")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
        set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -flto")
        set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -flto")
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -flto")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto")
    else()
        message(STATUS "LTO is disabled")
    endif()

    if (USE_LTO_LINKER_PLUGIN)
        if (NOT LTO_WORKS)
            message(FATAL_ERROR "LTO doesn't work with the current compiler setup")
        endif()

        # note Xcode's ld is capable of dealing with LLVM bitcode
        # the only issue is AppleClang doesn't support -fuse-ld=lld flag
        # so we bypass the check for it here
        if (NOT APPLE AND NOT FUSE_LD_EQ_LLD_WORKS)
            message(FATAL_ERROR "Unable to compile test program with -fuse-ld=lld")
        endif()
    endif()

    include(CheckLinkerFlag)
    check_linker_flag(CXX -Wl,--gc-sections GC_SECTIONS_WORKS)
    if (GC_SECTIONS_WORKS)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fdata-sections -ffunction-sections")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fdata-sections -ffunction-sections")
        set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -fdata-sections -ffunction-sections")
        set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -fdata-sections -ffunction-sections")
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -fdata-sections -ffunction-sections")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fdata-sections -ffunction-sections")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
        set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} -Wl,--gc-sections")
        set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -Wl,--gc-sections")
    endif()

    check_linker_flag(CXX -s STRIP_WORKS)
    if (STRIP_WORKS)
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
        set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} -s")
        set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -s")
    endif()

    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(MAGISKBOOT_TARGET magiskboot_debug)
    else()
        set(MAGISKBOOT_TARGET magiskboot)
    endif()

    include(CMakeLists.compat.txt)

    if (PREFER_STATIC_LINKING)
        message(STATUS "Static/standalone build")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        include(CheckLinkerFlag)
        check_linker_flag(CXX -static STATIC_LINK_WORKS)
        if (STATIC_LINK_WORKS)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
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
    find_package(PkgConfig REQUIRED)
    find_package(LibLZMA REQUIRED)
    pkg_check_modules(liblz4 REQUIRED IMPORTED_TARGET liblz4)
    find_package(BZip2 REQUIRED)
    find_package(ZLIB REQUIRED)

    add_compile_options(-Wall)

    include(CMakeLists.rust.txt)
    include(CMakeLists.magiskboot.txt)
    include(CMakeLists.lib.txt)
    include(CMakeLists.stub.txt)
endif()

include(CMakeLists.patch.txt)

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
