// Texture-quality hooks: forces UHD asset variants and applies the
// user-selected min/mag/anisotropic filtering to every CCTexture2D as
// soon as it finishes uploading its data to the GPU.
//
// Why two layers (UHD + filter):
//   * "Force UHD" only changes which file on disk gets loaded — it does
//     nothing about the way that bitmap is sampled when drawn.
//   * Texture filtering changes the GL sampler state on the already-
//     uploaded GL texture, so smoothing applies regardless of which
//     -uhd / -hd / no-suffix variant got loaded.
//
// Both are independently toggleable via mod settings.

#include <Geode/Geode.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/modify/CCTexture2D.hpp>

#include "Settings.hpp"

using namespace geode::prelude;
using maxq::Settings;

namespace {

// Apply the user-selected filtering to one already-initialised
// CCTexture2D. Safe to call from anywhere on the main thread; it only
// touches public CCTexture2D / GL state.
void applyMaxQualityFilter(CCTexture2D* tex) {
    auto& s = Settings::get();
    if (!s.enabled.load(std::memory_order_relaxed)) return;

    GLuint name = tex->getName();
    if (name == 0) return; // never uploaded

    int  mode  = s.filterMode.load(std::memory_order_relaxed);
    int  aniso = s.anisotropic.load(std::memory_order_relaxed);

    // setTexParameters() does the bind for us. Wrap mode is left at
    // CLAMP_TO_EDGE because GD textures are atlases — repeating across
    // sub-rect boundaries would bleed into neighbouring sprites.
    switch (mode) {
        case maxq::kFilterNearest: {
            ccTexParams p = {
                static_cast<GLuint>(GL_NEAREST),
                static_cast<GLuint>(GL_NEAREST),
                static_cast<GLuint>(GL_CLAMP_TO_EDGE),
                static_cast<GLuint>(GL_CLAMP_TO_EDGE),
            };
            tex->setTexParameters(&p);
            break;
        }
        case maxq::kFilterBilinear: {
            ccTexParams p = {
                static_cast<GLuint>(GL_LINEAR),
                static_cast<GLuint>(GL_LINEAR),
                static_cast<GLuint>(GL_CLAMP_TO_EDGE),
                static_cast<GLuint>(GL_CLAMP_TO_EDGE),
            };
            tex->setTexParameters(&p);
            break;
        }
        case maxq::kFilterTrilinear: {
            // Mipmaps require POT textures. cocos2d-x logs and bails on
            // NPOT, leaving the existing filter intact — that's fine,
            // we then fall back to bilinear-ish behaviour.
            if (!tex->hasMipmaps()) {
                tex->generateMipmap();
            }
            ccTexParams p = {
                static_cast<GLuint>(GL_LINEAR_MIPMAP_LINEAR),
                static_cast<GLuint>(GL_LINEAR),
                static_cast<GLuint>(GL_CLAMP_TO_EDGE),
                static_cast<GLuint>(GL_CLAMP_TO_EDGE),
            };
            tex->setTexParameters(&p);
            break;
        }
        default:
            break;
    }

    // Anisotropic filtering. Querying the driver max once per call is
    // cheap (the value is cached in the GL state). 1.0 = isotropic;
    // higher = better quality on slanted / scaled sprites.
    if (aniso > 1) {
        glBindTexture(GL_TEXTURE_2D, name);

        GLfloat driverMax = 1.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &driverMax);

        GLfloat use = std::min(static_cast<GLfloat>(aniso), driverMax);
        if (use >= 1.0f) {
            glTexParameterf(GL_TEXTURE_2D,
                GL_TEXTURE_MAX_ANISOTROPY_EXT, use);
        }
    }

    if (s.verboseLog.load(std::memory_order_relaxed)) {
        log::debug(
            "maxq: filtered tex#{} mode={} aniso={}",
            name, mode, aniso);
    }
}

} // namespace

// ---------------------------------------------------------------------------
// AppDelegate::setupGLView — picks the GL view's content scale factor.
// We override the value AFTER the original runs so any of GD's resolution-
// dependent setup (window size, design resolution) is intact, and then
// nudge the scale factor up to >= 2.0 so cocos2d's resource-suffix logic
// will pick `-uhd` variants for every subsequent file load.
// ---------------------------------------------------------------------------
class $modify(MaxQualityAppDelegate, AppDelegate) {
    void setupGLView() {
        AppDelegate::setupGLView();

        auto& s = Settings::get();
        if (s.enabled.load(std::memory_order_relaxed)
            && s.forceUhd.load(std::memory_order_relaxed)) {
            // 2.0 is GD's threshold for selecting -uhd. Anything >= 2.0
            // gives the same effect.
            CCDirector::sharedDirector()->setContentScaleFactor(2.0f);

            // Drop any cached path lookups so the next load re-resolves
            // through the new content-scale logic.
            CCFileUtils::sharedFileUtils()->purgeCachedEntries();

            log::info(
                "maxq: forced UHD content scale factor (= 2.0)");
        }
    }
};

// ---------------------------------------------------------------------------
// CCTexture2D::initWithImage — primary entry point GD uses for loading
// PNG/JPG sprites into a CCTexture2D. We let the original do its upload,
// then immediately retune the GL sampler state to our settings.
//
// For PVR / ETC / raw-data textures we additionally hook the relevant
// initWith* below, all funneling into the same applyMaxQualityFilter().
// ---------------------------------------------------------------------------
class $modify(MaxQualityTexture, CCTexture2D) {
    bool initWithImage(CCImage* image) {
        bool ok = CCTexture2D::initWithImage(image);
        if (ok) applyMaxQualityFilter(this);
        return ok;
    }

    bool initWithData(
        void const* data,
        CCTexture2DPixelFormat pixelFormat,
        unsigned int pixelsWide,
        unsigned int pixelsHigh,
        CCSize const& contentSize
    ) {
        bool ok = CCTexture2D::initWithData(
            data, pixelFormat, pixelsWide, pixelsHigh, contentSize);
        if (ok) applyMaxQualityFilter(this);
        return ok;
    }

    bool initWithPVRFile(char const* file) {
        bool ok = CCTexture2D::initWithPVRFile(file);
        if (ok) applyMaxQualityFilter(this);
        return ok;
    }

    bool initWithETCFile(char const* file) {
        bool ok = CCTexture2D::initWithETCFile(file);
        if (ok) applyMaxQualityFilter(this);
        return ok;
    }
};

namespace maxq {

// Re-applies the current filter settings to every texture cocos2d
// currently knows about. Used when the user toggles a setting at
// runtime so already-loaded textures pick up the new filter without
// having to restart the game.
void retuneAllTextures() {
    auto& s = Settings::get();
    if (!s.enabled.load(std::memory_order_relaxed)) return;

    auto* cache = CCTextureCache::sharedTextureCache();
    if (!cache) return;

    auto* dict = cache->snapshotTextures();
    if (!dict) return;

    int touched = 0;
    CCDictElement* el = nullptr;
    CCDICT_FOREACH(dict, el) {
        // typeinfo_cast is the cross-DLL-safe version of dynamic_cast
        // recommended by Geode for cocos2d types.
        if (auto* tex = typeinfo_cast<CCTexture2D*>(el->getObject())) {
            applyMaxQualityFilter(tex);
            ++touched;
        }
    }

    log::info("maxq: re-tuned {} textures from cache", touched);
}

} // namespace maxq
