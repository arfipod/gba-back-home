# gba-back-home

A **procedural dungeon crawler starter for Game Boy Advance** built with Butano. The theme is the journey back to the Father's house: seven dwellings, inner dungeons, basic combat, prayer as a secondary attack, enemies, items, lore, sprites, SFX, and an initial MOD soundtrack.

## What This Project Contains

- A Butano project ready to compile (`Makefile`, `src/main.cpp`, `graphics/`, `audio/`).
- A full Docker setup with devkitARM/devkitPro, Butano, mGBA, and a password-protected noVNC/VNC desktop.
- A devcontainer for VS Code/Codex.
- Indexed BMP sprites plus JSON metadata compatible with the Butano importer.
- `audio/pilgrimage.mod` music and 8-bit/22050 Hz WAV SFX.
- A local "login" screen: offline pilgrim key selection used as the world seed.
- mGBA logging through `BN_LOG`.
- Design documentation in `docs/`.

## Docker Startup

```bash
docker compose build
docker compose up -d gba-dev
docker compose exec gba-dev scripts/doctor.sh
docker compose exec gba-dev scripts/build.sh
docker compose exec gba-dev scripts/run-rom.sh
```

Then open the emulator in your browser:

```text
http://localhost:6080/vnc.html
```

Default password: `gba`. You can change it with `VNC_PASSWORD` in `docker-compose.yml` or when running `docker compose`.

## Local Startup Without Docker

Install devkitARM/devkitPro, Python, and a GBA emulator such as mGBA. Make sure `DEVKITPRO`, `DEVKITARM`, and `arm-none-eabi-g++` are available in your shell. Then clone Butano into `vendor/butano`, verify the environment, build, and run:

```bash
scripts/fetch-butano.sh
scripts/doctor.sh
scripts/build.sh
scripts/run-rom.sh
```

You can also point the build at an external Butano installation:

```bash
LIBBUTANO=/opt/butano/butano make -j$(nproc)
```

## Controls

- D-Pad: move through the dwelling.
- A: basic attack toward the last movement direction.
- B: expanding prayer, consumes fervor and hits nearby enemies.
- START in a dungeon: regenerate the current dwelling for debugging.

## Included Play Loop

1. Select a local pilgrim key.
2. Enter one of the seven dwellings.
3. The map is generated procedurally with a random walker, rooms, doors, a shrine, an exit, a key, items, and enemies.
4. Find the inner key and reach the exit.
5. Each dwelling increases pressure, enemy count, and density.
6. The seventh exit shows the return home.

## Recommended Next Work In Codex

Read `docs/CODEX_BRIEF.md`. The most valuable extensions are real SRAM persistence for profiles, maps rendered as backgrounds/tilemaps instead of sprites, frame-based animations, better AI, an inventory system, real musical composition, and graph-connected room generation instead of the current simple walker.
