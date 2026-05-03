#pragma once

#include <Geode/Geode.hpp>

#include <atomic>
#include <string>

namespace gd21 {

// Lightweight cached view of the mod's settings, refreshed whenever the user
// toggles something in the Geode mod menu.
//
// `useFpsLocked21Mode` is the only field read from the per-frame physics
// hook, so we keep it as a `std::atomic<bool>` to avoid a torn read from a
// concurrent settings UI thread without needing a mutex.
struct Settings {
    std::atomic<bool> useFpsLocked21Mode{true};

    bool enabled = true;
    std::string physicsRate = "fps-locked-21"; // "fps-locked-21" or "fixed-240"
    bool slopePhysics21 = true;
    bool preserve21Bugs = true;
    bool logPhysics = false;

    static Settings& get();
    void refreshFromMod();
};

} // namespace gd21
