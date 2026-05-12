#include <cstring>

#include <imgui.h>

#include "core/Config.hpp"
#include "menu/Menu.hpp"

namespace bloom::menu::windows {

namespace {

bool g_recording = false;

template <std::size_t N>
void string_input(const char* label, std::string& target, char (&buf)[N]) {
    if (buf[0] == '\0' && !target.empty()) {
        std::strncpy(buf, target.c_str(), N - 1);
    }
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputText(label, buf, N)) {
        target = buf;
    }
}

char g_codec_buf[64]   = {};
char g_extra_buf[256]  = {};
char g_vf_buf[256]     = {};
char g_audio_buf[256]  = {};
char g_format_buf[16]  = {};
char g_output_buf[256] = {};

} // namespace

void draw_internal_recorder(bool* open) {
    auto& cfg = core::get_config();

    if (!ImGui::Begin("INTERNAL RECORDER", open)) {
        ImGui::End();
        return;
    }

    if (search_matches("Start Recording")) {
        if (ImGui::Button(g_recording ? "Stop Recording" : "Start Recording",
                          ImVec2(-FLT_MIN, 0))) {
            g_recording = !g_recording;
            // Real FFmpeg pipe TBD - this surfaces the UI 1:1 with screenshot.
        }
    }

    if (search_matches("Resolution")) {
        ImGui::PushItemWidth(80);
        ImGui::InputInt("##w", &cfg.recorder_width, 0);
        ImGui::SameLine(); ImGui::TextUnformatted("X");
        ImGui::SameLine(); ImGui::InputInt("##h", &cfg.recorder_height, 0);
        ImGui::SameLine(); ImGui::TextUnformatted("/");
        ImGui::SameLine(); ImGui::InputInt("##fps", &cfg.recorder_fps, 0);
        ImGui::PopItemWidth();
    }

    if (search_matches("Bitrate")) {
        ImGui::SliderInt("Bitrate (MBPS)", &cfg.recorder_bitrate_mbps, 1, 300);
    }

    if (search_matches("Stop after endscreen")) {
        ImGui::SliderFloat("Stop after endscreen (s)",
                           &cfg.recorder_stop_after_endscreen_sec, 0.f, 30.f, "%.2fs");
    }

    if (search_matches("Codec")) {
        string_input("Codec", cfg.recorder_codec, g_codec_buf);
    }
    if (search_matches("Extra Arguments")) {
        string_input("Extra Arguments", cfg.recorder_extra_args, g_extra_buf);
    }
    if (search_matches("Arguments vf")) {
        string_input("Arguments vf", cfg.recorder_arguments_vf, g_vf_buf);
    }
    if (search_matches("Arguments Audio")) {
        string_input("Arguments Audio", cfg.recorder_arguments_audio, g_audio_buf);
    }
    if (search_matches("Format")) {
        string_input("Format", cfg.recorder_format, g_format_buf);
    }
    if (search_matches("Output")) {
        string_input("Output", cfg.recorder_output, g_output_buf);
    }

    ImGui::End();
}

} // namespace bloom::menu::windows
