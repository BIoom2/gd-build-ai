#pragma once

namespace bloom::hooks {

// Installs the EffectGameObject::triggerObject hook that enables the
// "Pulse Trigger Loop" feature. When Config::pulse_trigger_loop is true,
// completed pulse triggers are automatically re-fired so the pulse effect
// repeats indefinitely.  Safe to call multiple times.
void install_pulse_trigger_hooks();

// Called every physics frame (from the PlayLayer::update hook) to re-fire
// any tracked pulse triggers whose cycle has elapsed.
void tick_pulse_trigger_loop(void* play_layer, float dt);

// Clears all tracked pulse triggers (called on level reset).
void reset_pulse_trigger_state();

} // namespace bloom::hooks
