# Brief For Continuing In Codex

## Immediate Goal

Turn this starter into a solid vertical slice: one playable 3-5 minute dwelling with clear combat, coherent art, a simple boss, and a narrative ending.

## Technical Priorities

1. **Compile in Docker** and fix any Butano/devkitPro version incompatibilities.
2. Replace sprite-based tile rendering with a procedural background/tilemap.
3. Split `src/main.cpp` into modules:
   - `dungeon_generator.*`
   - `combat.*`
   - `entities.*`
   - `ui.*`
   - `audio_manager.*`
   - `profile.*`
4. Add SRAM for a real profile.
5. Add sprite animations and visual cooldowns.
6. Create host-side tests for the procedural generator using the same algorithm in portable C++.

## Design Priorities

1. Give each dwelling its own gameplay rule.
2. Guarantee that the key and exit are always reachable.
3. Turn relics into persistent lore fragments.
4. Add at least three enemy patterns:
   - doubt: slow pursuer;
   - shadow: fast movement, low health;
   - beast: door guardian, high health.
5. Add a final boss, "The False Door", defeated by avoiding attacks during windows of silence.

## Acceptance Criteria

- `docker compose run --rm gba-dev scripts/build.sh` generates `gba-back-home.gba`.
- `scripts/run-rom.sh` opens mGBA through noVNC.
- The game can go from title to victory without hangs.
- Visible sprites never exceed 128.
- The map never traps the key or exit.
