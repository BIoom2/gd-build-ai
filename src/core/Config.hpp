#pragma once

#include <array>
#include <string>

#include <nlohmann/json.hpp>

namespace bloom::core {

// Bloom-wide settings, persisted to bloom_settings.json next to Bloom.dll.
// Each window pulls live values from this struct via Config::get(); writes
// happen inline from the menu callbacks and are flushed by Config::save().
struct Config {
    // Master switches.
    bool enable_bot          = true;
    bool enable_lua_plugin   = false;
    bool advanced_settings   = false;
    bool dual_fps_mode       = false;
    bool dont_overwrite_macro= false;
    bool save_macro_on_exit  = true;
    bool ignore_inputs_on_replay = true;
    bool show_frame          = false;

    // Bot.
    std::string current_macro_name;
    int  recording_mode = 1; // 0 = recording, 1 = playback (matches screenshot)

    // Practice fixes.
    bool practice_mode_fix   = true;
    bool practice_rotation_fix = true;

    // Speed / FPS.
    float native_speed       = 1.0f;
    int   physics_fps        = 60;
    int   display_fps        = 660;
    bool  fps_bypass_enabled = false;

    // Frame stepper.
    bool frame_advance        = false;
    bool backwards_stepping   = false;
    bool universal_frame_advance = false;
    int  hold_start_ms        = 500;
    int  hold_interval_ms     = 30;
    int  store_backsteps      = 1230;

    // Auto clicker.
    bool ac_p1_enabled = false;
    bool ac_p2_enabled = false;
    bool ac_click      = false;
    bool ac_release    = false;
    int  ac_click_every_frames   = 0;
    int  ac_release_after_frames = 0;
    int  ac_player1_id = 888;
    int  ac_player2_id = 889;

    // Pickup macro.
    bool pickup_enabled = false;
    int  pickup_click_id   = 1;
    int  pickup_release_id = 2;
    int  pickup_player1_id = 888;
    int  pickup_player2_id = 889;

    // Pulse trigger loop.
    bool pulse_trigger_loop = false;

    // Menu visual.
    std::array<float, 4> menu_color = {0.06f, 0.06f, 0.06f, 0.95f};
    float menu_size = 0.80f;
    std::string menu_search;

    // Recorder.
    int  recorder_width = 3840;
    int  recorder_height = 2164;
    int  recorder_fps   = 60;
    int  recorder_bitrate_mbps = 150;
    std::string recorder_codec = "h264_nvenc";
    std::string recorder_extra_args;
    std::string recorder_arguments_vf;
    std::string recorder_arguments_audio;
    std::string recorder_format = "mp4";
    std::string recorder_output;
    float recorder_stop_after_endscreen_sec = 0.f;

    // Keybinds (Windows virtual-key codes). 0 means unset.
    int kb_show_menu      = 0xA3;   // VK_RCONTROL - matches screenshot "RCTRL"
    int kb_view_keybinds  = 0x10;   // VK_SHIFT
    int kb_search_controls= 0x70;   // VK_F1
    int kb_toggle_record  = 0;
    int kb_step_forward   = 0;
    int kb_step_backward  = 0;
    int kb_hold_start     = 0;
    // Double-tap-Alt toggle is hard-coded but can be disabled here.
    bool kb_double_alt_toggle_enabled = true;
    int  kb_double_alt_window_ms      = 300;

    // ----- persistence ---------------------------------------------------
    nlohmann::json to_json() const;
    void           from_json(const nlohmann::json& j);

    bool save(const std::string& path) const;
    bool load(const std::string& path);
};

// Process-wide singleton accessor.
Config& get_config();

} // namespace bloom::core
