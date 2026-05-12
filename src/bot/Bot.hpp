#pragma once

#include <atomic>
#include <cstdint>
#include <deque>
#include <mutex>

#include "bot/Macro.hpp"

namespace bloom::bot {

enum class BotMode {
    Idle      = 0,
    Recording = 1,
    Playback  = 2,
};

// Per-physics-frame state snapshot for the frame-stepper backstep ringbuffer.
struct StateSnapshot {
    std::int64_t frame = 0;
    float p1_x = 0.f, p1_y = 0.f, p1_yv = 0.f, p1_rot = 0.f;
    float p2_x = 0.f, p2_y = 0.f, p2_yv = 0.f, p2_rot = 0.f;
};

class Bot {
public:
    static Bot& instance();

    // Mode management.
    BotMode mode() const { return m_mode.load(std::memory_order_acquire); }
    void    set_mode(BotMode m);

    // Macro currently being recorded or played back.
    Macro& current_macro() { return m_macro; }
    const Macro& current_macro() const { return m_macro; }

    // Reset state for a fresh attempt (PlayLayer::resetLevel hook calls this).
    void on_level_reset();
    void on_level_start();

    // Called every physics step (PlayLayer::update hook). Drives recording
    // snapshots and playback input dispatch.
    void on_physics_step(std::int64_t frame);

    // Called from pushButton / releaseButton hooks to capture input events
    // when recording.
    void on_input_event(std::uint8_t player, std::uint8_t button, bool down,
                        std::int64_t frame);

    // Frame stepper utilities.
    void push_state_snapshot(const StateSnapshot& s);
    bool pop_state_snapshot(StateSnapshot& out);
    std::size_t snapshot_count() const { return m_snapshots.size(); }

    // Playback queries.
    bool has_input_for_frame(std::int64_t frame) const;

    // For replay: returns the next pending input at `frame` (or nullptr).
    const Frame* next_replay_frame_at(std::int64_t frame);

    // Current physics frame (since level start).
    std::int64_t current_frame() const { return m_current_frame; }
    void         set_current_frame(std::int64_t f) { m_current_frame = f; }

private:
    Bot() = default;
    Bot(const Bot&) = delete;
    Bot& operator=(const Bot&) = delete;

    std::atomic<BotMode> m_mode{BotMode::Idle};
    Macro m_macro;
    mutable std::mutex m_macro_mutex;

    std::size_t m_replay_cursor = 0;
    std::int64_t m_current_frame = 0;

    std::deque<StateSnapshot> m_snapshots;
    mutable std::mutex m_snapshots_mutex;
};

} // namespace bloom::bot
