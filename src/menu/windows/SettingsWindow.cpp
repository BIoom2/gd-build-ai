#include <cstring>

#include <imgui.h>

#include "core/Config.hpp"
#include "menu/Menu.hpp"

namespace bloom::menu::windows {

namespace {

char g_search_buf[256] = {};

} // namespace

void draw_settings(bool* open) {
    auto& cfg = core::get_config();

    if (!ImGui::Begin("SETTINGS", open)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Version: v%s (up-to-date)", BLOOM_VERSION_STRING);
    ImGui::TextDisabled("Status: Regular");
    ImGui::TextDisabled("4F15E94B-E17F-476E-EBB25B3E"); // visual ID like screenshot
    if (ImGui::Button("Copy")) {
        ImGui::SetClipboardText("Bloom v" BLOOM_VERSION_STRING);
    }
    ImGui::Separator();

    if (search_matches("Show")) {
        ImGui::Text("Show         [ RCTRL ]");
    }
    if (search_matches("View Keybinds")) {
        ImGui::Text("View Keybinds [ SHIFT ]");
    }
    if (search_matches("Search Controls")) {
        ImGui::Text("Search Controls [ F1 ]");
    }
    ImGui::Separator();

    if (search_matches("Enable Bot")) {
        ImGui::Checkbox("Enable Bot", &cfg.enable_bot);
    }
    if (search_matches("Enable Lua Plugin")) {
        ImGui::Checkbox("Enable Lua Plugin", &cfg.enable_lua_plugin);
    }
    if (search_matches("Advanced Settings")) {
        ImGui::Checkbox("Advanced Settings", &cfg.advanced_settings);
    }
    if (search_matches("Set Keybinds")) {
        ImGui::Checkbox("Set Keybinds", &cfg.advanced_settings); // reuse flag as placeholder
    }
    if (search_matches("Dual FPS Mode")) {
        ImGui::Checkbox("Dual FPS Mode", &cfg.dual_fps_mode);
    }
    if (search_matches("Don't Overwrite Macro")) {
        ImGui::Checkbox("Don't Overwrite Macro", &cfg.dont_overwrite_macro);
    }
    if (search_matches("Ignore Inputs on Replay")) {
        ImGui::Checkbox("Ignore Inputs on Replay", &cfg.ignore_inputs_on_replay);
    }
    if (search_matches("Clean Macro")) {
        if (ImGui::Button("Clean Macro")) {
            // implemented by Bot::clear via macro_library; here just a stub.
        }
    }
    if (search_matches("Reset Layout")) {
        if (ImGui::Button("Reset Layout")) {
            ImGui::GetIO().IniFilename = nullptr;
        }
    }

    ImGui::Separator();
    if (search_matches("Menu Colour")) {
        ImGui::ColorEdit4("Menu Colour", cfg.menu_color.data(),
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    }
    if (search_matches("Size")) {
        ImGui::SliderFloat("Size", &cfg.menu_size, 0.5f, 2.0f, "%.2f");
    }

    if (search_matches("Search")) {
        ImGui::Spacing();
        ImGui::Text("Search");
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::InputText("##bloom_search", g_search_buf, sizeof(g_search_buf))) {
            cfg.menu_search = g_search_buf;
        }
    }

    ImGui::End();
}

} // namespace bloom::menu::windows
