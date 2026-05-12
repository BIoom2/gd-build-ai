#pragma once

#include <functional>

namespace bloom::render {

// Frame callback. Called once per Present after the DX9 device is ready and
// ImGui has been set up. Implement Bloom UI inside this callback.
using FrameCallback = std::function<void()>;

// Install the DX9 EndScene / Reset / Present hooks so we can render ImGui on
// top of the game. Spawns a watcher thread that waits for the d3d9 device to
// be created.
bool install_d3d9_hooks(FrameCallback on_frame);

void uninstall_d3d9_hooks();

} // namespace bloom::render
