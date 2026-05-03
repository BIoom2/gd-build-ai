#include "StepRatePatch.hpp"
#include "Settings.hpp"

#include <Geode/Geode.hpp>
#include <Geode/loader/Mod.hpp>

#include <algorithm>
#include <cmath>

using namespace geode::prelude;

namespace gd21 {

int g_stepCount = 4;

#ifdef GEODE_IS_WINDOWS

namespace {
    // Helper from CBF: write a pointer into a byte vector at the given offset.
    void writeAddr(std::vector<uint8_t>& bytes, size_t offset, void* ptr) {
        auto v = reinterpret_cast<uintptr_t>(ptr);
        for (int i = 0; i < 8; i++) {
            bytes[offset + i] = static_cast<uint8_t>((v >> (i * 8)) & 0xff);
        }
    }

    // Patch sits in the middle of GJBaseGameLayer::update, on the path that
    // calculates the per-frame step count. Verified for GD 2.2081 (Win).
    constexpr uintptr_t PATCH_OFFSET = 0x237a91;

    Patch* g_patch = nullptr;
}

void enableStepRatePatch(bool enable) {
    auto* addr = reinterpret_cast<void*>(geode::base::get() + PATCH_OFFSET);

    if (!g_patch) {
        // mov rcx, &g_stepCount
        // mov r11d, dword ptr [rcx]
        std::vector<uint8_t> bytes = {
            0x48, 0xb9, 0, 0, 0, 0, 0, 0, 0, 0,
            0x44, 0x8b, 0x19,
        };
        writeAddr(bytes, 2, &g_stepCount);

        auto patchRes = Mod::get()->patch(addr, bytes);
        if (!patchRes) {
            log::error("gd21: failed to install step-rate patch: {}", patchRes.unwrapErr());
            return;
        }
        g_patch = patchRes.unwrap();
        log::info("gd21: step-rate patch installed at {}", addr);
    }

    if (enable) {
        (void) g_patch->enable();
    } else {
        (void) g_patch->disable();
    }
}

#else // non-Windows: no patch, settings simply have no effect

void enableStepRatePatch(bool /*enable*/) {
    // The mod currently only supports Windows; the patch is a no-op elsewhere.
}

#endif

int compute21StepCount(double delta, float timewarp) {
    // 2.1: at least 4 substeps per frame, scaled with delta.
    // 60 fps -> 4 substeps; 240 fps -> 16; 360 fps -> 24; etc.
    double tw = std::min(1.0f, timewarp);
    double raw = std::max(4.0, delta * 240.0) / tw;
    return static_cast<int>(std::round(raw));
}

int computeVanillaStepCount(double delta, float timewarp) {
    // Vanilla 2.2: fixed 240 Hz. Same formula GD uses internally.
    double tw = std::min(1.0f, timewarp);
    double raw = std::max(1.0, ((delta * 60.0) / tw) * 4.0);
    return static_cast<int>(std::round(raw));
}

} // namespace gd21
