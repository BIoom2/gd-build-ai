# Max Quality

A Geode mod for **Geometry Dash 2.2081 / Geode 5.6.1** that forces every
visual subsystem in the game to its highest possible quality.

| Feature | What it does | Setting |
|---|---|---|
| Force UHD textures | Always loads the `-uhd` asset variants regardless of screen resolution | `force-uhd` |
| Texture filtering | Switches every `CCTexture2D` between `nearest` / `bilinear` / `trilinear` sampling | `texture-filter` |
| Anisotropic filtering | Applies up to 16x anisotropic filtering to every texture (driver-clamped) | `anisotropic` |
| Particle multiplier | Multiplies every emitter's `totalParticles` (0.5x – 8x) | `particle-multiplier` |
| Particle cap removal | Lifts cocos2d's 16k soft cap so dense effects can fully render | `uncap-particles` |

## Installation

1. Install [Geode](https://geode-sdk.org/) for Windows GD 2.2081 if you haven't already.
2. Drop the released `.geode` file into:
   `<Geometry Dash folder>/geode/mods/`
   (or use the in-game mod browser → **Install from File**).
3. Launch GD. Open **Geode → Mods → Installed → Max Quality** to tune the settings.

## Building from source

> Standard Geode mod build. Requires a working Geode CLI + SDK.

```bash
# Configures & builds for the host platform
geode build

# Or, on systems where the auto-detected generator misbehaves
# (clang + ninja on Linux/macOS):
geode build --ninja
```

CI also produces a Windows `.geode` artifact on every push — see the
**Build Max Quality** workflow under the Actions tab. Download the
`max-quality-build` artifact from a successful run, unzip it, and the
`.geode` file inside is ready to install.

## How it works (technical)

* **UHD force** — hooks `AppDelegate::setupGLView` and, after the
  original runs, calls `CCDirector::setContentScaleFactor(2.0f)` plus
  `CCFileUtils::purgeCachedEntries()`. From that point on, GD's path
  resolution selects the `-uhd` suffix on every load.
* **Filtering** — hooks every `CCTexture2D::initWith*` so right after a
  bitmap is uploaded to the GPU we call `setTexParameters` with
  `GL_LINEAR` / `GL_LINEAR_MIPMAP_LINEAR` and (when supported)
  `GL_TEXTURE_MAX_ANISOTROPY_EXT`. When the user toggles a filter
  setting at runtime, the entry-point code walks
  `CCTextureCache::snapshotTextures()` and re-tunes every existing
  texture so the change takes effect without a restart.
* **Particles** — hooks `CCParticleSystem::initWithTotalParticles` to
  multiply the requested allocation by the user's multiplier, clamped
  to a hard ceiling of 64k (or 16k with `uncap-particles` off).

## License

MIT.
