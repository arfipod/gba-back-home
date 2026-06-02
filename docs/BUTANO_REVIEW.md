# Technical Butano Review Applied To The Starter

## Project Decisions

- Project base follows Butano's template layout: `src`, `include`, `graphics`, `audio`, `dmg_audio`, `Makefile`.
- `LIBBUTANO` is resolved through an environment variable; Docker points to `/opt/butano/butano`, local fallback is `vendor/butano/butano`.
- Text uses Butano's common fixed 8x8 sprite font through `$(LIBBUTANO)/../common/include` and `$(LIBBUTANO)/../common/graphics`.
- Direct Sound audio uses `audio/pilgrimage.mod` and SFX WAV files with Maxmod.
- Logging is enabled for mGBA with `BN_LOG` through `USERFLAGS`.

## Important GBA Limits

- GBA has 128 hardware sprites.
- Current dungeon uses sprite tiles: 9x6 = 54 sprites.
- Enemies, items, player, effects and text share the same OAM budget.
- Keep HUD and message text short.
- The next major technical upgrade should move map rendering to a background/tilemap.

## Added Gameplay Assets

The design pass added directional player sprites, silence/false-door tiles, new enemies and a boss. Each has a matching JSON file for the Butano importer.

## Debug vs Release

- START now pauses during dungeon play.
- Floor regeneration is only behind `GBH_DEBUG` with `L+R+START`.
- To enable debug regeneration:

```make
USERFLAGS := -DGBH_DEBUG -DBN_CFG_LOG_ENABLED=true -DBN_CFG_LOG_BACKEND=BN_LOG_BACKEND_MGBA
```

## Safe Points To Edit

- `src/main.cpp`: boot loop only.
- `src/gbh_game_flow.cpp`: mode transitions, title/profile/lore screens, run setup.
- `src/gbh_dungeon_generation.cpp`: map generation, placement, entity spawning.
- `src/gbh_dungeon_render.cpp`: sprite creation, viewport refresh, HUD/messages.
- `src/gbh_dungeon_actions.cpp`: player actions, combat, silence, items, enemy turns.
- `src/gbh_boss_and_results.cpp`: boss, victory and defeat screens.
- `src/gbh_localization.cpp` and `src/gbh_assets.cpp`: text and sprite item lookup.
- `graphics/*.bmp` and `graphics/*.json`: placeholder art.
- `audio/*.wav` and `audio/*.mod`: placeholder audio.
- `docs/DESIGN.md`: lore and design canon.
- `tools/procedural_smoke_test.py`: host-side model for generator validation.

## Points To Treat Carefully

- `Makefile`: especially `LIBBUTANO`, common paths and audio backend.
- `Dockerfile`: changing the devkitPro base can break devkitARM, grit or Maxmod.
- Long text strings in dungeon: they can exceed hardware sprites.
