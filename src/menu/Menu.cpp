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

    if (m_windows.tasbot)            windows::draw_tasbot(&m_windows.tasbot);
    if (m_windows.settings)          windows::draw_settings(&m_windows.settings);
    if (m_windows.internal_recorder) windows::draw_internal_recorder(&m_windows.internal_recorder);
    if (m_windows.utilities)         windows::draw_utilities(&m_windows.utilities);
    if (m_windows.auto_clicker)      windows::draw_auto_clicker(&m_windows.auto_clicker);
    if (m_windows.pickup)            windows::draw_pickup(&m_windows.pickup);
    if (m_windows.frame_advance)     windows::draw_frame_advance(&m_windows.frame_advance);

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
