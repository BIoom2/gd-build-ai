#pragma once

#include <cstdint>

#include "gd/Addresses.hpp"

// Minimal partial class layouts for GD 2.1 that Bloom touches. We never embed
// the full layout - just enough fields to read/write the state we care about.
// All access goes through helpers so we don't need to commit to cocos2d-x
// headers in this repo.

namespace bloom::gd {

struct CCPoint { float x; float y; };

class PlayerObject;
class PlayLayer;
class EffectGameObject;

inline void* shared_game_manager() {
    using Fn = void* (__cdecl*)();
    auto fn = reinterpret_cast<Fn>(resolve_gd(gd21::k_GameManager_sharedState));
    if (!fn) return nullptr;
    return fn();
}

inline PlayLayer* shared_play_layer() {
    void* gm = shared_game_manager();
    if (!gm) return nullptr;
    return *reinterpret_cast<PlayLayer**>(
        reinterpret_cast<std::uintptr_t>(gm) + gd21::k_PlayLayer_offset_in_GM);
}

namespace field {
    template <typename T>
    inline T& at(void* obj, std::uintptr_t offset) {
        return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(obj) + offset);
    }
    template <typename T>
    inline const T& at(const void* obj, std::uintptr_t offset) {
        return *reinterpret_cast<const T*>(reinterpret_cast<std::uintptr_t>(obj) + offset);
    }
}

// Accessors (read-only when const, mutable otherwise).
inline PlayerObject* player1(PlayLayer* pl) {
    return field::at<PlayerObject*>(pl, gd21::k_PlayLayer_field_player1);
}
inline PlayerObject* player2(PlayLayer* pl) {
    return field::at<PlayerObject*>(pl, gd21::k_PlayLayer_field_player2);
}
inline bool& is_practice(PlayLayer* pl) {
    return field::at<bool>(pl, gd21::k_PlayLayer_field_isPractice);
}
inline bool& is_paused(PlayLayer* pl) {
    return field::at<bool>(pl, gd21::k_PlayLayer_field_isPaused);
}
inline float& level_time(PlayLayer* pl) {
    return field::at<float>(pl, gd21::k_PlayLayer_field_levelTime);
}

inline CCPoint& position(PlayerObject* p) {
    return field::at<CCPoint>(p, gd21::k_PlayerObject_field_position);
}
inline float& y_velocity(PlayerObject* p) {
    return field::at<float>(p, gd21::k_PlayerObject_field_yVelocity);
}
inline float& rotation(PlayerObject* p) {
    return field::at<float>(p, gd21::k_PlayerObject_field_rotation);
}
inline bool& is_holding(PlayerObject* p) {
    return field::at<bool>(p, gd21::k_PlayerObject_field_isHolding);
}

// EffectGameObject / GameObject accessors.
constexpr int k_PulseTriggerObjectID = 1006;

inline int& object_id(void* obj) {
    return field::at<int>(obj, gd21::k_GameObject_field_objectID);
}
inline float& effect_fade_in(EffectGameObject* e) {
    return field::at<float>(e, gd21::k_EffectGameObject_field_fadeIn);
}
inline float& effect_hold(EffectGameObject* e) {
    return field::at<float>(e, gd21::k_EffectGameObject_field_hold);
}
inline float& effect_fade_out(EffectGameObject* e) {
    return field::at<float>(e, gd21::k_EffectGameObject_field_fadeOut);
}

} // namespace bloom::gd
