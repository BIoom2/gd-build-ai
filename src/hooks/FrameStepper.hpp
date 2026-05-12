#pragma once

#include <atomic>

namespace bloom::hooks {

// Public state used by the PlayLayer::update hook.
extern std::atomic<int> g_step_request;

// Drive the frame stepper from the menu / keybinds. Each call schedules one or
// more physics steps to be processed on the next PlayLayer::update tick.
void request_step_forward(int count = 1);
void request_step_backward(int count = 1);
void request_hold_start();

// Should be called once per ImGui frame so that "hold start" + "hold interval"
// re-fires Step Forward continuously while the user holds the key.
void tick_auto_hold();

} // namespace bloom::hooks
