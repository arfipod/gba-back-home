# Revisión técnica de Butano aplicada al starter

## Decisiones tomadas

- Base de proyecto: estructura equivalente al `template/` de Butano, con `src`, `include`, `graphics`, `audio`, `dmg_audio` y `Makefile`.
- `LIBBUTANO` se resuelve por variable de entorno; en Docker apunta a `/opt/butano/butano`, y fuera de Docker a `vendor/butano/butano`.
- El importador de Butano requiere assets dentro del binario: los sprites están en `graphics/*.bmp` acompañados por `graphics/*.json`.
- Los sprites se han creado como BMP indexados 4bpp con color 0 transparente.
- El texto usa la fuente común de Butano (`common_fixed_8x8_sprite_font`) incorporando `$(LIBBUTANO)/../common/include` y `$(LIBBUTANO)/../common/graphics`.
- Audio Direct Sound: `audio/pilgrimage.mod` para música Maxmod y varios `*.wav` para SFX.
- Logging: `USERFLAGS` fija backend mGBA con `BN_LOG`, útil desde mGBA/NanoBoyAdvance/Mesen.

## Límites GBA relevantes

- La GBA permite 128 sprites hardware; por eso el mapa se renderiza como ventana de 9x6 tiles mediante sprites, no como mapa completo.
- Siguiente mejora importante: convertir el mapa visible o el dungeon completo en background/tilemap para liberar sprites a favor de animaciones, partículas, UI y enemigos.
- La ROM no tiene filesystem: todo asset entra por el pipeline de Butano.
- Persistencia real de perfil requiere SRAM/EEPROM; el “login” actual es una clave local que alimenta la semilla procedural.

## Integración en Codex

Puntos seguros para editar:

- `src/main.cpp`: loop jugable y generación procedural.
- `graphics/*.bmp` y `*.json`: arte temporal.
- `audio/*.wav` y `audio/*.mod`: SFX/música temporal.
- `docs/DESIGN.md`: canon de lore y progresión.

Puntos que conviene no tocar sin revisar Butano:

- `Makefile`: especialmente `LIBBUTANO`, rutas `common` y `AUDIOBACKEND`.
- `Dockerfile`: cambiar de base devkitPro puede romper `gba-dev`, Maxmod o grit.
