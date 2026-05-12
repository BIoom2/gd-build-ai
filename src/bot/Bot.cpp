#include "bot/Bot.hpp"

#include <algorithm>

#include "core/Config.hpp"
#include "gd/GDClasses.hpp"
#include "util/Logger.hpp"

namespace bloom::bot {

Bot& Bot::instance() {
    static Bot b;
    return b;
}

void Bot::set_mode(BotMode m) {
    auto prev = m_mode.exchange(m, std::memory_order_acq_rel);
    if (prev == m) return;
    BLOOM_LOG_INFO("Bot mode -> %d", static_cast<int>(m));

    if (m == BotMode::Recording) {
        std::lock_guard<std::mutex> lock(m_macro_mutex);
        m_macro.clear();
    } else if (m == BotMode::Playback) {
        m_replay_cursor = 0;
    }
}

void Bot::on_level_reset() {
    m_current_frame = 0;
    m_replay_cursor = 0;
    if (mode() == BotMode::Recording) {
        // Truncate any frames recorded past the current point; user wants to
        // re-record from this checkpoint.
        std::lock_guard<std::mutex> lock(m_macro_mutex);
        m_macro.frames.erase(
            std::remove_if(m_macro.frames.begin(), m_macro.frames.end(),
                           [](const Frame&) { return true; }),
            m_macro.frames.end());
        // Actually clear() - the simpler thing; checkpoints from practice
        // mode are handled by PracticeFix which rewrites cursors instead.
        m_macro.clear();
    }
    {
        std::lock_guard<std::mutex> lock(m_snapshots_mutex);
        m_snapshots.clear();
    }
}

void Bot::on_level_start() {
    m_current_frame = 0;
    m_replay_cursor = 0;
}

void Bot::on_physics_step(std::int64_t frame) {
    m_current_frame = frame;

    // Take a state snapshot for the backstep ringbuffer.
    auto& cfg = core::get_config();
    if (cfg.frame_advance || cfg.backwards_stepping || cfg.universal_frame_advance) {
        StateSnapshot s;
        s.frame = frame;
        if (auto* pl = gd::shared_play_layer()) {
            if (auto* p1 = gd::player1(pl)) {
                auto& pos = gd::position(p1);
                s.p1_x = pos.x; s.p1_y = pos.y;
                s.p1_yv = gd::y_velocity(p1);
                s.p1_rot = gd::rotation(p1);
            }
            if (auto* p2 = gd::player2(pl)) {
                auto& pos = gd::position(p2);
                s.p2_x = pos.x; s.p2_y = pos.y;
                s.p2_yv = gd::y_velocity(p2);
                s.p2_rot = gd::rotation(p2);
            }
        }
        push_state_snapshot(s);
    }
}

void Bot::on_input_event(std::uint8_t player, std::uint8_t button, bool down,
                         std::int64_t frame) {
    if (mode() != BotMode::Recording) return;

    Frame f;
    f.frame  = frame;
    f.player = player;
    f.button = button;
    f.down   = down;

    if (auto* pl = gd::shared_play_layer()) {
        auto* po = (player == 2) ? gd::player2(pl) : gd::player1(pl);
        if (po) {
            auto& pos = gd::position(po);
            f.x = pos.x;
            f.y = pos.y;
            f.y_velocity = gd::y_velocity(po);
            f.rotation   = gd::rotation(po);
        }
    }

    std::lock_guard<std::mutex> lock(m_macro_mutex);
    m_macro.frames.push_back(f);
    m_macro.total_frames = std::max(m_macro.total_frames, frame);
}

void Bot::push_state_snapshot(const StateSnapshot& s) {
    auto cap = static_cast<std::size_t>(std::max(0, core::get_config().store_backsteps));
    std::lock_guard<std::mutex> lock(m_snapshots_mutex);
    m_snapshots.push_back(s);
    while (m_snapshots.size() > cap && !m_snapshots.empty()) {
        m_snapshots.pop_front();
    }
}

bool Bot::pop_state_snapshot(StateSnapshot& out) {
    std::lock_guard<std::mutex> lock(m_snapshots_mutex);
    if (m_snapshots.empty()) return false;
    out = m_snapshots.back();
    m_snapshots.pop_back();
    return true;
}

bool Bot::has_input_for_frame(std::int64_t frame) const {
    if (mode() != BotMode::Playback) return false;
    std::lock_guard<std::mutex> lock(m_macro_mutex);
    if (m_replay_cursor >= m_macro.frames.size()) return false;
    return m_macro.frames[m_replay_cursor].frame == frame;
}

const Frame* Bot::next_replay_frame_at(std::int64_t frame) {
    if (mode() != BotMode::Playback) return nullptr;
    std::lock_guard<std::mutex> lock(m_macro_mutex);
    if (m_replay_cursor >= m_macro.frames.size()) return nullptr;
    if (m_macro.frames[m_replay_cursor].frame != frame) return nullptr;
    return &m_macro.frames[m_replay_cursor++];
}

} // namespace bloom::bot
