# Implemented Design Changes

This file summarizes the requested design pass implemented in this package.

## Core Identity

- Shifted the game from a generic procedural crawler starter into a spiritual micro-roguelike about returning home.
- Standardized language around morada, umbral, luz, memoria, silencio and peregrino.
- Moved the design away from conquest and toward discernment.

## Player Profiles

Implemented four pilgrim identities:

- **FE**: larger light pool and longer prayer range.
- **ESPERANZA**: one automatic recovery from death per run.
- **CARIDAD**: prayer pacifies non-beast enemies.
- **PAZ**: lower enemy detection range and extra starting light.

## Dungeon Loop

- Replaced pure random walker with connected room/corridor generation.
- Added BFS distance logic for exit and key placement.
- Added safer start spacing for enemies/items.
- Added memories and optional branch rewards.
- Added explicit key-to-exit loop.

## Morada Rules

- Morada I: safer introduction.
- Morada II: silence gives directional calling.
- Morada III: tighter resource pressure.
- Morada IV: silence recovers light when safe.
- Morada V: more false doors and haste enemies.
- Morada VI: beast guardians.
- Morada VII: final boss transition.

## Combat

- Basic attack is framed as resolution.
- Prayer now damages, pacifies and discerns.
- Silence is a turn action with tactical and narrative purpose.
- Moving into enemies attacks them, keeping roguelike tactility.

## Doors

- Added false doors: harmful unless discerned with prayer.
- Added silence doors: opened with `R` or prayer.
- Exit requires the inner key.
- START no longer regenerates floors in release; it pauses.

## Shrine

- Shrine no longer instantly refills on contact.
- Player must use three turns of silence on it.
- Shrine becomes spent after use.

## Enemies

- **Doubt**: slow pursuer.
- **Shadow**: fast enemy, avoids shrines.
- **Noise**: drains light nearby.
- **Haste**: punished by silence; moves faster if player rushes.
- **Beast**: slow, high damage, guardian role.

## Lore

- Relics are now memories.
- Memory lines are short and reusable.
- Defeat now gives a cause-flavored line.
- Victory completes the recurring phrase: "No estaba lejos. Era hondo."

## Boss

- Added **La Puerta Falsa**.
- It alternates noise and silence windows.
- Attacking feeds/hurts the player.
- Silence/prayer during the right window advances victory.

## Assets

Added placeholder sprites:

- `player_down/up/left/right`
- `tile_silence`
- `tile_door_false`
- `tile_door_silence`
- `tile_shrine_spent`
- `enemy_noise`
- `enemy_haste`
- `boss_false_door`

All are checked in as indexed 4bpp BMP plus JSON metadata for Butano.

## Tooling

- Added `tools/procedural_smoke_test.py` to validate procedural connectivity at host level.
- Expanded `tools/generate_assets.py` so placeholder sprites can be regenerated.
