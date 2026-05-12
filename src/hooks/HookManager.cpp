#include "hooks/HookManager.hpp"

#include "util/Logger.hpp"

#include <MinHook.h>

namespace bloom::hooks {

HookManager::HookManager() {
    if (MH_Initialize() == MH_OK) {
        m_initialized = true;
        BLOOM_LOG_INFO("MinHook initialized");
    } else {
        BLOOM_LOG_ERROR("MinHook initialization failed");
    }
}

HookManager::~HookManager() {
    if (m_initialized) {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        m_initialized = false;
    }
}

bool HookManager::install(void* target, void* detour, void** out_original, const char* tag) {
    if (!m_initialized) {
        BLOOM_LOG_ERROR("HookManager::install %s called before init", tag);
        return false;
    }
    if (!target) {
        BLOOM_LOG_ERROR("Hook target %s is null - skipping", tag);
        return false;
    }
    MH_STATUS s = MH_CreateHook(target, detour, out_original);
    if (s != MH_OK) {
        BLOOM_LOG_ERROR("MH_CreateHook(%s) failed: %d", tag, s);
        return false;
    }
    s = MH_EnableHook(target);
    if (s != MH_OK) {
        BLOOM_LOG_ERROR("MH_EnableHook(%s) failed: %d", tag, s);
        return false;
    }
    m_installed.push_back({target, detour, tag});
    BLOOM_LOG_INFO("Hook installed: %s @ %p", tag, target);
    return true;
}

bool HookManager::enable_all() {
    if (!m_initialized) return false;
    return MH_EnableHook(MH_ALL_HOOKS) == MH_OK;
}

bool HookManager::disable_all() {
    if (!m_initialized) return false;
    return MH_DisableHook(MH_ALL_HOOKS) == MH_OK;
}

HookManager& HookManager::instance() {
    static HookManager hm;
    return hm;
}

} // namespace bloom::hooks
