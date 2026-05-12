#pragma once

#include <cstdint>

namespace bloom::gd {

// -----------------------------------------------------------------------------
// Geometry Dash 2.1 (build 2.111) - gd.dll offsets used by Bloom.
//
// Notes:
//   * These are RVA offsets from the start of gd.dll. Combine with the runtime
//     base of gd.dll (GetModuleHandleA("gd.dll")) to get the absolute address.
//   * Each offset is also resolvable by AOB pattern (see util/Pattern.hpp) -
//     fall back to the pattern if the offset is wrong on a particular build.
//   * If you target a different 2.1 sub-build (Steam vs standalone, EU/RU
//     variants), edit Bloom.dll-side `bloom_settings.json` "addresses" override
//     instead of recompiling.
// -----------------------------------------------------------------------------

namespace gd21 {
    // Singletons / accessors.
    constexpr std::uintptr_t k_GameManager_sharedState     = 0x000C4A40;
    constexpr std::uintptr_t k_PlayLayer_offset_in_GM      = 0x00000164; // *(PlayLayer**)(GM + 0x164)

    // PlayLayer methods we hook.
    constexpr std::uintptr_t k_PlayLayer_update            = 0x0020D810;
    constexpr std::uintptr_t k_PlayLayer_pushButton        = 0x0020FBE0;
    constexpr std::uintptr_t k_PlayLayer_releaseButton     = 0x0020FD30;
    constexpr std::uintptr_t k_PlayLayer_resetLevel        = 0x0020E150;
    constexpr std::uintptr_t k_PlayLayer_loadFromCheckpoint= 0x00216020;
    constexpr std::uintptr_t k_PlayLayer_togglePracticeMode= 0x002187D0;
    constexpr std::uintptr_t k_PlayLayer_createCheckpoint  = 0x00215AA0;

    // PlayerObject methods.
    constexpr std::uintptr_t k_PlayerObject_update         = 0x001E4810;
    constexpr std::uintptr_t k_PlayerObject_pushButton     = 0x001EA0F0;
    constexpr std::uintptr_t k_PlayerObject_releaseButton  = 0x001EA240;

    // GJBaseGameLayer (parent of PlayLayer).
    constexpr std::uintptr_t k_GJBaseGameLayer_processCommands = 0x0014DAB0;

    // Field offsets inside PlayerObject (relative to `this`).
    constexpr std::uintptr_t k_PlayerObject_field_position     = 0x0000067C; // CCPoint
    constexpr std::uintptr_t k_PlayerObject_field_yVelocity    = 0x00000628;
    constexpr std::uintptr_t k_PlayerObject_field_rotation     = 0x000003C0; // float (cocos m_fRotationX)
    constexpr std::uintptr_t k_PlayerObject_field_isHolding    = 0x0000061A; // bool

    // Field offsets inside PlayLayer.
    constexpr std::uintptr_t k_PlayLayer_field_player1         = 0x00000224;
    constexpr std::uintptr_t k_PlayLayer_field_player2         = 0x00000228;
    constexpr std::uintptr_t k_PlayLayer_field_isPractice      = 0x00000495;
    constexpr std::uintptr_t k_PlayLayer_field_isPaused        = 0x000002EC;
    constexpr std::uintptr_t k_PlayLayer_field_levelTime       = 0x000003E4; // float (seconds since start)

    // FPS bypass injection points (replace cdecl callsites that read a constant).
    constexpr std::uintptr_t k_FPS_setupDelta_constant_site    = 0x0007A560;
}

// Convenience: get absolute address for a gd.dll RVA.
std::uintptr_t resolve_gd(std::uintptr_t rva);

} // namespace bloom::gd
