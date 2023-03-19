cmake_minimum_required(VERSION 3.22)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
project(magiskboot_build)
enable_language(C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=lld -Wno-unused-command-line-argument")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld -Wno-unused-command-line-argument")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -fomit-frame-pointer -flto -fdata-sections -ffunction-sections -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -fomit-frame-pointer -flto -fdata-sections -ffunction-sections -Wl,--gc-sections")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -s -fomit-frame-pointer -flto -fdata-sections -ffunction-sections -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -s -fomit-frame-pointer -flto -fdata-sections -ffunction-sections -Wl,--gc-sections")
if(APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DARWIN_C_SOURCE -D__DARWIN_C_LEVEL=__DARWIN_C_FULL")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DARWIN_C_SOURCE -D__DARWIN_C_LEVEL=__DARWIN_C_FULL")
endif()

# Option to enable or disable patching vendor projects using patches directory.
# This helps to build vendor projects with or without any patching. Also if any
# files are changed in vendor projects those can be retained with this option.
option(PATCH_VENDOR_PROJECTS "Patch vendor projects using patches directory" ON)

add_compile_definitions(_FORTIFY_SOURCE=2)

include(CMakeLists.android.txt)
include(CMakeLists.magisk.txt)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(LibLZMA REQUIRED)
pkg_check_modules(liblz4 REQUIRED IMPORTED_TARGET liblz4)
find_package(BZip2 REQUIRED)
find_package(LIBFDT REQUIRED)
pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)
if (NOT APPLE)
    pkg_check_modules(libbsd REQUIRED IMPORTED_TARGET libbsd)
endif()

add_compile_options(-Wall)

include(CMakeLists.rust.txt)
include(CMakeLists.magiskboot.txt)
include(CMakeLists.lib.txt)
include(CMakeLists.patch.txt)

# CPack configuration for creating source tarballs which already include
# patched versions of the vendored dependencies.
set(CPACK_SOURCE_GENERATOR "TXZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "magiskboot_${MAGISK_VERSION}_${MAGISK_VER_CODE}-src")
set(CPACK_SOURCE_IGNORE_FILES "/patches/" "/build/" "/.git/"
    "/vendor/Magisk/app/" "/vendor/Magisk/buildSrc/" "/vendor/Magisk/docs/"
    "/vendor/Magisk/gradle/" "/vendor/Magisk/scripts/" "/vendor/Magisk/stub/"
    "/vendor/Magisk/tools/" "/vendor/Magisk/native/src/external/busybox/"
    "/vendor/Magisk/native/src/external/bzip2/" "/vendor/Magisk/native/src/external/cxx-rs/"
    "/vendor/Magisk/native/src/external/dtc/" "/vendor/Magisk/native/src/external/libcxx/"
    "/vendor/Magisk/native/src/external/lsplt/" "/vendor/Magisk/native/src/external/lz4/"
    "/vendor/Magisk/native/src/external/nanopb/" "/vendor/Magisk/native/src/external/parallel-hashmap/"
    "/vendor/Magisk/native/src/external/pcre/" "/vendor/Magisk/native/src/external/selinux/"
    "/vendor/Magisk/native/src/external/systemproperties/" "/vendor/Magisk/native/src/external/xz/"
    "/vendor/Magisk/native/src/external/xz-embedded/" "/vendor/Magisk/native/src/external/xz_config/"
    "/vendor/Magisk/native/src/external/zlib/" "/vendor/Magisk/native/src/resetprop/"
    "/vendor/Magisk/native/src/su/" "/vendor/Magisk/native/src/zygisk/"
    "\\\\.so$" "\\\\.zip$" "\\\\.orig" "\\\\.rej"
    "\\\\.tar$" "\\\\.tar\\\\..*$" "\\\\.tgz$"
    "\\\\.data$" "aes_128_gcm.txt" "aes_256_gcm.txt")
include(CPack)
