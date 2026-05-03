#include "Settings.hpp"

using namespace geode::prelude;

namespace gd21 {

Settings& Settings::get() {
    static Settings s;
    return s;
}

void Settings::refreshFromMod() {
    auto mod = Mod::get();
    enabled = mod->getSettingValue<bool>("enabled");
    physicsRate = mod->getSettingValue<std::string>("physics-rate");
    slopePhysics21 = mod->getSettingValue<bool>("slope-physics-21");
    preserve21Bugs = mod->getSettingValue<bool>("preserve-21-bugs");
    logPhysics = mod->getSettingValue<bool>("log-physics");

    // Re-derive the hot-path flag.
    useFpsLocked21Mode.store(
        enabled && physicsRate == "fps-locked-21",
        std::memory_order_relaxed
    );
}

} // namespace gd21
