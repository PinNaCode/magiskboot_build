> [!NOTE]
>
> Android and Linux CI builds are not uploaded to Release to save space, please download them from [Magisk][Magisk] instead.
>
> macOS 14 CI builds is not yet added at the moment (#16), binaries from the previous OS releases might work though.

#### Build types

- `debug`: for getting logs only, don't use unless you know what are you doing

- `release`: if you are a normal user, download package with this tag

#### Link types

- `standalone`: single binary unlike `aio`, works without extra dependency

#### For Windows releases

- `msvcrt`: usually works on most Windows versions without extra setup

- `ucrt`: available by default only since Windows 10+, sometimes requires installing or shipping with C runtime library manually to use on older version

[Magisk]: https://github.com/topjohnwu/Magisk/releases
