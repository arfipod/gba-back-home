# Brief For Continuing In Codex

## Current State

This repository is now a larger design slice instead of the original bare starter. The game has seven dwellings, pilgrim profiles, a room/corridor procedural generator, BFS placement for key/exit, safe start rules, prayer, silence, false doors, silence doors, exhausted shrines, memories, five enemy roles and a final boss.

## First Goal

Make the project compile cleanly in Docker, run it in mGBA, and playtest the whole title-to-victory path. Fix any Butano import, sprite, audio or API mismatch before adding new features.

## Immediate Acceptance Criteria

- `docker compose run --rm gba-dev scripts/build.sh` generates `gba-back-home.gba`.
- `scripts/run-rom.sh` opens the ROM in mGBA through noVNC.
- `docs/EMULATOR_SCREENSHOTS.md` documents the repeatable mGBA/noVNC screenshot and visual-check workflow.
- The game can reach victory from title without hangs.
- Visible sprites never exceed the 128 hardware sprite limit.
- Key and exit are reachable for all tested seeds.
- START pauses in release; regeneration only exists behind `GBH_DEBUG`.

## Technical Priority Order

1. **Compile and fix API mismatches**: especially new sprite item names, keypad helpers, text sprite counts and audio item names.
2. **Run procedural smoke test**:
   ```bash
   python3 tools/procedural_smoke_test.py --seeds 1000
   ```
3. **Verify OAM budget** in emulator. The map still uses 9x6 sprite tiles, so text lines must remain short.
4. **Move dungeon map to background/tilemap**. This frees sprites for animation, particles and richer UI.
5. Continue refining module boundaries as features grow:
   - split enemy AI from player actions if combat expands
   - split HUD/message rendering from viewport rendering if UI grows
   - introduce an audio manager once sound rules become stateful
6. Add SRAM for profile, reliquary and options.
7. Add sprite animations and hurt/attack telegraphs.
8. Add automated host-side tests for the exact C++ generator.

## Design Priority Order

1. Polish dwelling I as a perfect 3-5 minute vertical slice.
2. Give each dwelling stronger room grammar, not just parameter changes.
3. Make silence/waiting feel valuable and risky.
4. Improve false door telegraphing so it is fair, not random-feeling.
5. Convert memories into a reliquary screen.
6. Add final-boss readability: clear noise/silence cycle.
7. Add pacification feedback for Charity.
8. Balance shrines so they feel like a commitment, not a free refill.

## Known Fragile Points

- `src/main.cpp` is now only the boot loop. Gameplay lives in responsibility-focused `gbh_*` modules.
- Butano text uses sprites; long text lines can exceed OAM during dungeon scenes.
- New BMP assets are placeholder art. They are 4bpp indexed with JSON metadata.
- The procedural smoke test mirrors the generator conceptually, but the C++ source is the game authority.
- The boss is a design prototype: refine timing, feedback and damage after playtesting.

## Suggested First Codex Prompt

"Compile this Butano project in the provided Docker environment. Fix all build errors without changing the intended game design. Prioritize asset importer names, Butano API mismatches, and sprite/text budget. Then run the ROM in mGBA and report the first playable issues."
