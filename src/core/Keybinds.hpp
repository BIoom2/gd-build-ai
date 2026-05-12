#pragma once

#include <cstdint>
#include <functional>

namespace bloom::core {

// Tracks key state on a per-frame poll basis and dispatches:
//   * single-key bindings configured via Config (show menu, step F/B, ...);
//   * the double-Alt menu toggle (press Alt twice within the configured
//     window in ms to toggle the menu visible state).
//
// poll() must be called once per ImGui frame. It internally calls GetAsyncKeyState
// for the codes that are currently registered, so there's no need for a global
// WH_KEYBOARD hook.
class Keybinds {
public:
    using Callback = std::function<void()>;

    void set_toggle_menu(Callback cb)      { m_toggle_menu = std::move(cb); }
    void set_step_forward(Callback cb)     { m_step_forward = std::move(cb); }
    void set_step_backward(Callback cb)    { m_step_backward = std::move(cb); }
    void set_hold_start(Callback cb)       { m_hold_start = std::move(cb); }
    void set_toggle_record(Callback cb)    { m_toggle_record = std::move(cb); }

    // Process key state. Called once per ImGui frame.
    void poll();

    // Quick test: was a virtual-key just pressed this frame (edge trigger)?
    bool just_pressed(int vk) const;

private:
    bool m_prev[256]   = {};
    bool m_now[256]    = {};

    // Double-Alt detection.
    std::int64_t m_last_alt_release_ms = 0;
    bool         m_alt_was_down = false;

    Callback m_toggle_menu;
    Callback m_step_forward;
    Callback m_step_backward;
    Callback m_hold_start;
    Callback m_toggle_record;
};

Keybinds& get_keybinds();

} // namespace bloom::core
