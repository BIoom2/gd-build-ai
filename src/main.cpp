// Bloom entry point. The DLL is loaded by GeometryDash.exe via any standard
// DLL injector. We spin up the Bloom core on a worker thread so DllMain itself
// stays minimal (no loader-lock hazards).

#include <windows.h>

#include <filesystem>
#include <string>
#include <thread>

#include "core/Bloom.hpp"

namespace {

std::string dll_directory(HMODULE module) {
    char path[MAX_PATH] = {};
    GetModuleFileNameA(module, path, MAX_PATH);
    return std::filesystem::path(path).parent_path().string();
}

DWORD WINAPI bloom_thread(LPVOID lpParam) {
    auto* dir = static_cast<std::string*>(lpParam);
    bloom::core::Bloom::instance().start(*dir);
    delete dir;
    return 0;
}

} // namespace

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID /*reserved*/) {
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            auto* dir = new std::string(dll_directory(hModule));
            HANDLE t = CreateThread(nullptr, 0, &bloom_thread, dir, 0, nullptr);
            if (t) CloseHandle(t);
            break;
        }
        case DLL_PROCESS_DETACH:
            bloom::core::Bloom::instance().stop();
            break;
        default:
            break;
    }
    return TRUE;
}
