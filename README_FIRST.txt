GALACTIC ANDROID64 PROJECT

Target: Geometry Dash 2.2081 / Geode 5.11.2 / Android64
Mod ID: galactic.inputbuffer

IMPORTANT ACCURACY NOTE
This ZIP is a clean project foundation, not a verified finished predictive-input
or obstacle-warning mod. It includes:
- Input event queue and press/release state tracking
- Input age/window processing
- P1/P2 profile data for all 8 modes
- Basic constant-velocity path projection
- Circle-overlap helper

It does NOT yet implement:
- Actual replay/injection of buffered game inputs
- Verified mode-specific physics
- Scanning level objects for obstacles
- Collision prediction against real level geometry
- Death prevention or guaranteed safe-frame warnings

Gravity is deliberately neutral because the correct units/sign have not been
verified. The PlayLayer hook currently confirms loading and resets state; it
does not pretend to have complete gameplay hooks.

Before relying on it in-game, build it in GitHub Actions and verify the exact
Geode SDK/workflow compatibility. A green build proves compilation only.
