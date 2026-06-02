# Technical Butano Review Applied To The Starter

## Decisions Made

- Project base: structure equivalent to Butano's `template/`, with `src`, `include`, `graphics`, `audio`, `dmg_audio`, and `Makefile`.
- `LIBBUTANO` is resolved through an environment variable; in Docker it points to `/opt/butano/butano`, and outside Docker it points to `vendor/butano/butano`.
- The Butano importer requires assets inside the binary: sprites live in `graphics/*.bmp` with matching `graphics/*.json`.
- Sprites were created as indexed 4bpp BMPs with transparent color 0.
- Text uses Butano's common font (`common_fixed_8x8_sprite_font`) by including `$(LIBBUTANO)/../common/include` and `$(LIBBUTANO)/../common/graphics`.
- Direct Sound audio: `audio/pilgrimage.mod` for Maxmod music and several `*.wav` files for SFX.
- Logging: `USERFLAGS` sets the mGBA backend with `BN_LOG`, useful in mGBA/NanoBoyAdvance/Mesen.

## Relevant GBA Limits

- The GBA allows 128 hardware sprites; that is why the map is rendered as a 9x6 tile window using sprites, not as a full map.
- Next important improvement: convert the visible map or full dungeon into a background/tilemap to free sprites for animations, particles, UI, and enemies.
- The ROM has no filesystem: every asset enters through the Butano pipeline.
- Real profile persistence requires SRAM/EEPROM; the current "login" is a local key that feeds the procedural seed.

## Codex Integration

Safe points to edit:

- `src/main.cpp`: playable loop and procedural generation.
- `graphics/*.bmp` and `*.json`: placeholder art.
- `audio/*.wav` and `audio/*.mod`: placeholder SFX/music.
- `docs/DESIGN.md`: lore and progression canon.

Points that should not be touched without reviewing Butano:

- `Makefile`: especially `LIBBUTANO`, `common` paths, and `AUDIOBACKEND`.
- `Dockerfile`: changing the devkitPro base can break `gba-dev`, Maxmod, or grit.
