#include "hooks/PlayLayerHook.hpp"

#include <atomic>
#include <cstdint>

#include "bot/Bot.hpp"
#include "core/Config.hpp"
#include "gd/Addresses.hpp"
#include "gd/GDClasses.hpp"
#include "hooks/FrameStepper.hpp"
#include "hooks/HookManager.hpp"
#include "util/Logger.hpp"

namespace bloom::hooks {

namespace {

using PlayLayer_update_t            = void (__fastcall*)(void*, void*, float);
using PlayLayer_pushButton_t        = void (__fastcall*)(void*, void*, int, bool);
using PlayLayer_releaseButton_t     = void (__fastcall*)(void*, void*, int, bool);
using PlayLayer_resetLevel_t        = void (__fastcall*)(void*, void*);
using PlayLayer_loadFromCheckpoint_t= void (__fastcall*)(void*, void*, void*);
using PlayLayer_togglePractice_t    = void (__fastcall*)(void*, void*, bool);

PlayLayer_update_t             g_orig_update             = nullptr;
PlayLayer_pushButton_t         g_orig_pushButton         = nullptr;
PlayLayer_releaseButton_t      g_orig_releaseButton      = nullptr;
PlayLayer_resetLevel_t         g_orig_resetLevel         = nullptr;
PlayLayer_loadFromCheckpoint_t g_orig_loadFromCheckpoint = nullptr;
PlayLayer_togglePractice_t     g_orig_togglePractice     = nullptr;

std::atomic<std::int64_t> g_physics_frame{0};

void __fastcall hk_update(void* This, void* /*edx*/, float dt) {
    auto& cfg = core::get_config();

    // Frame stepper: skip stepping when paused unless an explicit step has
    // been requested (FrameStepper toggles bloom::hooks::g_step_request).
    if (cfg.frame_advance && bloom::hooks::g_step_request.load(std::memory_order_acquire) == 0) {
        // Hold the world, don't tick.
        return;
    }
    if (cfg.frame_advance && bloom::hooks::g_step_request.load(std::memory_order_acquire) > 0) {
        bloom::hooks::g_step_request.fetch_sub(1, std::memory_order_acq_rel);
    }

    // Physics FPS bypass: override dt so the engine advances at the target rate.
    if (cfg.fps_bypass_enabled && cfg.physics_fps > 0) {
        dt = 1.0f / static_cast<float>(cfg.physics_fps);
        dt *= cfg.native_speed;
    } else {
        dt *= cfg.native_speed;
    }

    auto frame = g_physics_frame.fetch_add(1, std::memory_order_acq_rel);
    bot::Bot::instance().on_physics_step(frame);

    // Dispatch any queued replay inputs scheduled for this frame.
    if (cfg.enable_bot && bot::Bot::instance().mode() == bot::BotMode::Playback) {
        while (const auto* f = bot::Bot::instance().next_replay_frame_at(frame)) {
            void* pl_player = (f->player == 2) ? gd::player2(static_cast<gd::PlayLayer*>(This))
                                               : gd::player1(static_cast<gd::PlayLayer*>(This));
            if (!pl_player) break;
            // Re-enter the original pushButton/releaseButton on This (PlayLayer)
            // so the engine treats it as a player-driven input.
            if (f->down) {
                if (g_orig_pushButton) g_orig_pushButton(This, nullptr, f->button, f->player == 2);
            } else {
                if (g_orig_releaseButton) g_orig_releaseButton(This, nullptr, f->button, f->player == 2);
            }
            (void)pl_player;
        }
    }

    if (g_orig_update) g_orig_update(This, nullptr, dt);
}

void __fastcall hk_pushButton(void* This, void* /*edx*/, int button, bool is_player2) {
    auto& cfg = core::get_config();
    if (cfg.enable_bot && bot::Bot::instance().mode() == bot::BotMode::Recording) {
        bot::Bot::instance().on_input_event(is_player2 ? 2 : 1,
                                            static_cast<std::uint8_t>(button),
                                            true,
                                            g_physics_frame.load());
    }
    if (cfg.enable_bot && cfg.ignore_inputs_on_replay &&
        bot::Bot::instance().mode() == bot::BotMode::Playback) {
        // Swallow live inputs during playback - the replay engine generates them itself.
        return;
    }
    if (g_orig_pushButton) g_orig_pushButton(This, nullptr, button, is_player2);
}

void __fastcall hk_releaseButton(void* This, void* /*edx*/, int button, bool is_player2) {
    auto& cfg = core::get_config();
    if (cfg.enable_bot && bot::Bot::instance().mode() == bot::BotMode::Recording) {
        bot::Bot::instance().on_input_event(is_player2 ? 2 : 1,
                                            static_cast<std::uint8_t>(button),
                                            false,
                                            g_physics_frame.load());
    }
    if (cfg.enable_bot && cfg.ignore_inputs_on_replay &&
        bot::Bot::instance().mode() == bot::BotMode::Playback) {
        return;
    }
    if (g_orig_releaseButton) g_orig_releaseButton(This, nullptr, button, is_player2);
}

void __fastcall hk_resetLevel(void* This, void* /*edx*/) {
    g_physics_frame.store(0, std::memory_order_release);
    bot::Bot::instance().on_level_reset();
    if (g_orig_resetLevel) g_orig_resetLevel(This, nullptr);
}

void __fastcall hk_loadFromCheckpoint(void* This, void* /*edx*/, void* checkpoint) {
    if (g_orig_loadFromCheckpoint) g_orig_loadFromCheckpoint(This, nullptr, checkpoint);

    // Practice Fix: realign the replay cursor / recording head to the physics
    // frame that the checkpoint was captured on. We don't have an authoritative
    // physics-frame stored on the checkpoint object in GD 2.1, so we approximate
    // by mapping `level_time` (seconds) -> nearest recorded frame.
    auto& cfg = core::get_config();
    if (!cfg.practice_mode_fix) return;

    auto* pl = static_cast<gd::PlayLayer*>(This);
    float t = gd::level_time(pl);
    auto& macro = bot::Bot::instance().current_macro();
    auto target_frame = static_cast<std::int64_t>(t * macro.fps);
    g_physics_frame.store(target_frame, std::memory_order_release);
    bot::Bot::instance().set_current_frame(target_frame);
    BLOOM_LOG_INFO("PracticeFix: resync to frame %lld (level_time=%.3f)",
                   static_cast<long long>(target_frame), t);
}

void __fastcall hk_togglePracticeMode(void* This, void* /*edx*/, bool on) {
    if (g_orig_togglePractice) g_orig_togglePractice(This, nullptr, on);
    BLOOM_LOG_INFO("Practice mode toggled -> %s", on ? "ON" : "OFF");
}

} // namespace

void install_play_layer_hooks() {
    static bool installed = false;
    if (installed) return;
    installed = true;

    auto& hm = HookManager::instance();

    hm.install(reinterpret_cast<void*>(gd::resolve_gd(gd::gd21::k_PlayLayer_update)),
               reinterpret_cast<void*>(&hk_update),
               reinterpret_cast<void**>(&g_orig_update),
               "PlayLayer::update");

    hm.install(reinterpret_cast<void*>(gd::resolve_gd(gd::gd21::k_PlayLayer_pushButton)),
               reinterpret_cast<void*>(&hk_pushButton),
               reinterpret_cast<void**>(&g_orig_pushButton),
               "PlayLayer::pushButton");

    hm.install(reinterpret_cast<void*>(gd::resolve_gd(gd::gd21::k_PlayLayer_releaseButton)),
               reinterpret_cast<void*>(&hk_releaseButton),
               reinterpret_cast<void**>(&g_orig_releaseButton),
               "PlayLayer::releaseButton");

    hm.install(reinterpret_cast<void*>(gd::resolve_gd(gd::gd21::k_PlayLayer_resetLevel)),
               reinterpret_cast<void*>(&hk_resetLevel),
               reinterpret_cast<void**>(&g_orig_resetLevel),
               "PlayLayer::resetLevel");

    hm.install(reinterpret_cast<void*>(gd::resolve_gd(gd::gd21::k_PlayLayer_loadFromCheckpoint)),
               reinterpret_cast<void*>(&hk_loadFromCheckpoint),
               reinterpret_cast<void**>(&g_orig_loadFromCheckpoint),
               "PlayLayer::loadFromCheckpoint");

    hm.install(reinterpret_cast<void*>(gd::resolve_gd(gd::gd21::k_PlayLayer_togglePracticeMode)),
               reinterpret_cast<void*>(&hk_togglePracticeMode),
               reinterpret_cast<void**>(&g_orig_togglePractice),
               "PlayLayer::togglePracticeMode");
}

} // namespace bloom::hooks
