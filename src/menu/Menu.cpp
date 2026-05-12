#include "menu/Menu.hpp"

#include <algorithm>
#include <cctype>

#include <imgui.h>

#include "core/Config.hpp"
#include "render/ImGuiTheme.hpp"
#include "render/WndProcHook.hpp"

namespace bloom::menu {

namespace windows {
    void draw_tasbot(bool* open);
    void draw_settings(bool* open);
    void draw_internal_recorder(bool* open);
    void draw_utilities(bool* open);
    void draw_auto_clicker(bool* open);
    void draw_pickup(bool* open);
    void draw_frame_advance(bool* open);
}

Menu& Menu::instance() {
    static Menu m;
    return m;
}

namespace {

// Tile the 7 windows horizontally along the top of the screen the first time
// the user sees the menu. Pixels are screen-space ImGui coordinates.
constexpr float k_window_width = 220.0f;
constexpr float k_gutter       = 6.0f;
constexpr float k_y_offset     = 50.0f;

void set_initial_pos(int index) {
    float x = 20.0f + index * (k_window_width + k_gutter);
    ImGui::SetNextWindowPos(ImVec2(x, k_y_offset), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(k_window_width, 0.0f), ImGuiCond_FirstUseEver);
}

} // namespace

void Menu::draw() {
    // Toggle ImGui input capture so the game doesn't see clicks while the menu
    // is interactive.
    render::set_block_game_input(m_visible);

    if (!m_visible) return;

    auto& cfg = core::get_config();
    auto bg = cfg.menu_color;
    ImGui::PushStyleColor(ImGuiCol_WindowBg,
                          ImVec4(bg[0], bg[1], bg[2], bg[3]));
    ImGui::GetIO().FontGlobalScale = std::clamp(cfg.menu_size, 0.5f, 2.0f);

    // Small "Bloom" header bar on top so the user can always see Bloom is
    // running even if every other window is collapsed.
    ImGui::SetNextWindowPos(ImVec2(20.0f, 4.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.85f);
    if (ImGui::Begin("##BloomHeader", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::TextColored(ImVec4(0.35f, 0.90f, 0.35f, 1.00f),
                           "Bloom v%s", BLOOM_VERSION_STRING);
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        ImGui::Checkbox("TASBOT", &m_windows.tasbot);             ImGui::SameLine();
        ImGui::Checkbox("SETTINGS", &m_windows.settings);         ImGui::SameLine();
        ImGui::Checkbox("INTERNAL RECORDER", &m_windows.internal_recorder); ImGui::SameLine();
        ImGui::Checkbox("UTILITIES", &m_windows.utilities);       ImGui::SameLine();
        ImGui::Checkbox("AUTO CLICKER", &m_windows.auto_clicker); ImGui::SameLine();
        ImGui::Checkbox("PICKUP", &m_windows.pickup);             ImGui::SameLine();
        ImGui::Checkbox("FRAME ADVANCE", &m_windows.frame_advance);
    }
    ImGui::End();

    int slot = 0;
    if (m_windows.tasbot)            { set_initial_pos(slot++); windows::draw_tasbot(&m_windows.tasbot); }
    if (m_windows.settings)          { set_initial_pos(slot++); windows::draw_settings(&m_windows.settings); }
    if (m_windows.internal_recorder) { set_initial_pos(slot++); windows::draw_internal_recorder(&m_windows.internal_recorder); }
    if (m_windows.utilities)         { set_initial_pos(slot++); windows::draw_utilities(&m_windows.utilities); }
    if (m_windows.auto_clicker)      { set_initial_pos(slot++); windows::draw_auto_clicker(&m_windows.auto_clicker); }
    if (m_windows.pickup)            { set_initial_pos(slot++); windows::draw_pickup(&m_windows.pickup); }
    if (m_windows.frame_advance)     { set_initial_pos(slot++); windows::draw_frame_advance(&m_windows.frame_advance); }

    ImGui::PopStyleColor();
}

bool search_matches(const std::string& label) {
    auto& q = core::get_config().menu_search;
    if (q.empty()) return true;

    auto to_lower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    };
    return to_lower(label).find(to_lower(q)) != std::string::npos;
}

} // namespace bloom::menu
