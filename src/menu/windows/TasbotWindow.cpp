#include <algorithm>
#include <cstring>
#include <string>

#include <imgui.h>

#include "bot/Bot.hpp"
#include "bot/MacroLibrary.hpp"
#include "core/Config.hpp"
#include "menu/Menu.hpp"
#include "util/Logger.hpp"

namespace bloom::menu::windows {

namespace {

char g_macro_search_buf[128] = {};
char g_macro_name_buf[128]   = {};

void render_macro_combo_with_search(core::Config& cfg) {
    auto& lib = bot::get_macro_library();

    // Sync the input buf with cfg the first time around.
    if (g_macro_name_buf[0] == '\0' && !cfg.current_macro_name.empty()) {
        std::strncpy(g_macro_name_buf, cfg.current_macro_name.c_str(),
                     sizeof(g_macro_name_buf) - 1);
    }

    ImGui::TextUnformatted("Macro name");
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputText("##macro_name", g_macro_name_buf, sizeof(g_macro_name_buf))) {
        cfg.current_macro_name = g_macro_name_buf;
    }

    if (ImGui::BeginCombo("##macro_combo", "", ImGuiComboFlags_NoPreview | ImGuiComboFlags_HeightLarge)) {
        // Search box like screenshot 3.
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputTextWithHint("##macro_search", "Search...",
                                 g_macro_search_buf, sizeof(g_macro_search_buf));
        ImGui::Separator();

        auto filtered = lib.filtered(g_macro_search_buf);
        for (const auto& name : filtered) {
            bool selected = (name == cfg.current_macro_name);
            if (ImGui::Selectable(name.c_str(), selected)) {
                cfg.current_macro_name = name;
                std::strncpy(g_macro_name_buf, name.c_str(),
                             sizeof(g_macro_name_buf) - 1);
            }
        }
        if (filtered.empty()) {
            ImGui::TextDisabled("(no macros)");
        }
        ImGui::EndCombo();
    }
}

} // namespace

void draw_tasbot(bool* open) {
    auto& cfg = core::get_config();
    auto& bot_inst = bot::Bot::instance();
    auto& lib = bot::get_macro_library();

    if (!ImGui::Begin("TASBOT", open)) {
        ImGui::End();
        return;
    }

    if (search_matches("Recording Mode")) {
        bool rec = (bot_inst.mode() == bot::BotMode::Recording);
        if (ImGui::RadioButton("Recording Mode", rec)) {
            bot_inst.set_mode(bot::BotMode::Recording);
            cfg.recording_mode = 0;
        }
    }
    if (search_matches("Playback Mode")) {
        bool pb = (bot_inst.mode() == bot::BotMode::Playback);
        if (ImGui::RadioButton("Playback Mode", pb)) {
            bot_inst.set_mode(bot::BotMode::Playback);
            cfg.recording_mode = 1;
        }
    }
    ImGui::Spacing();

    render_macro_combo_with_search(cfg);
    ImGui::Spacing();

    if (search_matches("Save")) {
        if (ImGui::Button("Save", ImVec2(-FLT_MIN, 0))) {
            if (!cfg.current_macro_name.empty()) {
                if (cfg.dont_overwrite_macro && lib.exists(cfg.current_macro_name)) {
                    BLOOM_LOG_WARN("Refusing to overwrite existing macro '%s'",
                                   cfg.current_macro_name.c_str());
                } else {
                    auto& macro = bot_inst.current_macro();
                    macro.name = cfg.current_macro_name;
                    if (lib.save(macro)) {
                        BLOOM_LOG_INFO("Saved macro '%s' (%zu frames)",
                                       macro.name.c_str(), macro.frames.size());
                        lib.refresh();
                    }
                }
            }
        }
    }
    if (search_matches("Load")) {
        if (ImGui::Button("Load", ImVec2(-FLT_MIN, 0))) {
            if (auto loaded = lib.load(cfg.current_macro_name)) {
                bot_inst.current_macro() = std::move(*loaded);
                BLOOM_LOG_INFO("Loaded macro '%s' (%zu frames)",
                               cfg.current_macro_name.c_str(),
                               bot_inst.current_macro().frames.size());
            } else {
                BLOOM_LOG_WARN("Macro '%s' not found", cfg.current_macro_name.c_str());
            }
        }
    }

    ImGui::End();
}

} // namespace bloom::menu::windows
