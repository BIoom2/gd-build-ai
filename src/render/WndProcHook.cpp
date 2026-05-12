#include "render/WndProcHook.hpp"

#include <atomic>

#include <imgui.h>
#include <imgui_impl_win32.h>

// Forward declaration of ImGui's Win32 message handler at global scope
// (the upstream header doesn't expose this prototype).
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace bloom::render {

namespace {

WNDPROC g_original_wndproc = nullptr;
HWND    g_hwnd             = nullptr;
std::atomic<bool> g_block_input{false};

LRESULT CALLBACK bloom_wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp);
        auto& io = ImGui::GetIO();
        if (g_block_input.load(std::memory_order_acquire)) {
            // Swallow mouse / keyboard / char messages so the game doesn't act
            // on them while the menu is interactive.
            switch (msg) {
                case WM_LBUTTONDOWN: case WM_LBUTTONUP:
                case WM_RBUTTONDOWN: case WM_RBUTTONUP:
                case WM_MBUTTONDOWN: case WM_MBUTTONUP:
                case WM_MOUSEWHEEL:  case WM_MOUSEMOVE:
                    if (io.WantCaptureMouse) return 0;
                    break;
                case WM_KEYDOWN: case WM_KEYUP:
                case WM_SYSKEYDOWN: case WM_SYSKEYUP:
                case WM_CHAR:
                    if (io.WantCaptureKeyboard) return 0;
                    break;
            }
        }
    }
    return CallWindowProcW(g_original_wndproc, hwnd, msg, wp, lp);
}

} // namespace

bool install_wndproc_hook(HWND hwnd) {
    if (g_original_wndproc || !hwnd) return false;
    g_hwnd = hwnd;
    g_original_wndproc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&bloom_wndproc)));
    return g_original_wndproc != nullptr;
}

void uninstall_wndproc_hook() {
    if (!g_original_wndproc || !g_hwnd) return;
    SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_original_wndproc));
    g_original_wndproc = nullptr;
    g_hwnd = nullptr;
}

void set_block_game_input(bool block) {
    g_block_input.store(block, std::memory_order_release);
}

} // namespace bloom::render
