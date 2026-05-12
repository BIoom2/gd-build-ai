#pragma once

#include <array>
#include <string>

namespace bloom::menu {

// Per-window visibility flags. Matches screenshot 1.
struct WindowsOpen {
    bool tasbot           = true;
    bool settings         = true;
    bool internal_recorder= true;
    bool utilities        = true;
    bool auto_clicker     = true;
    bool pickup           = true;
    bool frame_advance    = true;
};

class Menu {
public:
    static Menu& instance();

    void draw();
    void toggle_visible() { m_visible = !m_visible; }
    bool visible() const  { return m_visible; }

    WindowsOpen& windows() { return m_windows; }

private:
    bool m_visible = true;
    WindowsOpen m_windows;
};

// Helper used by each window to decide whether to skip an item under the
// global "Search" filter. Pass the human-readable label - returns true if the
// item should be rendered.
bool search_matches(const std::string& label);

} // namespace bloom::menu
