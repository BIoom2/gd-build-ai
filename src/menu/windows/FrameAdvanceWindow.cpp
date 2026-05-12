#include <imgui.h>

#include "core/Config.hpp"
#include "hooks/FrameStepper.hpp"
#include "menu/Menu.hpp"

namespace bloom::menu::windows {

void draw_frame_advance(bool* open) {
    auto& cfg = core::get_config();

    if (!ImGui::Begin("FRAME ADVANCE", open, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
        return;
    }

    if (search_matches("Frame Advance")) {
        ImGui::Checkbox("Frame Advance", &cfg.frame_advance);
    }
    if (search_matches("Backwards Stepping")) {
        ImGui::Checkbox("Backwards Stepping", &cfg.backwards_stepping);
    }
    if (search_matches("Universal Frame Advance")) {
        ImGui::Checkbox("Universal Frame Advance", &cfg.universal_frame_advance);
    }

    ImGui::Separator();
    if (search_matches("Step Forward")) {
        if (ImGui::Button("Step Forward", ImVec2(-FLT_MIN, 0))) {
            hooks::request_step_forward(1);
        }
    }
    if (search_matches("Step Backwards")) {
        if (ImGui::Button("Step Backwards", ImVec2(-FLT_MIN, 0))) {
            hooks::request_step_backward(1);
        }
    }

    ImGui::Separator();
    if (search_matches("Hold Start")) {
        ImGui::SliderInt("Hold Start ms", &cfg.hold_start_ms, 0, 5000);
    }
    if (search_matches("Hold Interval")) {
        ImGui::SliderInt("Hold Interval ms", &cfg.hold_interval_ms, 1, 1000);
    }
    if (search_matches("Store Backsteps")) {
        ImGui::SliderInt("Store N Backsteps", &cfg.store_backsteps, 0, 10000);
    }

    ImGui::End();
}

} // namespace bloom::menu::windows
