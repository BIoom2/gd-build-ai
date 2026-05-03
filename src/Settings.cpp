#include "Settings.hpp"

using namespace geode::prelude;

namespace maxq {

Settings& Settings::get() {
    static Settings s;
    return s;
}

void Settings::refreshFromMod() {
    auto* mod = Mod::get();

    enabled.store(
        mod->getSettingValue<bool>("enabled"),
        std::memory_order_relaxed);

    forceUhd.store(
        mod->getSettingValue<bool>("force-uhd"),
        std::memory_order_relaxed);

    anisotropic.store(
        std::clamp<int>(mod->getSettingValue<int64_t>("anisotropic"), 1, 16),
        std::memory_order_relaxed);

    {
        auto const v = mod->getSettingValue<std::string>("texture-filter");
        int mode = kFilterTrilinear;
        if      (v == "nearest")   mode = kFilterNearest;
        else if (v == "bilinear")  mode = kFilterBilinear;
        else if (v == "trilinear") mode = kFilterTrilinear;
        filterMode.store(mode, std::memory_order_relaxed);
    }

    particleMultiplier.store(
        std::clamp<float>(
            static_cast<float>(mod->getSettingValue<double>("particle-multiplier")),
            0.5f, 8.0f),
        std::memory_order_relaxed);

    uncapParticles.store(
        mod->getSettingValue<bool>("uncap-particles"),
        std::memory_order_relaxed);

    verboseLog.store(
        mod->getSettingValue<bool>("verbose-log"),
        std::memory_order_relaxed);
}

} // namespace maxq
