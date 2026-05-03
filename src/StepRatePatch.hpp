#pragma once

namespace gd21 {

// Globally accessible step count used by the 2.1 step-rate patch.
//
// When the patch is enabled, GD's machine code that calculates the number of
// physics substeps per frame is replaced with a 13-byte stub that reads from
// this variable instead. We update it from a hook on
// `GJBaseGameLayer::getModifiedDelta` so that the value is fresh every frame.
//
// Adapted from Click Between Frames (MIT, theyareonit).
extern int g_stepCount;

// Apply / remove the in-engine memory patch. Safe to call multiple times.
void enableStepRatePatch(bool enable);

// Compute the 2.1-style step count for the given frame delta and timewarp.
// 2.1 used `max(4, delta * 240) / timewarp`, i.e. at least 4 substeps per
// frame and FPS-scaled (60 fps -> 4, 240 fps -> 16, etc.)
int compute21StepCount(double delta, float timewarp);

// Vanilla 2.2 formula, used as a fallback when the user picks "fixed-240".
int computeVanillaStepCount(double delta, float timewarp);

} // namespace gd21
