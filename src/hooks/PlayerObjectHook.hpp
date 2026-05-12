#pragma once

namespace bloom::hooks {

// Installs PlayerObject::update so we can implement the Practice Rotation Fix.
void install_player_object_hooks();

} // namespace bloom::hooks
