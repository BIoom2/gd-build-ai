#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace bloom::bot {

// One recorded input event. We capture both the input (down/up + which button)
// and a snapshot of the player's state at that frame. The snapshot is what
// powers Practice Fix and Practice Rotation Fix at replay time.
struct Frame {
    std::int64_t frame  = 0;  // physics-frame index
    std::uint8_t player = 1;  // 1 or 2
    std::uint8_t button = 1;  // 1 = jump, 2 = left, 3 = right (GD button IDs)
    bool         down   = false;

    // Per-frame state snapshot (filled in when `Bot::record_player_state`
    // runs at the same physics step the input happened on).
    float x = 0.f;
    float y = 0.f;
    float y_velocity = 0.f;
    float rotation = 0.f;
};

struct Macro {
    std::string  name;
    std::string  version = "bloom-1";
    int          fps     = 60;   // physics fps the macro was recorded at
    bool         dual    = false;
    std::int64_t total_frames = 0;
    std::vector<Frame> frames;

    nlohmann::json to_json() const;
    static Macro   from_json(const nlohmann::json& j);

    bool save(const std::string& path) const;
    static bool load(const std::string& path, Macro& out);

    // Helpers.
    void clear() { frames.clear(); total_frames = 0; }

    // Binary search for the first frame at or after the given physics frame.
    std::size_t first_frame_index_at_or_after(std::int64_t physics_frame) const;
};

} // namespace bloom::bot
