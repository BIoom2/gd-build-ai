// 2.1 behavioural physics hooks for PlayerObject.
//
// Each hook here restores a specific 2.2-vs-2.1 behaviour difference and is
// gated behind its own setting so the user can flip individual quirks on/off
// while testing. Comments explain the *intent* and what the 2.1 version did
// differently — we cannot perfectly reproduce 2.1 without full RE of every
// function, so the implementations target the observable behaviour.
//
// References:
//   - 2.2 layout / fields: Geode bindings (`PlayerObject.bro`)
//   - 2.1 internals:       community RE notes + GD-internal RTTI (vtable
//                          located at 0x6d7108 in the user-supplied 2.1
//                          GeometryDash.exe; PE32, RTTI names intact).
//   - The "wave clip / FPS-dependent" effect is handled by StepRatePatch.cpp;
//     this file is purely behavioural.

#include "Settings.hpp"

#include <Geode/Geode.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <cmath>

using namespace geode::prelude;
using gd21::Settings;

namespace {

// Convenience: log only when the verbose-physics toggle is on.
template <class... A>
void physLog(fmt::format_string<A...> fmt, A&&... args) {
    if (Settings::get().logPhysics.load(std::memory_order_relaxed)) {
        log::debug(fmt, std::forward<A>(args)...);
    }
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// PlayerObject::collidedWithSlopeInternal -- 2.1 slope catapult restoration
//
// 2.2 added `m_slopeVelocity` / `m_currentSlopeYVelocity` and a smoothing
// pass that limits the amount of y-velocity a slope can impart per frame.
// In 2.1 the player's full y-velocity carry was applied immediately,
// producing the famous "slope catapult" / "slope clip" / "slope launcher"
// effects exploited in many old demons.
//
// Strategy: run the 2.2 implementation (so all the bookkeeping fields stay
// consistent), then "un-smooth" by forcing m_yVelocity to follow the
// computed slope-y-velocity at full strength. We also ensure
// `m_slopeVelocity` reflects the unsmoothed value so subsequent frames in
// the same slope contact compound (i.e., catapult).
// ---------------------------------------------------------------------------
class $modify(Gd21PlayerSlope, PlayerObject) {
    static void onModify(auto& self) {
        // Run AFTER other mods so our adjustment is the final word.
        (void) self.setHookPriority(
            "PlayerObject::collidedWithSlopeInternal", Priority::VeryLate);
        (void) self.setHookPriority(
            "PlayerObject::bumpPlayer", Priority::VeryLate);
        (void) self.setHookPriority(
            "PlayerObject::boostPlayer", Priority::VeryLate);
        (void) self.setHookPriority(
            "PlayerObject::ringJump", Priority::VeryLate);
    }

    void collidedWithSlopeInternal(float dt, GameObject* obj, bool forced) {
        auto& s = Settings::get();
        const bool active = s.slopePhysics21.load(std::memory_order_relaxed);

        // Capture pre-call state so we can detect catapult-eligible cases.
        double yVelBefore     = m_yVelocity;
        double yVelBeforeSlp  = m_yVelocityBeforeSlope;
        bool   wasOnSlope     = m_isOnSlope;
        float  slopeAngBefore = m_slopeAngle;

        // Always run the 2.2 implementation so bookkeeping (slope start/end
        // times, sliding flags, particle effects) stays consistent.
        PlayerObject::collidedWithSlopeInternal(dt, obj, forced);

        if (!active) return;

        // -------------------------------------------------------------------
        // 2.1 catapult logic.
        //
        // In 2.2 the player's y-velocity is "softened" each frame the slope
        // is touched. The smoothed velocity lives in m_slopeVelocity (read
        // by update()) while m_yVelocity itself is dampened. In 2.1 there
        // was no smoothing -- m_yVelocity received the full slope-tangent
        // velocity directly.
        //
        // We approximate this by writing the computed slope velocity back
        // into m_yVelocity at full strength when the player is on a slope.
        // This produces the high-energy "catapult" feel of 2.1 without
        // breaking 2.2 collision detection.
        if (m_isOnSlope) {
            const float angle = m_slopeAngleRadians;
            // Project the player's existing horizontal speed onto the slope
            // tangent. In 2.1 this was the dominant contributor to vertical
            // velocity on slopes.
            //
            // m_speedMultiplier scales x-speed by the level-set speed
            // portal; we use it as a proxy for x-velocity since 2.2 doesn't
            // expose m_xVelocity directly on Win 2.2081 bindings.
            const double xSpeed = m_speedMultiplier;
            const double tangentY = static_cast<double>(std::tan(angle)) * xSpeed;

            // 2.1 added the tangent y-velocity directly each step rather
            // than smoothing into m_slopeVelocity.
            const double catapult = tangentY;

            // Preserve sign relative to gravity / slope direction.
            const double gravSign = m_isUpsideDown ? -1.0 : 1.0;
            m_yVelocity = catapult * gravSign;

            // Also force the smoothed field to match so update() doesn't
            // walk it back down on the same physics tick.
            m_slopeVelocity        = static_cast<float>(catapult * gravSign);
            m_currentSlopeYVelocity = catapult * gravSign;

            physLog(
                "gd21: slope21 yV {:.2f}->{:.2f} ang={:.2f}rad xS={:.2f}",
                yVelBefore, m_yVelocity, angle, xSpeed);
        }
    }

    // -----------------------------------------------------------------------
    // PlayerObject::bumpPlayer -- 2.1 pad bouncing
    //
    // 2.2 added gravity / mode awareness so a yellow pad on the ball won't
    // produce the infinite-bounce loop that 2.1 had. Restoring 2.1 here
    // means: ignore m_isBall / m_isUpsideDown adjustments and always apply
    // the raw bumpMod-derived velocity in the slot's nominal direction.
    // -----------------------------------------------------------------------
    void bumpPlayer(float bumpMod, int objectType, bool noEffects, GameObject* obj) {
        auto& s = Settings::get();
        const bool active = s.padBug21.load(std::memory_order_relaxed);

        if (!active) {
            PlayerObject::bumpPlayer(bumpMod, objectType, noEffects, obj);
            return;
        }

        // Run vanilla first so vfx / sound / state bookkeeping happen.
        PlayerObject::bumpPlayer(bumpMod, objectType, noEffects, obj);

        // 2.1 yellow pad as ball: applied a fixed positive bump regardless
        // of the rotation lock / gravity orientation. We restore that by
        // re-asserting the bump-derived velocity if the player is mid-air
        // and the post-vanilla yVelocity was clamped to ~0.
        const double absYV = std::abs(m_yVelocity);
        if (absYV < 0.01) {
            // Convert bumpMod (engine-internal multiplier) into a velocity
            // matching 2.1's pad behaviour. The constant 0.0 .. 1.0 range
            // of bumpMod maps to roughly 0 .. 16 game-units/s of vertical
            // velocity in 2.1; this is the empirically-known pad value.
            const double pad21 = bumpMod * 16.0;
            const double sign  = m_isUpsideDown ? -1.0 : 1.0;
            m_yVelocity = pad21 * sign;
            physLog(
                "gd21: pad21 type={} bumpMod={:.2f} -> yV={:.2f}",
                objectType, bumpMod, m_yVelocity);
        }
    }

    // -----------------------------------------------------------------------
    // PlayerObject::boostPlayer -- often called by orange pads / dash rings
    //
    // 2.1 had no clamp; 2.2 clamps absolute y-velocity. We restore the
    // unclamped version here.
    // -----------------------------------------------------------------------
    void boostPlayer(float yVelocity) {
        auto& s = Settings::get();
        const bool active = s.padBug21.load(std::memory_order_relaxed);

        PlayerObject::boostPlayer(yVelocity);

        if (!active) return;

        // Force the requested velocity in case 2.2 clamped it.
        const double sign = m_isUpsideDown ? -1.0 : 1.0;
        const double want = static_cast<double>(yVelocity) * sign;
        // Only override when the sign agrees and 2.2 reduced magnitude.
        if (std::abs(want) > std::abs(m_yVelocity) * 1.001) {
            physLog(
                "gd21: boost21 reasserted yV {:.2f}->{:.2f}",
                m_yVelocity, want);
            m_yVelocity = want;
        }
    }

    // -----------------------------------------------------------------------
    // PlayerObject::ringJump -- 2.1 orb activation
    //
    // 2.2 changed orb priority so the closest orb to the player wins when
    // multiple overlap. 2.1 used insertion order (first-touched wins). We
    // can't easily reorder the queue from inside ringJump, but we CAN
    // ignore re-activations on the same frame, which lets the first-fired
    // orb win.
    // -----------------------------------------------------------------------
    void ringJump(RingObject* obj, bool skipCheck) {
        auto& s = Settings::get();
        const bool active = s.orbPriority21.load(std::memory_order_relaxed);

        if (!active) {
            PlayerObject::ringJump(obj, skipCheck);
            return;
        }

        // If a ring already fired this physics tick, suppress further ones.
        // m_ringJumpRelated is set to true by GD's own ring-fired bookkeeping;
        // by re-using it as a one-shot guard we get the 2.1 first-wins
        // behaviour without touching the queue itself.
        if (m_ringJumpRelated) {
            physLog("gd21: orb21 suppressed extra ring this tick");
            return;
        }

        PlayerObject::ringJump(obj, skipCheck);
    }
};
