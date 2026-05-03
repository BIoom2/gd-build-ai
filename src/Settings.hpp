#pragma once

#include <Geode/Geode.hpp>

#include <atomic>
#include <string>

namespace maxq {

// Cached view of the mod's settings, refreshed from the Geode Mod object
// whenever the user toggles something. Hot-path values are atomics so the
// per-frame texture / particle hooks can read them lock-free without
// tearing relative to GUI-thread writes.
struct Settings {
    // ---- Master toggle ---------------------------------------------
    std::atomic<bool> enabled{true};

    // ---- Texture pipeline ------------------------------------------
    std::atomic<bool>  forceUhd{true};
    std::atomic<int>   anisotropic{16};
    // 0 = nearest, 1 = bilinear, 2 = trilinear
    std::atomic<int>   filterMode{2};

    // ---- Particles -------------------------------------------------
    std::atomic<float> particleMultiplier{2.0f};
    std::atomic<bool>  uncapParticles{true};

    // ---- Debug ------------------------------------------------------
    std::atomic<bool> verboseLog{false};

    static Settings& get();

    // Re-reads every value from the Geode Mod object's settings store
    // and writes them into the atomic / scalar fields above. Called on
    // mod load and on every settingChanged event.
    void refreshFromMod();
};

// Filter-mode enum mirroring `filterMode` for readability.
enum FilterMode : int {
    kFilterNearest   = 0,
    kFilterBilinear  = 1,
    kFilterTrilinear = 2,
};

} // namespace maxq
