#include <imgui.h>

#include "core/Config.hpp"
#include "menu/Menu.hpp"

namespace bloom::menu::windows {

void draw_pickup(bool* open) {
    auto& cfg = core::get_config();

    if (!ImGui::Begin("PICKUP", open, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        return;
    }

    if (search_matches("Toggle Pickup Macro")) {
        ImGui::Checkbox("Toggle Pickup Macro", &cfg.pickup_enabled);
    }

    if (search_matches("Click ID")) {
        ImGui::InputInt("Click ID",   &cfg.pickup_click_id);
    }
    if (search_matches("Release ID")) {
        ImGui::InputInt("Release ID", &cfg.pickup_release_id);
    }
    if (search_matches("Player 1 ID")) {
        ImGui::InputInt("Player 1 ID", &cfg.pickup_player1_id);
    }
    if (search_matches("Player 2 ID")) {
        ImGui::InputInt("Player 2 ID", &cfg.pickup_player2_id);
    }

    ImGui::End();
}

} // namespace bloom::menu::windows
