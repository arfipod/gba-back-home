# Asset Pipeline

## Butano Import Shape

Butano imports assets that are inside the repository at build time. This project keeps placeholder art in `graphics/` as indexed BMP files, each with a matching JSON metadata file.

Example:

```text
graphics/player_down.bmp
graphics/player_down.json
```

The generated C++ include becomes:

```cpp
#include "bn_sprite_items_player_down.h"
```

and the sprite item is used as:

```cpp
bn::sprite_items::player_down.create_sprite(0, 0);
```

## Current Sprite Set

### Tiles

- `tile_floor`
- `tile_wall`
- `tile_door`
- `tile_door_false`
- `tile_door_silence`
- `tile_silence`
- `tile_shrine`
- `tile_shrine_spent`
- `tile_exit`

### Player

- `player_down`
- `player_up`
- `player_left`
- `player_right`

### Enemies

- `enemy_doubt`
- `enemy_shadow`
- `enemy_beast`
- `enemy_noise`
- `enemy_haste`
- `boss_false_door`

### Items / Effects

- `item_bread`
- `item_candle`
- `item_key`
- `item_relic` used as memory icon
- `attack_wave`

## Format Requirements

- BMP indexed/palettized.
- 4bpp mode expected by JSON.
- 16 colors.
- Palette index 0 reserved for transparency.
- Most sprites are 16x16.
- `attack_wave` and `boss_false_door` are 32x32.

## Regenerating Placeholder Art

```bash
python3 tools/generate_assets.py
```

This regenerates the placeholder sprites as 4bpp BMP files. It is not a final art pipeline; final art should come from Aseprite, Usenti or a similar indexed-palette workflow.

## Audio

- `audio/pilgrimage.mod` is the placeholder MOD soundtrack.
- `audio/*.wav` files are SFX.
- The Makefile uses `AUDIOBACKEND := maxmod`.

## OAM Budget Warning

The dungeon currently renders the visible map as a 9x6 sprite window:

```text
54 tile sprites
+ 8 enemy sprites
+ 8 item sprites
+ 1 player sprite
+ 1 attack sprite
+ text sprites
```

This is close to the GBA hardware sprite limit when long messages are shown. Keep in-game messages short until the map is moved to a background/tilemap.
