# gba-back-home

Starter de **dungeon crawler procedural para Game Boy Advance** con Butano. El tema es el regreso a la casa del Padre: siete moradas, mazmorras interiores, combate básico, oración como ataque secundario, enemigos, objetos, lore, sprites, SFX y una banda sonora MOD inicial.

## Qué contiene el proyecto

- Proyecto Butano listo para compilar (`Makefile`, `src/main.cpp`, `graphics/`, `audio/`).
- Docker completo con devkitARM/devkitPro, Butano, mGBA y escritorio noVNC/VNC con contraseña.
- Devcontainer para VS Code/Codex.
- Sprites BMP indexados + JSON compatibles con el importador de Butano.
- Música `audio/pilgrimage.mod` y SFX WAV 8-bit/22050 Hz.
- Pantalla de “login” local: selección de clave de peregrino sin red, usada como semilla del mundo.
- Logging para mGBA vía `BN_LOG`.
- Documentación de diseño en `docs/`.

## Arranque con Docker

```bash
docker compose build
docker compose up -d gba-dev
docker compose exec gba-dev scripts/doctor.sh
docker compose exec gba-dev scripts/build.sh
docker compose exec gba-dev scripts/run-rom.sh
```

Después abre el emulador en el navegador:

```text
http://localhost:6080/vnc.html
```

Contraseña por defecto: `gba`. Puedes cambiarla con `VNC_PASSWORD` en `docker-compose.yml` o al ejecutar `docker compose`.

## Arranque local sin Docker

Instala devkitARM/devkitPro, Python y un emulador GBA. Luego clona Butano en `vendor/butano`:

```bash
scripts/fetch-butano.sh
make -j$(nproc)
mgba-qt gba-back-home.gba
```

También puedes apuntar a una instalación externa:

```bash
LIBBUTANO=/opt/butano/butano make -j$(nproc)
```

## Controles

- D-Pad: moverse por la morada.
- A: ataque básico hacia la última dirección.
- B: oración expansiva, consume fervor y golpea enemigos cercanos.
- START en mazmorra: regenera la morada actual para depuración.

## Loop jugable incluido

1. Seleccionas una clave local de peregrino.
2. Entras en una de las siete moradas.
3. El mapa se genera proceduralmente con un caminante aleatorio, salas, puertas, santuario, salida, llave, objetos y enemigos.
4. Debes encontrar la llave interior y llegar a la salida.
5. Cada morada aumenta presión, enemigos y densidad.
6. En la séptima salida se muestra el retorno a casa.

## Siguiente trabajo recomendado en Codex

Lee `docs/CODEX_BRIEF.md`. Las ampliaciones más valiosas son: persistencia SRAM real para perfiles, mapas como fondos/tilemaps en lugar de sprites, animaciones por frames, mejor IA, sistema de inventario, composición musical real, y generación de salas conectadas por grafo en vez de caminante simple.
