#include "render/ImGuiTheme.hpp"

#include "core/Config.hpp"

#include <imgui.h>

namespace bloom::render {

void apply_bloom_theme() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding    = 4.0f;
    s.WindowPadding     = ImVec2(8, 6);
    s.WindowBorderSize  = 1.0f;
    s.FrameRounding     = 3.0f;
    s.FramePadding      = ImVec2(6, 3);
    s.ItemSpacing       = ImVec2(6, 4);
    s.ItemInnerSpacing  = ImVec2(4, 4);
    s.IndentSpacing     = 14.0f;
    s.ScrollbarSize     = 10.0f;
    s.GrabRounding      = 3.0f;
    s.TabRounding       = 3.0f;
    s.WindowTitleAlign  = ImVec2(0.5f, 0.5f);

    auto* c = s.Colors;
    // Closely matches screenshot 2: dark blue-grey panels, lighter strip
    // title bars, vivid green accents on selected / active controls.
    c[ImGuiCol_Text]                  = ImVec4(0.92f, 0.92f, 0.94f, 1.00f);
    c[ImGuiCol_TextDisabled]          = ImVec4(0.45f, 0.46f, 0.50f, 1.00f);
    c[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.17f, 0.96f);
    c[ImGuiCol_ChildBg]               = ImVec4(0.13f, 0.14f, 0.17f, 0.00f);
    c[ImGuiCol_PopupBg]               = ImVec4(0.11f, 0.12f, 0.15f, 0.98f);
    c[ImGuiCol_Border]                = ImVec4(0.22f, 0.24f, 0.28f, 1.00f);
    c[ImGuiCol_FrameBg]               = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
    c[ImGuiCol_FrameBgHovered]        = ImVec4(0.22f, 0.26f, 0.31f, 1.00f);
    c[ImGuiCol_FrameBgActive]         = ImVec4(0.22f, 0.40f, 0.26f, 1.00f);
    c[ImGuiCol_TitleBg]               = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
    c[ImGuiCol_TitleBgActive]         = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    c[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
    c[ImGuiCol_MenuBarBg]             = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    c[ImGuiCol_CheckMark]             = ImVec4(0.35f, 0.90f, 0.35f, 1.00f);
    c[ImGuiCol_SliderGrab]            = ImVec4(0.35f, 0.90f, 0.35f, 1.00f);
    c[ImGuiCol_SliderGrabActive]      = ImVec4(0.45f, 1.00f, 0.45f, 1.00f);
    c[ImGuiCol_Button]                = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
    c[ImGuiCol_ButtonHovered]         = ImVec4(0.18f, 0.20f, 0.18f, 1.00f);
    c[ImGuiCol_ButtonActive]          = ImVec4(0.22f, 0.35f, 0.22f, 1.00f);
    c[ImGuiCol_Header]                = ImVec4(0.13f, 0.16f, 0.13f, 1.00f);
    c[ImGuiCol_HeaderHovered]         = ImVec4(0.18f, 0.22f, 0.18f, 1.00f);
    c[ImGuiCol_HeaderActive]          = ImVec4(0.22f, 0.32f, 0.22f, 1.00f);
    c[ImGuiCol_Separator]             = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    c[ImGuiCol_SeparatorHovered]      = ImVec4(0.30f, 0.50f, 0.30f, 1.00f);
    c[ImGuiCol_SeparatorActive]       = ImVec4(0.35f, 0.90f, 0.35f, 1.00f);
    c[ImGuiCol_ResizeGrip]            = ImVec4(0.20f, 0.20f, 0.22f, 0.50f);
    c[ImGuiCol_ResizeGripHovered]     = ImVec4(0.30f, 0.50f, 0.30f, 0.75f);
    c[ImGuiCol_ResizeGripActive]      = ImVec4(0.35f, 0.90f, 0.35f, 1.00f);
}

std::array<float, 4> window_bg_color() {
    return core::get_config().menu_color;
}

float window_scale() {
    return core::get_config().menu_size;
}

} // namespace bloom::render
