# Brief para continuar en Codex

## Objetivo inmediato

Convertir este starter en un vertical slice sólido: una morada jugable de 3-5 minutos con combate claro, arte coherente, un jefe simple y cierre narrativo.

## Prioridades técnicas

1. **Compilar en Docker** y corregir cualquier incompatibilidad de versión de Butano/devkitPro.
2. Sustituir render de tiles por background/tilemap procedural.
3. Separar `src/main.cpp` en módulos:
   - `dungeon_generator.*`
   - `combat.*`
   - `entities.*`
   - `ui.*`
   - `audio_manager.*`
   - `profile.*`
4. Añadir SRAM para perfil real.
5. Añadir animaciones de sprites y cooldown visual.
6. Crear tests host-side para el generador procedural usando el mismo algoritmo en C++ portable.

## Prioridades de diseño

1. Hacer que cada morada tenga una regla jugable propia.
2. Garantizar que llave y salida siempre sean alcanzables.
3. Convertir reliquias en fragmentos de lore persistentes.
4. Añadir al menos tres patrones de enemigos:
   - duda: persigue lento;
   - sombra: se mueve rápido, poca vida;
   - bestia: guardián de puerta, mucha vida.
5. Añadir un jefe final: “La puerta falsa”, que se vence evitando atacar durante ventanas de silencio.

## Criterios de aceptación

- `docker compose run --rm gba-dev scripts/build.sh` genera `gba-back-home.gba`.
- `scripts/run-rom.sh` abre mGBA vía noVNC.
- La partida llega desde título hasta victoria sin cuelgues.
- No se superan 128 sprites visibles.
- El mapa nunca encierra la llave o la salida.
