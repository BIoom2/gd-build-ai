#pragma once

namespace bloom::hooks {

// Installs PlayLayer::update, ::pushButton, ::releaseButton, ::resetLevel,
// ::loadFromCheckpoint and ::togglePracticeMode hooks. Safe to call multiple
// times - subsequent calls are no-ops.
void install_play_layer_hooks();

} // namespace bloom::hooks
