#pragma once

namespace bloom::hooks {

// Hooks Sleep / SwapBuffers to drive the display FPS bypass and configures the
// physics FPS via the PlayLayer::update detour (which intercepts dt). Call
// once during init; toggling is purely config-driven.
void install_fps_bypass_hooks();

} // namespace bloom::hooks
