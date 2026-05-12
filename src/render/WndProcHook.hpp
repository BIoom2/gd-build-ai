#pragma once

#include <windows.h>

namespace bloom::render {

// Installs a WndProc subclass so ImGui receives input. Idempotent.
bool install_wndproc_hook(HWND hwnd);

void uninstall_wndproc_hook();

// Toggle whether input should be forwarded to the game. When the menu is open
// and ImGui wants input, we set this to false so clicks/keys don't leak
// through to gameplay.
void set_block_game_input(bool block);

} // namespace bloom::render
