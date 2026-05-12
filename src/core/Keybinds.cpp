#include "core/Keybinds.hpp"

#include "core/Config.hpp"
#include "util/Time.hpp"

#include <windows.h>

namespace bloom::core {

namespace {

bool sample_vk(int vk) {
    if (vk == 0) return false;
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

} // namespace

void Keybinds::poll() {
    auto& cfg = get_config();

    for (int vk = 1; vk < 256; ++vk) {
        m_prev[vk] = m_now[vk];
        m_now[vk]  = sample_vk(vk);
    }

    // ----- double-Alt toggle (matches user request "бинд меню на две кнопки alt") ----
    // Detect press edge on either Alt key.
    bool alt_down_now = m_now[VK_LMENU] || m_now[VK_RMENU] || m_now[VK_MENU];
    bool alt_down_prev = m_prev[VK_LMENU] || m_prev[VK_RMENU] || m_prev[VK_MENU];
    if (cfg.kb_double_alt_toggle_enabled) {
        if (alt_down_now && !alt_down_prev) {
            // Press edge.
            std::int64_t now = util::now_ms();
            if (m_last_alt_release_ms != 0 &&
                (now - m_last_alt_release_ms) <= cfg.kb_double_alt_window_ms) {
                if (m_toggle_menu) m_toggle_menu();
                m_last_alt_release_ms = 0;
            } else {
                // First tap - remember press time as the candidate window start.
                // We reset on release below; using release time as the gating
                // boundary keeps "hold alt to skip the toggle" working naturally.
            }
        } else if (!alt_down_now && alt_down_prev) {
            // Release edge.
            m_last_alt_release_ms = util::now_ms();
        }
    }
    m_alt_was_down = alt_down_now;

    // ----- single-key bindings ------------------------------------------
    auto trigger = [&](int vk, const Callback& cb) {
        if (vk == 0 || !cb) return;
        if (m_now[vk] && !m_prev[vk]) cb();
    };

    trigger(cfg.kb_toggle_record, m_toggle_record);
    trigger(cfg.kb_step_forward,  m_step_forward);
    trigger(cfg.kb_step_backward, m_step_backward);
    trigger(cfg.kb_hold_start,    m_hold_start);
    // kb_show_menu shows the menu (toggle visible). Mapped to RCTRL by default.
    trigger(cfg.kb_show_menu,     m_toggle_menu);
}

bool Keybinds::just_pressed(int vk) const {
    if (vk <= 0 || vk >= 256) return false;
    return m_now[vk] && !m_prev[vk];
}

Keybinds& get_keybinds() {
    static Keybinds kb;
    return kb;
}

} // namespace bloom::core
