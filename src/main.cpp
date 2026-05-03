#include <Geode/Geode.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "Settings.hpp"
#include "StepRatePatch.hpp"

using namespace geode::prelude;
using gd21::Settings;

// ---------------------------------------------------------------------------
// Step-rate hook: pre-computes the per-frame substep count so that GD's
// in-engine step-count read (rewritten by our patch to load from
// `gd21::g_stepCount`) sees the right value for this frame.
//
// We only override the count when actually inside a PlayLayer; outside
// (main menu, editor, end-screen) we fall back to vanilla so editor
// playback / menu animations stay smooth.
// ---------------------------------------------------------------------------
class $modify(Gd21BaseGameLayer, GJBaseGameLayer) {
    static void onModify(auto& self) {
        // Run before mods that hook getModifiedDelta for input timing.
        (void) self.setHookPriority("GJBaseGameLayer::getModifiedDelta", Priority::Early);
    }

    double getModifiedDelta(float delta) {
        double modified = GJBaseGameLayer::getModifiedDelta(delta);

        auto& s = Settings::get();
        auto* pl = PlayLayer::get();
        bool inLevel = pl != nullptr;
        bool use21 = inLevel && s.useFpsLocked21Mode.load(std::memory_order_relaxed);

        float timewarp = inLevel ? pl->m_gameState.m_timeWarp : 1.0f;

        if (use21) {
            gd21::g_stepCount = gd21::compute21StepCount(modified, timewarp);
        } else {
            // Outside a level OR user picked "fixed-240": fall back to the
            // exact vanilla formula so the patch (if still enabled) is a
            // no-op in behavior.
            gd21::g_stepCount = gd21::computeVanillaStepCount(modified, timewarp);
        }

        if (s.logPhysics && inLevel) {
            log::debug(
                "gd21: delta={:.5f} tw={:.3f} mode={} steps={}",
                modified, timewarp, use21 ? "2.1" : "vanilla",
                gd21::g_stepCount
            );
        }

        return modified;
    }
};

// ---------------------------------------------------------------------------
// Entry point: wire up settings & install/uninstall the step-rate patch.
// ---------------------------------------------------------------------------

namespace {
    void applyAllSettings() {
        Settings::get().refreshFromMod();
        auto& s = Settings::get();

        // Install or remove the in-engine step-count read patch. We keep
        // it on whenever the mod is enabled (in any rate mode), because
        // we always want OUR computed value used; the choice between 2.1
        // and vanilla is made inside `getModifiedDelta`.
        gd21::enableStepRatePatch(s.enabled);

        log::info(
            "gd21: settings — enabled={} rate={} slope21={} keepBugs={}",
            s.enabled, s.physicsRate, s.slopePhysics21, s.preserve21Bugs
        );
    }
}

$on_mod(Loaded) {
    applyAllSettings();

    listenForSettingChanges<bool>("enabled",
        +[](bool) { applyAllSettings(); });
    listenForSettingChanges<std::string>("physics-rate",
        +[](std::string) { applyAllSettings(); });
    listenForSettingChanges<bool>("slope-physics-21",
        +[](bool) { applyAllSettings(); });
    listenForSettingChanges<bool>("preserve-21-bugs",
        +[](bool) { applyAllSettings(); });
    listenForSettingChanges<bool>("log-physics",
        +[](bool) { applyAllSettings(); });
}
