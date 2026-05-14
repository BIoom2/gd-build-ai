#include "hooks/PulseTriggerHook.hpp"

#include <mutex>
#include <vector>

#include "core/Config.hpp"
#include "gd/Addresses.hpp"
#include "gd/GDClasses.hpp"
#include "hooks/HookManager.hpp"
#include "util/Logger.hpp"

namespace bloom::hooks {

namespace {

// ---------------------------------------------------------------------------
// EffectGameObject::triggerObject hook
// ---------------------------------------------------------------------------

using EffectGameObject_triggerObject_t =
    void (__fastcall*)(void* /*this*/, void* /*edx*/, void* /*gameLayer*/);

EffectGameObject_triggerObject_t g_orig_triggerObject = nullptr;

// Per-pulse tracking entry.
struct PulseEntry {
    void* trigger;      // EffectGameObject* that fired the pulse.
    void* game_layer;   // GJBaseGameLayer* passed to triggerObject.
    float cycle_time;   // Total pulse duration (fadeIn + hold + fadeOut).
    float elapsed;      // Seconds since last (re-)trigger.
};

std::mutex g_pulse_mutex;
std::vector<PulseEntry> g_tracked_pulses;

void __fastcall hk_triggerObject(void* This, void* /*edx*/, void* gameLayer) {
    // Always call the original so every trigger (not just pulses) works normally.
    if (g_orig_triggerObject) g_orig_triggerObject(This, nullptr, gameLayer);

    auto& cfg = core::get_config();
    if (!cfg.pulse_trigger_loop) return;

    // Check whether this trigger is a Pulse trigger (object ID 1006).
    int id = gd::object_id(This);
    if (id != gd::k_PulseTriggerObjectID) return;

    auto* eff = static_cast<gd::EffectGameObject*>(This);
    float fade_in  = gd::effect_fade_in(eff);
    float hold     = gd::effect_hold(eff);
    float fade_out = gd::effect_fade_out(eff);
    float cycle    = fade_in + hold + fade_out;

    // Avoid infinite-frequency retrigger when all durations are zero.
    if (cycle < 0.05f) cycle = 0.05f;

    std::lock_guard<std::mutex> lk(g_pulse_mutex);

    // Avoid duplicate tracking of the same trigger object.
    for (auto& p : g_tracked_pulses) {
        if (p.trigger == This) {
            p.elapsed    = 0.f;
            p.cycle_time = cycle;
            p.game_layer = gameLayer;
            return;
        }
    }

    g_tracked_pulses.push_back({This, gameLayer, cycle, 0.f});
    BLOOM_LOG_INFO("PulseLoop: tracking trigger %p (cycle=%.3fs)", This, cycle);
}

} // namespace

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void install_pulse_trigger_hooks() {
    static bool installed = false;
    if (installed) return;
    installed = true;

    HookManager::instance().install(
        reinterpret_cast<void*>(
            gd::resolve_gd(gd::gd21::k_EffectGameObject_triggerObject)),
        reinterpret_cast<void*>(&hk_triggerObject),
        reinterpret_cast<void**>(&g_orig_triggerObject),
        "EffectGameObject::triggerObject");
}

void tick_pulse_trigger_loop(void* /*play_layer*/, float dt) {
    auto& cfg = core::get_config();
    if (!cfg.pulse_trigger_loop) return;

    std::lock_guard<std::mutex> lk(g_pulse_mutex);
    for (auto& p : g_tracked_pulses) {
        p.elapsed += dt;
        if (p.elapsed >= p.cycle_time) {
            p.elapsed -= p.cycle_time;
            // Re-fire the pulse trigger so the effect starts a new cycle.
            if (g_orig_triggerObject) {
                g_orig_triggerObject(p.trigger, nullptr, p.game_layer);
            }
        }
    }
}

void reset_pulse_trigger_state() {
    std::lock_guard<std::mutex> lk(g_pulse_mutex);
    g_tracked_pulses.clear();
}

} // namespace bloom::hooks
