# Pipeline de assets

## Gráficos

Butano espera imágenes en `graphics/` acompañadas por un JSON del mismo nombre.

Ejemplo:

```json
{
  "type": "sprite",
  "width": 16,
  "height": 16,
  "bpp_mode": "bpp_4",
  "colors_count": 16
}
```

Reglas del starter:

- BMP sin compresión.
- Paleta indexada de 16 colores.
- Color 0 = transparente.
- Sprites principales de 16x16.
- `attack_wave` y `logo_cross` de 32x32.

Al compilar, Butano genera cabeceras como:

```cpp
#include "bn_sprite_items_player.h"
```

## Audio

- Música Direct Sound en `audio/*.mod`.
- SFX en `audio/*.wav`.
- WAV recomendados: mono, 8-bit, 22050 Hz.
- El starter usa Maxmod (`AUDIOBACKEND := maxmod`).

## Consejos para producción

- Edita sprites en Usenti/Aseprite/GIMP, pero verifica que el BMP final sea paletizado real.
- Mantén paletas pequeñas y compartidas por familia visual.
- Evita demasiados sprites simultáneos: la GBA tiene 128 sprites hardware.
- Cambia tiles estáticos a backgrounds cuanto antes.
