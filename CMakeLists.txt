cmake_minimum_required(VERSION 3.22)
project(magiskboot_build)
enable_language(C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT NO_TARGETS_OR_DEPS)
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
message(FATAL_ERROR "Clang is required to build magiskboot, please set CXX or CMAKE_CXX_COMPILER to clang++ and reconfigure.")
endif()

if (NOT WITHOUT_LIBCXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
endif()
endif()
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -fomit-frame-pointer")

# Option to enable or disable patching vendor projects using patches directory.
# This helps to build vendor projects with or without any patching. Also if any
# files are changed in vendor projects those can be retained with this option.
option(PATCH_VENDOR_PROJECTS "Patch vendor projects using patches directory" ON)

option(NO_TARGETS_OR_DEPS "Turn this on if you are creating source tarball only" OFF)

option(USE_LTO_LINKER_PLUGIN "Keep LLVM bitcode in objects for LTO (requires LLD)" OFF)

# Option to enable or disable building static linked version of magiskboot.
# If enabled, the build result will be a standalone binary which you can run
# on any device with the same operating system and architecture.
# This requires installing depended static libraries on the host.
option(PREFER_STATIC_LINKING "Prefer static libraries when linking" OFF)

option(WITHOUT_LIBCXX "Don't link libcxx instead of libstdc++" OFF)

# XXX: can be detected if dump target JSON (rustc)
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND NOT CYGWIN AND NOT APPLE)
set(USE_LTO_LINKER_PLUGIN ON)
endif()

if (NOT NO_TARGETS_OR_DEPS)
if (USE_LTO_LINKER_PLUGIN)
find_program(LLD_EXECUTABLE lld REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=lld -Wno-unused-command-line-argument")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld -Wno-unused-command-line-argument")
set(RUSTFLAGS -Clinker-plugin-lto)
endif() # USE_LTO_LINKER_PLUGIN
endif() # NO_TARGETS_OR_DEPS

add_compile_options(-U_FORTIFY_SOURCE)

if (NOT NO_TARGETS_OR_DEPS)
include(CMakeLists.compat.txt)
include(CMakeLists.android.txt)
endif()
include(CMakeLists.magisk.txt)

if (PREFER_STATIC_LINKING)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
if (NOT APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libstdc++")
endif()
endif()

if (NOT NO_TARGETS_OR_DEPS)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(LibLZMA REQUIRED)
pkg_check_modules(liblz4 REQUIRED IMPORTED_TARGET liblz4)
find_package(BZip2 REQUIRED)
pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)
endif() # NO_TARGETS_OR_DEPS

add_compile_options(-Wall)

if (NOT NO_TARGETS_OR_DEPS)
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
set(CPACK_SOURCE_IGNORE_FILES "/patches/" "/build/" "/.git/"
    "/vendor/Magisk/app/" "/vendor/Magisk/buildSrc/" "/vendor/Magisk/docs/"
    "/vendor/Magisk/gradle/" "/vendor/Magisk/scripts/" "/vendor/Magisk/stub/"
    "vendor/Magisk/tools/ndk-bins" "vendor/Magisk/tools/termux-elf-cleaner"
    "/vendor/Magisk/native/src/external/busybox/" "/vendor/Magisk/native/src/external/bzip2/"
    "/vendor/Magisk/native/src/external/libcxx/" "/vendor/Magisk/native/src/external/lsplt/"
    "/vendor/Magisk/native/src/external/lz4/" "/vendor/Magisk/native/src/external/pcre/"
    "/vendor/Magisk/native/src/external/selinux/" "/vendor/Magisk/native/src/external/systemproperties/"
    "/vendor/Magisk/native/src/external/xz/" "/vendor/Magisk/native/src/external/xz-embedded/"
    "/vendor/Magisk/native/src/external/xz_config/" "/vendor/Magisk/native/src/external/zlib/"
    "/vendor/Magisk/native/src/resetprop/" "/vendor/Magisk/native/src/su/"
    "/vendor/Magisk/native/src/zygisk/" "\\\\.so$" "\\\\.zip$" "\\\\.orig" "\\\\.rej"
    "\\\\.tar$" "\\\\.tar\\\\..*$" "\\\\.tgz$" "\\\\.data$" "aes_128_gcm.txt" "aes_256_gcm.txt")
include(CPack)
