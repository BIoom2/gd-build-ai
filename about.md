# Max Quality

**Maximum graphical fidelity for Geometry Dash 2.2081.**

This mod squeezes every drop of detail out of the game's existing assets:

- **Force UHD textures** — every sprite is loaded at its highest-resolution variant regardless of your screen size, so textures never look pixelated even when the game is upscaled.
- **Smooth filtering + anti-aliasing** — replaces the default nearest-neighbour sampling on most textures with bilinear or trilinear filtering plus up-to-16x anisotropic filtering. This delivers an effective anti-aliased look on every rotated, scaled, or slanted sprite — which is most of the visible game geometry.
- **Particle multiplier** — multiplies every emitter's `totalParticles` so dense effects (rocket trails, ship dust, explosions) actually render in full.
- **Particle cap removal** — removes the internal cap GD imposes on simultaneously-alive particles so the multiplier above can really stretch its legs.

All of the above are individually toggleable in the mod settings. Set the master switch off and the mod becomes a complete no-op.

## Compatibility

- **Game version:** Geometry Dash 2.2081 on Windows (only platform built/tested).
- **Geode version:** 5.6.1.
- **Texture-pack mods:** fully compatible with [Texture Loader](https://geode-sdk.org/mods/geode.texture-loader). Install your favourite HD pack as usual; this mod will scale, filter, and smooth it.

## Notes

- True hardware MSAA is not exposed because Geometry Dash's GL context is created before mod code runs, and recreating the context at runtime is not safe. Trilinear + 16x anisotropic filtering on every texture is the next-best thing and gives a strongly anti-aliased look on the actual visible geometry (which is almost all sprites).
- "Force UHD" can slightly increase VRAM usage. If you see flickering on a low-VRAM GPU, drop the setting.
- The particle multiplier is hard-clamped at 8x to prevent emitter allocations from exploding.
