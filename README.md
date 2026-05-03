# gd-build-ai

Geode mods for Geometry Dash 2.2 — currently hosting the **GD 2.1 Physics** mod.

## GD 2.1 Physics

A Geode mod that restores 2.1-era physics on Geometry Dash 2.2.0/2.2081 so legacy
2.1 levels (which depended on physics quirks RobTop fixed in 2.2) become playable
again. See [`about.md`](./about.md) for full details.

### Quick start

1. Install [Geode](https://geode-sdk.org/install/) for GD 2.2081 Windows.
2. Copy `devin.gd-21-physics.geode` into your Geode `mods` folder
   (`%LOCALAPPDATA%\GeometryDash\geode\mods\`).
3. Launch GD; toggle settings under "GD 2.1 Physics" in the in-game mod list.

### Build from source

```sh
# Linux cross-compile to Windows:
cmake -B build -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=/path/to/clang-cl-msvc.cmake \
    -DSPLAT_DIR=/path/to/splat \
    -DHOST_ARCH=x86_64 \
    -DCLANG_VER=19 \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build -j
# Output: build/devin.gd-21-physics.geode
```

### Credits

The 2.1 physics step-rate memory patch is adapted from
[Click Between Frames](https://github.com/theyareonit/Click-Between-Frames)
by theyareonit (MIT-licensed). See [`LICENSE`](./LICENSE).
