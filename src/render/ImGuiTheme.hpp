#pragma once

#include <array>

namespace bloom::render {

// Applies the dark-with-green-accent theme matching screenshot 1. Call once
// after ImGui::CreateContext().
void apply_bloom_theme();

// Returns the colour used for window backgrounds (driven by Config.menu_color).
std::array<float, 4> window_bg_color();

// Returns the master scale derived from Config.menu_size.
float window_scale();

} // namespace bloom::render
