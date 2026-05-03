#include "Settings.hpp"

using namespace geode::prelude;

namespace gd21 {

Settings& Settings::get() {
    static Settings s;
    return s;
}

void Settings::refreshFromMod() {
    auto mod = Mod::get();

    enabled       = mod->getSettingValue<bool>("enabled");
    physicsRate   = mod->getSettingValue<std::string>("physics-rate");
    tcbotCompat   = mod->getSettingValue<bool>("tcbot-compat");

    bool slope    = mod->getSettingValue<bool>("slope-physics-21");
    bool orb      = mod->getSettingValue<bool>("orb-priority-21");
    bool pad      = mod->getSettingValue<bool>("pad-bug-21");
    bool wave     = mod->getSettingValue<bool>("wave-hitbox-21");
    bool keepBugs = mod->getSettingValue<bool>("preserve-21-bugs");
    bool logP     = mod->getSettingValue<bool>("log-physics");

    // Step-rate model: 2.1 unless the user picked "fixed-240" OR turned on
    // TCBot Compat (which forces fixed-240 so input macros keep their
    // recorded timing).
    bool wantFps21 = enabled && physicsRate == "fps-locked-21" && !tcbotCompat;

    useFpsLocked21Mode.store(wantFps21,           std::memory_order_relaxed);
    slopePhysics21.store(   enabled && slope,     std::memory_order_relaxed);
    orbPriority21.store(    enabled && orb,       std::memory_order_relaxed);
    padBug21.store(         enabled && pad,       std::memory_order_relaxed);
    waveHitbox21.store(     enabled && wave,      std::memory_order_relaxed);
    preserve21Bugs.store(   enabled && keepBugs,  std::memory_order_relaxed);
    logPhysics.store(       logP,                 std::memory_order_relaxed);
}

} // namespace gd21
