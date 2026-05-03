#pragma once

#include <Geode/Geode.hpp>

#include <atomic>
#include <string>

namespace gd21 {

// Cached view of the mod's settings, refreshed from the Geode Mod object
// whenever the user toggles something. We keep small atomic flags for
// values read inside hot per-frame / per-physics-step hooks so the GUI
// thread can never tear them.
struct Settings {
    // ---- Hot-path atomics (read every frame / step) -----------------
    std::atomic<bool> useFpsLocked21Mode{true};
    std::atomic<bool> slopePhysics21{true};
    std::atomic<bool> orbPriority21{true};
    std::atomic<bool> padBug21{true};
    std::atomic<bool> waveHitbox21{true};
    std::atomic<bool> preserve21Bugs{true};
    std::atomic<bool> logPhysics{false};

    // ---- Cold settings (only read on apply / UI) --------------------
    bool enabled = true;
    std::string physicsRate = "fps-locked-21"; // "fps-locked-21" | "fixed-240"
    bool tcbotCompat = false;

    static Settings& get();
    void refreshFromMod();
};

} // namespace gd21
