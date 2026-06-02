# gba-back-home Design

## Core Fantasy

The protagonist does not "explore a castle" outside themselves: they cross an inner architecture. Each dungeon is a dwelling on the way back home. The goal is not to conquer, but to order, remember, and return.

## Structure

Seven dwellings:

1. **Self-Knowledge**: simple enemies, many walls, learning.
2. **Calling**: more doors and long corridors.
3. **Discipline**: persistent enemies, fewer resources.
4. **Stillness**: shrines matter more, fervor resources.
5. **Trust**: distant keys, shadow-type enemies.
6. **Purification**: strong enemies, punishment for carelessness.
7. **Home**: final exit, victory loop.

## Current Procedural System

- The whole map starts as walls.
- A random walker carves floors from the center.
- Every fixed number of steps opens a small room.
- The exit is calculated as the floor tile farthest from the origin.
- The shrine, decorative doors, key, resources, and enemies are placed on valid floor tiles.
- The pilgrim key modifies the seed.

## Combat

- **Basic Attack (A)**: hits one tile in the last movement direction.
- **Expanding Prayer (B)**: consumes 2 fervor and damages enemies at Manhattan distance <= 2.
- Adjacent enemies damage the player when taking a turn.
- Shadows move faster; beasts deal more damage.

## Items

- **Bread**: heals health.
- **Candle**: restores fervor.
- **Inner Key**: opens the current dwelling's exit.
- **Relic/Memory**: lore counter, future narrative unlock.
- **Shrine**: restores health and fervor when stepped on.

## Original Lore Baseline

The text avoids preaching: it uses brief, almost liturgical phrases so the player completes the meaning during exploration. The baseline vocabulary is door, dwelling, silence, shadow, key, house, Father, bread, light, night, grace.

## Planned Improvements

- Graph-based room generator with a guaranteed critical path.
- Visual biomes per dwelling.
- Enemies with simple telemetry: patrol, fleeing from light, key guardian.
- Reliquary with unlockable entries.
- SRAM for profile, statistics, and progress.
- Tilemap/background rendering to free sprites.
- Music per dwelling with a shared leitmotif.
