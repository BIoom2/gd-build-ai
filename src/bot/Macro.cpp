#include "bot/Macro.hpp"

#include <algorithm>
#include <fstream>

#include "util/Logger.hpp"

namespace bloom::bot {

nlohmann::json Macro::to_json() const {
    nlohmann::json j;
    j["name"]    = name;
    j["version"] = version;
    j["fps"]     = fps;
    j["dual"]    = dual;
    j["total_frames"] = total_frames;

    auto& jf = j["frames"];
    jf = nlohmann::json::array();
    jf.get_ref<nlohmann::json::array_t&>().reserve(frames.size());
    for (const auto& f : frames) {
        jf.push_back({
            {"frame",  f.frame},
            {"player", f.player},
            {"button", f.button},
            {"down",   f.down},
            {"x",      f.x},
            {"y",      f.y},
            {"yv",     f.y_velocity},
            {"rot",    f.rotation},
        });
    }
    return j;
}

Macro Macro::from_json(const nlohmann::json& j) {
    Macro m;
    if (j.contains("name"))    m.name    = j["name"].get<std::string>();
    if (j.contains("version")) m.version = j["version"].get<std::string>();
    if (j.contains("fps"))     m.fps     = j["fps"].get<int>();
    if (j.contains("dual"))    m.dual    = j["dual"].get<bool>();
    if (j.contains("total_frames")) m.total_frames = j["total_frames"].get<std::int64_t>();

    if (j.contains("frames") && j["frames"].is_array()) {
        m.frames.reserve(j["frames"].size());
        for (const auto& je : j["frames"]) {
            Frame f;
            if (je.contains("frame"))  f.frame  = je["frame"].get<std::int64_t>();
            if (je.contains("player")) f.player = je["player"].get<std::uint8_t>();
            if (je.contains("button")) f.button = je["button"].get<std::uint8_t>();
            if (je.contains("down"))   f.down   = je["down"].get<bool>();
            if (je.contains("x"))      f.x      = je["x"].get<float>();
            if (je.contains("y"))      f.y      = je["y"].get<float>();
            if (je.contains("yv"))     f.y_velocity = je["yv"].get<float>();
            if (je.contains("rot"))    f.rotation   = je["rot"].get<float>();
            m.frames.push_back(f);
        }
    }
    return m;
}

bool Macro::save(const std::string& path) const {
    std::ofstream f(path);
    if (!f) {
        BLOOM_LOG_ERROR("Could not write macro to %s", path.c_str());
        return false;
    }
    f << to_json().dump(2);
    return true;
}

bool Macro::load(const std::string& path, Macro& out) {
    std::ifstream f(path);
    if (!f) return false;
    try {
        nlohmann::json j;
        f >> j;
        out = from_json(j);
        return true;
    } catch (const std::exception& e) {
        BLOOM_LOG_ERROR("Failed to load macro %s: %s", path.c_str(), e.what());
        return false;
    }
}

std::size_t Macro::first_frame_index_at_or_after(std::int64_t physics_frame) const {
    auto it = std::lower_bound(frames.begin(), frames.end(), physics_frame,
        [](const Frame& a, std::int64_t v) { return a.frame < v; });
    return static_cast<std::size_t>(it - frames.begin());
}

} // namespace bloom::bot
