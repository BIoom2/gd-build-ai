#include <imgui.h>

#include "core/Config.hpp"
#include "menu/Menu.hpp"

namespace bloom::menu::windows {

void draw_utilities(bool* open) {
    auto& cfg = core::get_config();

    if (!ImGui::Begin("UTILITIES", open)) {
        ImGui::End();
        return;
    }

    bool audio_speedhack = false;
    if (search_matches("Audio Speedhack")) {
        ImGui::Checkbox("Audio Speedhack", &audio_speedhack);
    }
    bool noclip1 = false;
    bool noclip2 = false;
    if (search_matches("No-clip Player 1")) ImGui::Checkbox("No-clip Player 1", &noclip1);
    if (search_matches("No-clip Player 2")) ImGui::Checkbox("No-clip Player 2", &noclip2);
    if (search_matches("Show Frame"))       ImGui::Checkbox("Show Frame", &cfg.show_frame);

    ImGui::Separator();
    if (search_matches("Pulse Trigger Loop")) {
        ImGui::Checkbox("Pulse Trigger Loop", &cfg.pulse_trigger_loop);
    }

    ImGui::Separator();
    if (search_matches("Practice Mode Fix")) {
        ImGui::Checkbox("Practice Mode Fix", &cfg.practice_mode_fix);
    }
    if (search_matches("Practice Rotation Fix")) {
        ImGui::Checkbox("Practice Rotation Fix", &cfg.practice_rotation_fix);
    }

    ImGui::Separator();
    if (search_matches("Native Speed")) {
        ImGui::SliderFloat("Native Speed", &cfg.native_speed, 0.1f, 4.0f, "%.3f");
    }
    if (search_matches("Physics FPS")) {
        ImGui::SliderInt("Physics FPS", &cfg.physics_fps, 30, 1000);
    }
    if (search_matches("Display FPS")) {
        ImGui::SliderInt("Display FPS", &cfg.display_fps, 30, 1000);
    }
    if (search_matches("FPS Bypass")) {
        ImGui::Checkbox("FPS Bypass enabled", &cfg.fps_bypass_enabled);
    }

    ImGui::End();
}

} // namespace bloom::menu::windows
