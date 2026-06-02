# Diseño de gba-back-home

## Fantasía central

El protagonista no “explora un castillo” exterior: atraviesa una arquitectura interior. Cada mazmorra es una morada del regreso a casa. El objetivo no es conquistar, sino ordenar, recordar y volver.

## Estructura

Siete moradas:

1. **Conocimiento propio**: enemigos simples, muchas paredes, aprendizaje.
2. **Llamada**: más puertas y pasillos largos.
3. **Disciplina**: enemigos persistentes, menos recursos.
4. **Quietud**: santuarios más importantes, recursos de fervor.
5. **Confianza**: llaves alejadas, enemigos tipo sombra.
6. **Purificación**: enemigos fuertes, castigo por descuido.
7. **Hogar**: salida final, loop de victoria.

## Sistema procedural actual

- Se inicializa todo como muro.
- Un caminante aleatorio excava suelos desde el centro.
- Cada cierto número de pasos abre una sala pequeña.
- Se calcula la salida como el tile de suelo más lejano al origen.
- Se colocan santuario, puertas decorativas, llave, recursos y enemigos en suelos válidos.
- La clave de peregrino modifica la semilla.

## Combate

- **Ataque básico (A)**: golpea una casilla en la última dirección de movimiento.
- **Oración expansiva (B)**: consume 2 de fervor y daña enemigos a distancia Manhattan <= 2.
- Enemigos adyacentes dañan al jugador al tomar turno.
- Sombras se mueven más rápido; bestias hacen más daño.

## Objetos

- **Pan**: cura vida.
- **Candela**: recupera fervor.
- **Llave interior**: abre la salida de la morada actual.
- **Reliquia/memoria**: contador de lore, futuro desbloqueo narrativo.
- **Santuario**: restaura vida y fervor al pisarlo.

## Lore original de base

El texto evita sermonear: usa frases breves, casi litúrgicas, para que el jugador complete el sentido durante la exploración. El vocabulario base: puerta, morada, silencio, sombra, llave, casa, Padre, pan, luz, noche, gracia.

## Mejoras previstas

- Generador por grafo de habitaciones con camino crítico garantizado.
- Biomas visuales por morada.
- Enemigos con telemetría simple: patrulla, huida de luz, guardián de llave.
- Relicario con entradas desbloqueables.
- SRAM para perfil, estadísticas y progreso.
- Tilemap/background para liberar sprites.
- Música por morada con leitmotiv compartido.
