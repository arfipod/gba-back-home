# gba-back-home

**gba-back-home** es un dungeon crawler procedural para **Game Boy Advance** hecho con **Butano**. La fantasia central no es conquistar una mazmorra, sino volver a la casa del Padre atravesando siete moradas interiores: avanzar, callar, recordar, discernir puertas y aprender cuando combatir no basta.

Esta version es una **vertical slice ampliada** lista para sustituir el repositorio: incluye Docker, compilador, emulador, sprites, audio placeholder, generacion procedural, reglas de moradas, combate, oracion, silencio, memorias, puertas falsas, santuario con coste, enemigos con roles y boss final.

## Lo que contiene

- Proyecto Butano completo: `Makefile`, `src/main.cpp`, `graphics/`, `audio/`, `dmg_audio/`.
- Docker con devkitPro/devkitARM, Butano, mGBA y noVNC protegido por contrasena.
- Devcontainer para VS Code/Codex.
- Sprites BMP 4bpp + JSON para Butano.
- Musica MOD inicial y SFX WAV.
- Seleccion de peregrino: **FE**, **ESPERANZA**, **CARIDAD**, **PAZ**.
- Generador procedural por salas/corredores con BFS para llave y salida.
- Zona inicial segura, recursos, santuario, puertas falsas y umbrales de silencio.
- Siete moradas con reglas jugables distintas.
- Oracion como dano, pacificacion y discernimiento.
- Accion de silencio/espera.
- Reliquias transformadas en **memorias**.
- Boss final: **La Puerta Falsa**.
- Herramienta host-side para validar semillas: `tools/procedural_smoke_test.py`.

## Docker startup

```bash
docker compose build
docker compose up -d gba-dev
docker compose exec gba-dev scripts/doctor.sh
docker compose exec gba-dev scripts/build.sh
docker compose exec gba-dev scripts/run-rom.sh
```

Abre el emulador en el navegador:

```text
http://localhost:6080/vnc.html
```

Contrasena por defecto: `gba`.

## Startup local sin Docker

Instala devkitARM/devkitPro, Python y un emulador GBA compatible, por ejemplo mGBA. Luego:

```bash
scripts/fetch-butano.sh
scripts/doctor.sh
scripts/build.sh
scripts/run-rom.sh
```

Tambien puedes apuntar a una instalacion externa de Butano:

```bash
LIBBUTANO=/opt/butano/butano make -j$(nproc)
```

## Controles

| Boton | Accion |
|---|---|
| D-Pad | Moverse / orientar al peregrino |
| A | Golpe basico de resolucion hacia la ultima direccion |
| B | Oracion: consume luz, dana/pacifica enemigos y discierne puertas cercanas |
| R | Silencio: esperar, escuchar, recuperar luz o abrir umbrales de silencio |
| L | Mostrar la ultima memoria o una senal |
| SELECT | Ayuda breve de controles |
| START | Pausa; en debug `GBH_DEBUG`, `L+R+START` regenera la morada |

## Peregrinos

- **FE**: +1 luz maxima y mayor alcance de oracion.
- **ESPERANZA**: una caida por run te levanta con 1 HP.
- **CARIDAD**: la oracion pacifica enemigos no-bestia en lugar de destruirlos.
- **PAZ**: los enemigos tienen menos rango de deteccion y empiezas con mas luz.

## Loop jugable

1. Elige peregrino.
2. Entra en una morada.
3. Busca la llave interior.
4. Aprende a usar pan, velas, santuarios, silencio y oracion.
5. Distingue puertas verdaderas, falsas y de silencio.
6. Recoge memorias opcionales.
7. Cruza el umbral.
8. En la septima morada, enfrenta **La Puerta Falsa**.

## Moradas

1. **Conocimiento propio**: aprendizaje, limites, enemigos simples.
2. **Llamada**: `R` orienta hacia llave o salida.
3. **Disciplina**: recursos mas escasos y mas sombras.
4. **Quietud**: esperar en seguridad recupera luz.
5. **Confianza**: mas puertas falsas y enemigos de prisa.
6. **Purificacion**: bestias guardianas y mayor castigo al descuido.
7. **Hogar**: el umbral conduce al boss de discernimiento.

## Pruebas de generacion

La herramienta de smoke test replica el modelo procedural a nivel host y valida conectividad de llave/salida en muchas semillas:

```bash
python3 tools/procedural_smoke_test.py --seeds 1000
```

No sustituye al build de Butano, pero ayuda a detectar regresiones de diseno procedural.

## Siguiente trabajo recomendado

1. Compilar en Docker y ajustar cualquier incompatibilidad de Butano/devkitPro.
2. Migrar el mapa visible de sprites a background/tilemap para liberar OAM.
3. Dividir `src/main.cpp` en modulos (`generator`, `combat`, `ui`, `audio`, `profile`).
4. Anadir SRAM para reliquiario persistente y opciones.
5. Crear arte final y animaciones frame-based.
6. Playtestear 30-50 semillas por morada.
