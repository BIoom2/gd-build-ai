#include "hooks/FrameStepper.hpp"

#include "bot/Bot.hpp"
#include "core/Config.hpp"
#include "gd/GDClasses.hpp"
#include "util/Logger.hpp"
#include "util/Time.hpp"

namespace bloom::hooks {

std::atomic<int> g_step_request{0};

namespace {
std::int64_t g_hold_started_at = 0;
std::int64_t g_last_repeat_at  = 0;
bool         g_hold_armed      = false;
} // namespace

void request_step_forward(int count) {
    if (count <= 0) return;
    g_step_request.fetch_add(count, std::memory_order_acq_rel);
}

void request_step_backward(int count) {
    if (count <= 0) return;
    // Pop snapshot(s) from the ringbuffer and rewrite player state. The engine
    // itself doesn't truly run backwards; we just rewind the visual + physics
    // observables Bloom captures.
    bot::StateSnapshot s;
    for (int i = 0; i < count; ++i) {
        if (!bot::Bot::instance().pop_state_snapshot(s)) break;
        if (auto* pl = gd::shared_play_layer()) {
            if (auto* p1 = gd::player1(pl)) {
                auto& pos = gd::position(p1);
                pos.x = s.p1_x; pos.y = s.p1_y;
                gd::y_velocity(p1) = s.p1_yv;
                gd::rotation(p1)   = s.p1_rot;
            }
            if (auto* p2 = gd::player2(pl)) {
                auto& pos = gd::position(p2);
                pos.x = s.p2_x; pos.y = s.p2_y;
                gd::y_velocity(p2) = s.p2_yv;
                gd::rotation(p2)   = s.p2_rot;
            }
        }
        bot::Bot::instance().set_current_frame(s.frame);
    }
}

void request_hold_start() {
    g_hold_armed = true;
    g_hold_started_at = util::now_ms();
    g_last_repeat_at  = 0;
}

void tick_auto_hold() {
    auto& cfg = core::get_config();
    if (!g_hold_armed) return;
    auto now = util::now_ms();
    if (now - g_hold_started_at < cfg.hold_start_ms) return;
    if (cfg.hold_interval_ms <= 0) return;
    if (now - g_last_repeat_at < cfg.hold_interval_ms) return;
    g_last_repeat_at = now;
    request_step_forward(1);
}

} // namespace bloom::hooks
