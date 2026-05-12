#include "hooks/FpsBypass.hpp"

#include <atomic>
#include <thread>

#include "core/Config.hpp"
#include "hooks/HookManager.hpp"
#include "util/Logger.hpp"

#include <windows.h>

namespace bloom::hooks {

namespace {

using Sleep_t = void (WINAPI*)(DWORD);
Sleep_t g_orig_Sleep = nullptr;

// We don't drop user-requested sleeps to zero (that would burn CPU 100% on the
// title screen). Instead we cap any sleep > 1 ms to (1000 / display_fps) ms
// whenever fps_bypass_enabled is on. This matches the behaviour TasBot 4.x
// uses for the "Display FPS" slider.
void WINAPI hk_Sleep(DWORD ms) {
    auto& cfg = core::get_config();
    if (cfg.fps_bypass_enabled && cfg.display_fps > 0 && ms > 0) {
        DWORD cap = static_cast<DWORD>(1000 / cfg.display_fps);
        if (cap == 0) cap = 1;
        if (ms > cap) ms = cap;
    }
    if (g_orig_Sleep) g_orig_Sleep(ms);
    else ::Sleep(ms);
}

} // namespace

void install_fps_bypass_hooks() {
    static bool installed = false;
    if (installed) return;
    installed = true;

    HMODULE k = GetModuleHandleA("kernel32.dll");
    if (!k) {
        BLOOM_LOG_ERROR("FpsBypass: kernel32 missing");
        return;
    }
    auto* sleep_addr = GetProcAddress(k, "Sleep");
    if (!sleep_addr) return;

    HookManager::instance().install(
        reinterpret_cast<void*>(sleep_addr),
        reinterpret_cast<void*>(&hk_Sleep),
        reinterpret_cast<void**>(&g_orig_Sleep),
        "kernel32.dll!Sleep");
}

} // namespace bloom::hooks
