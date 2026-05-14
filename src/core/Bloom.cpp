#include "core/Bloom.hpp"

#include <filesystem>

#include "bot/Bot.hpp"
#include "bot/MacroLibrary.hpp"
#include "core/Config.hpp"
#include "core/Keybinds.hpp"
#include "hooks/FpsBypass.hpp"
#include "hooks/FrameStepper.hpp"
#include "hooks/HookManager.hpp"
#include "hooks/PlayLayerHook.hpp"
#include "hooks/PlayerObjectHook.hpp"
#include "hooks/PulseTriggerHook.hpp"
#include "menu/Menu.hpp"
#include "render/D3D9Hook.hpp"
#include "util/Logger.hpp"

namespace bloom::core {

namespace fs = std::filesystem;

Bloom& Bloom::instance() {
    static Bloom b;
    return b;
}

void Bloom::start(const std::string& dll_dir) {
    if (m_started) return;
    m_started = true;
    m_dll_dir = dll_dir;

    auto log_path     = (fs::path(dll_dir) / "bloom.log").string();
    auto cfg_path     = (fs::path(dll_dir) / "bloom_settings.json").string();
    auto macros_dir   = (fs::path(dll_dir) / "bloom_macros").string();

    util::Logger::instance().init(log_path);
    BLOOM_LOG_INFO("Bloom v%s starting", BLOOM_VERSION_STRING);
    BLOOM_LOG_INFO("DLL directory: %s", dll_dir.c_str());

    get_config().load(cfg_path);
    bot::get_macro_library().init(macros_dir);

    // Initialise hook manager early so all subsequent install_* calls see it.
    (void)hooks::HookManager::instance();

    hooks::install_play_layer_hooks();
    hooks::install_player_object_hooks();
    hooks::install_fps_bypass_hooks();
    hooks::install_pulse_trigger_hooks();

    // Frame stepper key bindings dispatch via Keybinds.
    auto& kb = get_keybinds();
    kb.set_toggle_menu([]{ menu::Menu::instance().toggle_visible(); });
    kb.set_step_forward([]{ hooks::request_step_forward(1); });
    kb.set_step_backward([]{ hooks::request_step_backward(1); });
    kb.set_hold_start([]{ hooks::request_hold_start(); });
    kb.set_toggle_record([]{
        auto& bot_inst = bot::Bot::instance();
        bot_inst.set_mode(bot_inst.mode() == bot::BotMode::Recording
                          ? bot::BotMode::Idle
                          : bot::BotMode::Recording);
    });

    // Render hook installs ImGui once GD's d3d9 device shows up.
    render::install_d3d9_hooks([this]{
        get_keybinds().poll();
        hooks::tick_auto_hold();
        menu::Menu::instance().draw();
    });

    BLOOM_LOG_INFO("Bloom started. Hooks installed: %zu",
                   hooks::HookManager::instance().installed_count());
}

void Bloom::stop() {
    if (!m_started) return;
    m_started = false;

    BLOOM_LOG_INFO("Bloom stopping");

    auto cfg_path = (fs::path(m_dll_dir) / "bloom_settings.json").string();
    get_config().save(cfg_path);

    render::uninstall_d3d9_hooks();
    util::Logger::instance().shutdown();
}

} // namespace bloom::core
