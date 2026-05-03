# GD 2.1 Physics

A Geode mod for **Geometry Dash 2.2** that restores **2.1-era physics** so old 2.1 levels (which rely on bugs/quirks RobTop fixed) become playable again.

## What this mod does

In 2.2, RobTop rewrote the physics step:

- **2.1**: `4 physics updates per frame`, tied to FPS. At 60 FPS that's 240 Hz, at 240 FPS that's **960 Hz** (this is what causes the famous "high-FPS bugs").
- **2.2**: A *fixed* 240 Hz physics step. Same behavior at every frame rate — but a lot of 2.1 timing-sensitive levels broke as a result.

This mod hooks `PlayerObject` and `GJBaseGameLayer` and:

1. Forces the **FPS-locked 2.1 step model** (4 substeps per frame) when enabled.
2. Restores **2.1 slope physics** (the ones that let you "catapult" off slopes).
3. Preserves selected **2.1 quirks / bugs** that old levels (e.g. *Warcora* by itzhewy) depend on.

## Settings

- **Enable 2.1 Physics** — master toggle.
- **Physics Step Rate** — `fps-locked-21` (default) vs. `fixed-240` (vanilla 2.2).
- **2.1 Slope Physics** — toggle 2.1 slope behavior independently.
- **Preserve 2.1 Bugs** — keep quirks that levels rely on.
- **Verbose Physics Log** — debug only.

## Status

Early WIP. Gradually expanding gamemode coverage:

- [x] Mod skeleton + settings
- [x] PlayerObject::update hook scaffolding
- [ ] Cube gravity / jump constants tuned to 2.1
- [ ] 2.1 slope physics
- [ ] Ship, Ball, UFO, Wave, Robot, Spider, Swing
- [ ] Specific bug catalogue (corner snap, wave slice, etc.)

## Credits

- Mod by **the411539** (Devin-assisted)
- Based on physics differences documented in the public 2.2 Physics Explainer
