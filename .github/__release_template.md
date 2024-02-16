> [!IMPORTANT]
>
> The official magiskboot only supports running on Android and Linux,
>
> If you find something that works on the officially supported platforms,
> but not on the extra platforms supported by magiskboot_build, then that
> is a magiskboot_build-specific bug.
>
> Please, do ***NOT*** report magiskboot_build bugs to the upstream Magisk!

> [!NOTE]
>
> Android and Linux CI builds are not / no longer uploaded to Release to save space, please use `libmagiskboot.so` extracted from the official [Magisk][Magisk] APKs instead.

For debug builds, please download from [here](../../releases/last-debug-ci) instead.

#### For Windows releases

- `msvcrt`: usually works on most Windows versions without extra setup

- `ucrt`: available by default only since Windows 10+, sometimes requires installing or shipping with C runtime library manually to use on older version

#### For macOS users

OS release number is minimal required version to run the binary, binaries from the previous OS release(s) should work.

#### About Web builds

- `noderawfs`: using NodeJS's host filesystem API (requires NodeJS to run, see the *WebAssembly*->*Emscripten* section in [README.md](README.md#emscripten) for more details), build with this tag is intended for running on desktop OS that is not yet supported by magiskboot_build

[Magisk]: https://github.com/topjohnwu/Magisk/releases
