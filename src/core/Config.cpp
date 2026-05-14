#include "core/Config.hpp"

#include <fstream>

#include "util/Logger.hpp"

namespace bloom::core {

namespace {

template <typename T>
void load_field(const nlohmann::json& j, const char* key, T& out) {
    auto it = j.find(key);
    if (it == j.end()) return;
    try {
        out = it->get<T>();
    } catch (...) {
        // leave default
    }
}

void load_color(const nlohmann::json& j, const char* key, std::array<float, 4>& out) {
    auto it = j.find(key);
    if (it == j.end() || !it->is_array() || it->size() != 4) return;
    for (std::size_t i = 0; i < 4; ++i) {
        out[i] = it->at(i).get<float>();
    }
}

} // namespace

nlohmann::json Config::to_json() const {
    nlohmann::json j;
    j["version"] = "bloom-1";

    j["enable_bot"] = enable_bot;
    j["enable_lua_plugin"] = enable_lua_plugin;
    j["advanced_settings"] = advanced_settings;
    j["dual_fps_mode"] = dual_fps_mode;
    j["dont_overwrite_macro"] = dont_overwrite_macro;
    j["save_macro_on_exit"] = save_macro_on_exit;
    j["ignore_inputs_on_replay"] = ignore_inputs_on_replay;
    j["show_frame"] = show_frame;

    j["current_macro_name"] = current_macro_name;
    j["recording_mode"] = recording_mode;

    j["practice_mode_fix"] = practice_mode_fix;
    j["practice_rotation_fix"] = practice_rotation_fix;

    j["native_speed"] = native_speed;
    j["physics_fps"] = physics_fps;
    j["display_fps"] = display_fps;
    j["fps_bypass_enabled"] = fps_bypass_enabled;

    j["frame_advance"] = frame_advance;
    j["backwards_stepping"] = backwards_stepping;
    j["universal_frame_advance"] = universal_frame_advance;
    j["hold_start_ms"] = hold_start_ms;
    j["hold_interval_ms"] = hold_interval_ms;
    j["store_backsteps"] = store_backsteps;

    j["ac_p1_enabled"] = ac_p1_enabled;
    j["ac_p2_enabled"] = ac_p2_enabled;
    j["ac_click"] = ac_click;
    j["ac_release"] = ac_release;
    j["ac_click_every_frames"] = ac_click_every_frames;
    j["ac_release_after_frames"] = ac_release_after_frames;
    j["ac_player1_id"] = ac_player1_id;
    j["ac_player2_id"] = ac_player2_id;

    j["pickup_enabled"] = pickup_enabled;
    j["pickup_click_id"] = pickup_click_id;
    j["pickup_release_id"] = pickup_release_id;
    j["pickup_player1_id"] = pickup_player1_id;
    j["pickup_player2_id"] = pickup_player2_id;

    j["pulse_trigger_loop"] = pulse_trigger_loop;

    j["menu_color"] = menu_color;
    j["menu_size"] = menu_size;
    j["menu_search"] = menu_search;

    j["recorder_width"] = recorder_width;
    j["recorder_height"] = recorder_height;
    j["recorder_fps"] = recorder_fps;
    j["recorder_bitrate_mbps"] = recorder_bitrate_mbps;
    j["recorder_codec"] = recorder_codec;
    j["recorder_extra_args"] = recorder_extra_args;
    j["recorder_arguments_vf"] = recorder_arguments_vf;
    j["recorder_arguments_audio"] = recorder_arguments_audio;
    j["recorder_format"] = recorder_format;
    j["recorder_output"] = recorder_output;
    j["recorder_stop_after_endscreen_sec"] = recorder_stop_after_endscreen_sec;

    j["kb_show_menu"] = kb_show_menu;
    j["kb_view_keybinds"] = kb_view_keybinds;
    j["kb_search_controls"] = kb_search_controls;
    j["kb_toggle_record"] = kb_toggle_record;
    j["kb_step_forward"] = kb_step_forward;
    j["kb_step_backward"] = kb_step_backward;
    j["kb_hold_start"] = kb_hold_start;
    j["kb_double_alt_toggle_enabled"] = kb_double_alt_toggle_enabled;
    j["kb_double_alt_window_ms"] = kb_double_alt_window_ms;

    return j;
}

void Config::from_json(const nlohmann::json& j) {
    load_field(j, "enable_bot", enable_bot);
    load_field(j, "enable_lua_plugin", enable_lua_plugin);
    load_field(j, "advanced_settings", advanced_settings);
    load_field(j, "dual_fps_mode", dual_fps_mode);
    load_field(j, "dont_overwrite_macro", dont_overwrite_macro);
    load_field(j, "save_macro_on_exit", save_macro_on_exit);
    load_field(j, "ignore_inputs_on_replay", ignore_inputs_on_replay);
    load_field(j, "show_frame", show_frame);

    load_field(j, "current_macro_name", current_macro_name);
    load_field(j, "recording_mode", recording_mode);

    load_field(j, "practice_mode_fix", practice_mode_fix);
    load_field(j, "practice_rotation_fix", practice_rotation_fix);

    load_field(j, "native_speed", native_speed);
    load_field(j, "physics_fps", physics_fps);
    load_field(j, "display_fps", display_fps);
    load_field(j, "fps_bypass_enabled", fps_bypass_enabled);

    load_field(j, "frame_advance", frame_advance);
    load_field(j, "backwards_stepping", backwards_stepping);
    load_field(j, "universal_frame_advance", universal_frame_advance);
    load_field(j, "hold_start_ms", hold_start_ms);
    load_field(j, "hold_interval_ms", hold_interval_ms);
    load_field(j, "store_backsteps", store_backsteps);

    load_field(j, "ac_p1_enabled", ac_p1_enabled);
    load_field(j, "ac_p2_enabled", ac_p2_enabled);
    load_field(j, "ac_click", ac_click);
    load_field(j, "ac_release", ac_release);
    load_field(j, "ac_click_every_frames", ac_click_every_frames);
    load_field(j, "ac_release_after_frames", ac_release_after_frames);
    load_field(j, "ac_player1_id", ac_player1_id);
    load_field(j, "ac_player2_id", ac_player2_id);

    load_field(j, "pickup_enabled", pickup_enabled);
    load_field(j, "pickup_click_id", pickup_click_id);
    load_field(j, "pickup_release_id", pickup_release_id);
    load_field(j, "pickup_player1_id", pickup_player1_id);
    load_field(j, "pickup_player2_id", pickup_player2_id);

    load_field(j, "pulse_trigger_loop", pulse_trigger_loop);

    load_color(j, "menu_color", menu_color);
    load_field(j, "menu_size", menu_size);
    load_field(j, "menu_search", menu_search);

    load_field(j, "recorder_width", recorder_width);
    load_field(j, "recorder_height", recorder_height);
    load_field(j, "recorder_fps", recorder_fps);
    load_field(j, "recorder_bitrate_mbps", recorder_bitrate_mbps);
    load_field(j, "recorder_codec", recorder_codec);
    load_field(j, "recorder_extra_args", recorder_extra_args);
    load_field(j, "recorder_arguments_vf", recorder_arguments_vf);
    load_field(j, "recorder_arguments_audio", recorder_arguments_audio);
    load_field(j, "recorder_format", recorder_format);
    load_field(j, "recorder_output", recorder_output);
    load_field(j, "recorder_stop_after_endscreen_sec", recorder_stop_after_endscreen_sec);

    load_field(j, "kb_show_menu", kb_show_menu);
    load_field(j, "kb_view_keybinds", kb_view_keybinds);
    load_field(j, "kb_search_controls", kb_search_controls);
    load_field(j, "kb_toggle_record", kb_toggle_record);
    load_field(j, "kb_step_forward", kb_step_forward);
    load_field(j, "kb_step_backward", kb_step_backward);
    load_field(j, "kb_hold_start", kb_hold_start);
    load_field(j, "kb_double_alt_toggle_enabled", kb_double_alt_toggle_enabled);
    load_field(j, "kb_double_alt_window_ms", kb_double_alt_window_ms);
}

bool Config::save(const std::string& path) const {
    std::ofstream f(path);
    if (!f) {
        BLOOM_LOG_ERROR("Could not write config to %s", path.c_str());
        return false;
    }
    f << to_json().dump(2);
    return true;
}

bool Config::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    try {
        nlohmann::json j;
        f >> j;
        from_json(j);
        return true;
    } catch (const std::exception& e) {
        BLOOM_LOG_ERROR("Failed to parse config %s: %s", path.c_str(), e.what());
        return false;
    }
}

Config& get_config() {
    static Config cfg;
    return cfg;
}

} // namespace bloom::core
