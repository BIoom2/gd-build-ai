# Bloom

A TAS / macro bot for **Geometry Dash 2.1** (Windows x86), styled after the
classic TasBot menu layout.

> Bloom is a stand-alone DLL (no Geode required). Inject it into a running
> `GeometryDash.exe` with any DLL injector and the ImGui menu will appear on
> top of the game.

## Features

| Window              | What it does                                                                                            |
| ------------------- | ------------------------------------------------------------------------------------------------------- |
| **TASBOT**          | Recording / Playback radio, macro name field with **search dropdown**, Save / Load.                    |
| **SETTINGS**        | Version info, keybinds, Enable Bot / Lua Plugin, menu colour & size, global Search (filters all UI).   |
| **INTERNAL RECORDER** | Resolution / bitrate / codec / ffmpeg-style args (UI only — encoder pipe is a TODO).                 |
| **UTILITIES**       | Audio Speedhack, No-clip P1 / P2, Show Frame, **Practice Mode Fix**, **Practice Rotation Fix**, Native Speed, **Physics FPS**, **Display FPS**, **FPS Bypass enabled**. |
| **AUTO CLICKER**    | Auto Clicker P1 / P2, click / release IDs and intervals.                                               |
| **PICKUP**          | Pickup-macro toggles + click / release / player IDs.                                                   |
| **FRAME ADVANCE**   | Frame Advance, Backwards Stepping, Universal Frame Advance, Step F / B, Hold Start ms / Interval ms, Store N Backsteps. |

## Keybinds

| Key                | Action                                                                  |
| ------------------ | ----------------------------------------------------------------------- |
| **Alt + Alt** (double-tap inside 300 ms) | Toggle the menu visible / hidden (configurable).  |
| `RCTRL`            | Show menu (legacy TasBot binding).                                      |
| `SHIFT`            | View keybinds overlay.                                                  |
| `F1`               | Focus the global Search box.                                            |
| (configurable)     | Step Forward / Step Backward / Hold Start / Toggle Record.              |

All single-key bindings are remappable from `bloom_settings.json`.

## Macro format

Macros are stored as JSON in `bloom_macros/<name>.json` next to `Bloom.dll`:

```json
{
    "name": "deepweb",
    "version": "bloom-1",
    "fps": 60,
    "dual": false,
    "total_frames": 18432,
    "frames": [
        { "frame": 7,  "player": 1, "button": 1, "down": true,  "x": 0.0, "y": 105.0, "yv": 0.0, "rot": 0.0 },
        { "frame": 13, "player": 1, "button": 1, "down": false, "x": 0.0, "y": 105.0, "yv": 0.0, "rot": 0.0 }
    ]
}
```

* `version` is `"bloom-1"` for forwards-compatibility.
* `fps` is the physics FPS the macro was recorded at — Bloom uses this to
  compensate when replaying under a different `physics_fps` setting.
* Each frame carries a state snapshot used by **Practice Mode Fix** to realign
  the replay cursor when you load from a checkpoint.

## Building

CI builds on `windows-latest` with MSVC and Win32 (x86). Build locally:

```pwsh
cmake -S . -B build -A Win32 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

The artifact lands at `build/Release/Bloom.dll`.

### Dependencies (auto-fetched via CMake `FetchContent`)

* [Dear ImGui](https://github.com/ocornut/imgui) `v1.90.4` (DX9 + Win32 backends).
* [MinHook](https://github.com/TsudaKageyu/minhook) `v1.3.3` for function detours.
* [nlohmann/json](https://github.com/nlohmann/json) `v3.11.3` for macro / settings JSON.

## Project layout

```
src/
├── core/        # Bloom orchestrator, Config, Keybinds (incl. double-Alt)
├── render/      # DX9 + WndProc hooks, ImGui theme
├── menu/        # ImGui window definitions (TASBOT, SETTINGS, ...)
├── bot/         # Recording / playback state + JSON macro format
├── hooks/       # PlayLayer / PlayerObject / FrameStepper / FpsBypass / HookManager
├── gd/          # GD 2.1 addresses + minimal class accessors
├── util/        # Logger, time, AOB pattern scanner
└── main.cpp     # DllMain
```

## Addresses

`src/gd/Addresses.hpp` declares the GD 2.1 (build 2.111) `gd.dll` RVAs Bloom
hooks. If you target a different 2.1 sub-build and a hook fails to install,
override the offset with `bloom_settings.json` or supply an AOB pattern via
`util::scan_module` — no recompile required.

## Disclaimer

Bloom is intended for offline practice and TAS creation. Don't submit
bot-played runs to the GD leaderboards.
