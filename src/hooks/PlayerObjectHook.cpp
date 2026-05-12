#include "hooks/PlayerObjectHook.hpp"

#include "core/Config.hpp"
#include "gd/Addresses.hpp"
#include "gd/GDClasses.hpp"
#include "hooks/HookManager.hpp"
#include "util/Logger.hpp"

namespace bloom::hooks {

namespace {

using PlayerObject_update_t = void (__fastcall*)(void*, void*, float);
PlayerObject_update_t g_orig_update = nullptr;

void __fastcall hk_update(void* This, void* /*edx*/, float dt) {
    auto& cfg = core::get_config();

    // Practice Rotation Fix: snapshot the rotation before the engine runs its
    // per-frame interpolation, then restore it if the engine's interpolation
    // produced the well-known 2.1 "wrong rotation after checkpoint load" bug.
    //
    // Strategy: if dt is exactly 0 (the engine sets dt=0 on the first frame
    // after a checkpoint load), we save the pre-update rotation and overwrite
    // the post-update rotation with it. This is the classic TasBot fix for
    // the practice-mode rotation drift on 2.1.
    float pre_rot = 0.f;
    bool  did_save = false;
    if (cfg.practice_rotation_fix && dt == 0.f) {
        pre_rot = gd::rotation(static_cast<gd::PlayerObject*>(This));
        did_save = true;
    }

    if (g_orig_update) g_orig_update(This, nullptr, dt);

    if (did_save) {
        gd::rotation(static_cast<gd::PlayerObject*>(This)) = pre_rot;
    }
}

} // namespace

void install_player_object_hooks() {
    static bool installed = false;
    if (installed) return;
    installed = true;

    HookManager::instance().install(
        reinterpret_cast<void*>(gd::resolve_gd(gd::gd21::k_PlayerObject_update)),
        reinterpret_cast<void*>(&hk_update),
        reinterpret_cast<void**>(&g_orig_update),
        "PlayerObject::update");
}

} // namespace bloom::hooks
