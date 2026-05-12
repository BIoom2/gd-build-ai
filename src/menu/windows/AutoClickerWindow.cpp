#include <imgui.h>

#include "core/Config.hpp"
#include "menu/Menu.hpp"

namespace bloom::menu::windows {

void draw_auto_clicker(bool* open) {
    auto& cfg = core::get_config();

    if (!ImGui::Begin("AUTO CLICKER", open)) {
        ImGui::End();
        return;
    }

    if (search_matches("Auto Clicker Player 1")) {
        ImGui::Checkbox("Auto Clicker Player 1", &cfg.ac_p1_enabled);
    }
    if (search_matches("Auto Clicker Player 2")) {
        ImGui::Checkbox("Auto Clicker Player 2", &cfg.ac_p2_enabled);
    }
    if (search_matches("Auto Clicker Click")) {
        ImGui::Checkbox("Auto Clicker Click", &cfg.ac_click);
    }
    if (search_matches("Auto Clicker Release")) {
        ImGui::Checkbox("Auto Clicker Release", &cfg.ac_release);
    }

    ImGui::Separator();
    if (search_matches("Click Every Frames")) {
        ImGui::InputInt("Click Every Frames", &cfg.ac_click_every_frames);
    }
    if (search_matches("Release After Frames")) {
        ImGui::InputInt("Release After Frames", &cfg.ac_release_after_frames);
    }

    ImGui::End();
}

} // namespace bloom::menu::windows
