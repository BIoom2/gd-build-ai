// Max Quality — entry point. Wires the mod's settings cache into the
// hook layer and re-tunes already-loaded textures whenever the user
// flips a setting at runtime, so every change is visible without a
// game restart.

#include <Geode/Geode.hpp>

#include "Settings.hpp"

using namespace geode::prelude;
using maxq::Settings;

namespace maxq {
    // Defined in TextureQualityHooks.cpp.
    void retuneAllTextures();
}

namespace {

void onAnySettingChanged() {
    Settings::get().refreshFromMod();
    maxq::retuneAllTextures();

    auto const& s = Settings::get();
    log::info(
        "maxq: settings refreshed — enabled={} forceUhd={} filter={} "
        "aniso={} particleMul={:.2f} uncap={}",
        s.enabled.load(),
        s.forceUhd.load(),
        s.filterMode.load(),
        s.anisotropic.load(),
        s.particleMultiplier.load(),
        s.uncapParticles.load());
}

} // namespace

$on_mod(Loaded) {
    Settings::get().refreshFromMod();

    listenForSettingChanges<bool>(
        "enabled",            +[](bool)        { onAnySettingChanged(); });
    listenForSettingChanges<bool>(
        "force-uhd",          +[](bool)        { onAnySettingChanged(); });
    listenForSettingChanges<std::string>(
        "texture-filter",     +[](std::string) { onAnySettingChanged(); });
    listenForSettingChanges<int64_t>(
        "anisotropic",        +[](int64_t)     { onAnySettingChanged(); });
    listenForSettingChanges<double>(
        "particle-multiplier",+[](double)      { onAnySettingChanged(); });
    listenForSettingChanges<bool>(
        "uncap-particles",    +[](bool)        { onAnySettingChanged(); });
    listenForSettingChanges<bool>(
        "verbose-log",        +[](bool)        { onAnySettingChanged(); });

    log::info("maxq: loaded v{}", Mod::get()->getVersion().toVString());
}
