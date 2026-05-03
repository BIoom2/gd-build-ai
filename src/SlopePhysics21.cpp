// 2.1 slope physics restoration — placeholder.
//
// 2.1 had several slope quirks that were rewritten in 2.2:
//   - Different y-velocity carry-over when leaving a slope ("catapulting")
//   - Different "snap-to-ground" thresholds at slope/ground transitions
//   - Subtly different `m_isOnSlope` clearance timing
//
// Restoring those requires hooks on:
//   PlayerObject::collidedWithSlope
//   PlayerObject::collidedWithSlopeInternal
//   PlayerObject::getModifiedSlopeYVel  (2.1 only — manual reimpl in 2.2)
//   PlayerObject::checkSnapJumpToObject
//
// Each needs its 2.1 behaviour transplanted. Left as a clearly-marked stub
// so future iterations can fill it in incrementally without touching
// step-rate code.
//
// This file intentionally has no symbols yet — the Settings flag
// `slopePhysics21` is read but no hook is installed. Toggling it at the
// moment is a no-op (documented in mod.json).

#include "Settings.hpp"
