# Asset Pipeline

## Graphics

Butano expects images in `graphics/` with a JSON file of the same name.

Example:

```json
{
  "type": "sprite",
  "width": 16,
  "height": 16,
  "bpp_mode": "bpp_4",
  "colors_count": 16
}
```

Starter rules:

- Uncompressed BMP.
- Indexed 16-color palette.
- Color 0 = transparent.
- Main sprites are 16x16.
- `attack_wave` and `logo_cross` are 32x32.

When compiling, Butano generates headers such as:

```cpp
#include "bn_sprite_items_player.h"
```

## Audio

- Direct Sound music in `audio/*.mod`.
- SFX in `audio/*.wav`.
- Recommended WAV format: mono, 8-bit, 22050 Hz.
- The starter uses Maxmod (`AUDIOBACKEND := maxmod`).

## Production Tips

- Edit sprites in Usenti/Aseprite/GIMP, but verify that the final BMP is truly palettized.
- Keep palettes small and shared by visual family.
- Avoid too many simultaneous sprites: the GBA has 128 hardware sprites.
- Move static tiles to backgrounds as soon as possible.
