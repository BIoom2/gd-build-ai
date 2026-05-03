// Particle-quality hooks: every emitter that initialises through
// CCParticleSystem::initWithTotalParticles gets its budget multiplied
// by the user's particle-multiplier setting. The "uncap" toggle lifts
// the soft 16k upper bound so dense rocket-trail / explosion plists can
// actually use the full budget.
//
// Why hook initWithTotalParticles instead of setTotalParticles:
//   * initWithTotalParticles is what allocates the m_pParticles /
//     m_pQuads / m_pIndices arrays. setTotalParticles does NOT
//     reallocate them in cocos2d-x — it only clamps the active count.
//     So multiplying inside setTotalParticles would just be ignored
//     (the new value would exceed m_uAllocatedParticles).

#include <Geode/Geode.hpp>
#include <Geode/modify/CCParticleSystem.hpp>

#include "Settings.hpp"

#include <algorithm>

using namespace geode::prelude;
using maxq::Settings;

namespace {

// Hard ceiling so a runaway level/multiplier combo can't request a
// gigabyte-long allocation.
constexpr unsigned int kAbsoluteMaxParticles    = 65536;
// Soft cap matching cocos2d's typical "you probably don't want more
// than this" advisory, lifted only when uncapParticles is on.
constexpr unsigned int kCappedMaxParticles      = 16384;

unsigned int computeBoostedCount(unsigned int original) {
    auto& s = Settings::get();

    if (!s.enabled.load(std::memory_order_relaxed)) return original;
    if (original == 0) return 0;

    float mul = s.particleMultiplier.load(std::memory_order_relaxed);
    bool uncap = s.uncapParticles.load(std::memory_order_relaxed);

    // Round to nearest, clamped to >= 1 so "0.5x" of a 1-particle
    // emitter still leaves at least one particle alive.
    auto boosted = static_cast<unsigned int>(
        std::max(1.0f, std::round(static_cast<float>(original) * mul)));

    unsigned int ceiling = uncap
        ? kAbsoluteMaxParticles
        : kCappedMaxParticles;

    return std::min(boosted, ceiling);
}

} // namespace

class $modify(MaxQualityParticles, CCParticleSystem) {
    bool initWithTotalParticles(unsigned int numberOfParticles, bool unk) {
        unsigned int boosted = computeBoostedCount(numberOfParticles);

        if (Settings::get().verboseLog.load(std::memory_order_relaxed)
            && boosted != numberOfParticles) {
            log::debug(
                "maxq: particle init {} -> {}",
                numberOfParticles, boosted);
        }

        return CCParticleSystem::initWithTotalParticles(boosted, unk);
    }
};
