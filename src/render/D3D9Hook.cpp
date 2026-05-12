#include "render/D3D9Hook.hpp"

#include <atomic>
#include <thread>

#include <d3d9.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include "hooks/HookManager.hpp"
#include "render/ImGuiTheme.hpp"
#include "render/WndProcHook.hpp"
#include "util/Logger.hpp"

namespace bloom::render {

namespace {

using EndScene_t   = HRESULT (WINAPI*)(IDirect3DDevice9*);
using Reset_t      = HRESULT (WINAPI*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

EndScene_t g_orig_EndScene = nullptr;
Reset_t    g_orig_Reset    = nullptr;

std::atomic<bool> g_imgui_initialized{false};
HWND              g_hwnd = nullptr;
FrameCallback     g_frame_callback;

void init_imgui(IDirect3DDevice9* device) {
    if (g_imgui_initialized.load(std::memory_order_acquire)) return;

    D3DDEVICE_CREATION_PARAMETERS params{};
    if (FAILED(device->GetCreationParameters(&params))) return;
    g_hwnd = params.hFocusWindow;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplWin32_Init(g_hwnd);
    ImGui_ImplDX9_Init(device);

    apply_bloom_theme();
    install_wndproc_hook(g_hwnd);

    g_imgui_initialized.store(true, std::memory_order_release);
    BLOOM_LOG_INFO("ImGui initialized (hwnd=%p)", g_hwnd);
}

HRESULT WINAPI hk_EndScene(IDirect3DDevice9* device) {
    init_imgui(device);

    if (g_imgui_initialized.load(std::memory_order_acquire)) {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (g_frame_callback) g_frame_callback();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    return g_orig_EndScene(device);
}

HRESULT WINAPI hk_Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) {
    if (g_imgui_initialized.load(std::memory_order_acquire)) {
        ImGui_ImplDX9_InvalidateDeviceObjects();
    }
    HRESULT hr = g_orig_Reset(device, params);
    if (SUCCEEDED(hr) && g_imgui_initialized.load(std::memory_order_acquire)) {
        ImGui_ImplDX9_CreateDeviceObjects();
    }
    return hr;
}

// Build a transient device just so we can read its vtable, then release it.
// This is the standard hooking technique for d3d9 - much more robust than
// trying to find the vtable by AOB.
void* find_endscene_address() {
    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return nullptr;

    D3DPRESENT_PARAMETERS pp{};
    pp.Windowed = TRUE;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.hDeviceWindow = GetDesktopWindow();

    IDirect3DDevice9* device = nullptr;
    HRESULT hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                                   pp.hDeviceWindow,
                                   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                   &pp, &device);
    if (FAILED(hr) || !device) {
        d3d->Release();
        return nullptr;
    }

    void** vtable = *reinterpret_cast<void***>(device);
    void* endscene = vtable[42]; // IDirect3DDevice9::EndScene
    device->Release();
    d3d->Release();
    return endscene;
}

void* find_reset_address() {
    // Lazily resolved via the same temp-device trick.
    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return nullptr;

    D3DPRESENT_PARAMETERS pp{};
    pp.Windowed = TRUE;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.hDeviceWindow = GetDesktopWindow();

    IDirect3DDevice9* device = nullptr;
    HRESULT hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                                   pp.hDeviceWindow,
                                   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                   &pp, &device);
    if (FAILED(hr) || !device) {
        d3d->Release();
        return nullptr;
    }
    void** vtable = *reinterpret_cast<void***>(device);
    void* reset = vtable[16];
    device->Release();
    d3d->Release();
    return reset;
}

} // namespace

bool install_d3d9_hooks(FrameCallback on_frame) {
    g_frame_callback = std::move(on_frame);

    void* endscene = find_endscene_address();
    void* reset    = find_reset_address();
    if (!endscene || !reset) {
        BLOOM_LOG_ERROR("Could not locate d3d9 EndScene/Reset");
        return false;
    }

    auto& hm = hooks::HookManager::instance();
    bool ok = true;
    ok &= hm.install(endscene, reinterpret_cast<void*>(&hk_EndScene),
                     reinterpret_cast<void**>(&g_orig_EndScene), "IDirect3DDevice9::EndScene");
    ok &= hm.install(reset, reinterpret_cast<void*>(&hk_Reset),
                     reinterpret_cast<void**>(&g_orig_Reset), "IDirect3DDevice9::Reset");
    return ok;
}

void uninstall_d3d9_hooks() {
    if (g_imgui_initialized.exchange(false, std::memory_order_acq_rel)) {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    uninstall_wndproc_hook();
}

} // namespace bloom::render
