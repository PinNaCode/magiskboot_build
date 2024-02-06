> [!NOTE]
>
> Android and Linux CI builds are not / no longer uploaded to Release to save space, please use `libmagiskboot.so` extracted from the official [Magisk][Magisk] APKs instead.

For debug builds, please download from [here](../../releases/last-debug-ci) instead.

#### For Windows releases

- `msvcrt`: usually works on most Windows versions without extra setup

- `ucrt`: available by default only since Windows 10+, sometimes requires installing or shipping with C runtime library manually to use on older version

#### For macOS users

OS release number is minimal required version to run the binary, binaries from the previous OS release(s) should work.

[Magisk]: https://github.com/topjohnwu/Magisk/releases
