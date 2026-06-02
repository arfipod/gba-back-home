# gba-back-home Design

## Vision

El juego no trata de conquistar un castillo, sino de **volver**. La arquitectura no es exterior: cada mapa representa una morada interior. El jugador avanza, calla, discierne, recuerda y atraviesa. Matar enemigos puede ser util, pero no debe ser siempre el camino optimo.

## Pilares

1. **Volver, no conquistar**: la meta es reconocer el umbral verdadero.
2. **La luz es recurso, informacion y combate**: la luz/fervor sirve para orar, pacificar y discernir.
3. **Cada morada cambia una regla**: no solo suben los numeros.
4. **La mazmorra debe tener intencion**: proceduralidad con salas, ramas, llave, santuario, memorias y salida.
5. **La derrota ensena**: caer no es estar perdido.

## Estructura de run

1. Seleccion de peregrino.
2. Pantalla de lore/regla de morada.
3. Dungeon procedural con objetivo de llave + umbral.
4. Memoria opcional.
5. Transicion a siguiente morada.
6. En la morada VII, boss **La Puerta Falsa**.
7. Victoria: regresar a casa.

## Peregrinos

- **FE**: aumenta la luz maxima y el alcance de oracion.
- **ESPERANZA**: una vez por run, al caer, vuelve a 1 HP.
- **CARIDAD**: la oracion pacifica enemigos no-bestia y suma contador de paz.
- **PAZ**: reduce el rango de deteccion enemigo y empieza con mas luz.

## Moradas

### I. Conocimiento propio

- Introduce movimiento, golpe, luz, llave y umbral.
- Enemigos principales: Duda.
- Regla emocional: los muros ensenan limite.

### II. Llamada

- `R` da una senal hacia llave o salida.
- Mapas con mas corredores.
- Enemigo sugerido: Ruido, que apaga luz cercana.

### III. Disciplina

- Menos recursos.
- Sombras mas frecuentes.
- La mala economia de luz castiga.

### IV. Quietud

- `R` en seguridad recupera luz.
- Tiles de silencio y puertas de silencio aparecen con mas frecuencia.
- Esperar se vuelve una accion positiva.

### V. Confianza

- Mas puertas falsas.
- Enemigos de prisa que castigan moverse sin callar.
- El jugador aprende a discernir antes de golpear.

### VI. Purificacion

- Bestias guardianas cerca de umbrales.
- Danio mas alto, pero telegráfico por rol: la bestia es lenta.
- El santuario se vuelve una decision de riesgo.

### VII. Hogar

- El umbral lleva a **La Puerta Falsa**.
- El boss se supera con silencio/oracion durante ventanas correctas, no golpeando.

## Combate

### Golpe basico: Resolucion

- Boton: `A`.
- Golpea una casilla en la ultima direccion.
- Si el jugador intenta moverse hacia un enemigo, tambien golpea.
- Sirve contra Duda, Sombra y Prisa.
- No discierne puertas falsas.

### Oracion

- Boton: `B`.
- Consume 2 de luz.
- Afecta enemigos a rango Manhattan 2; FE aumenta a 3.
- Con CARIDAD, pacifica enemigos no-bestia.
- Tambien revela/desactiva puertas falsas y abre puertas de silencio cercanas.

### Silencio

- Boton: `R`.
- Consume un turno.
- Abre puertas de silencio al estar orientado hacia ellas.
- En santuario, requiere tres turnos para restaurar HP/luz; el santuario se agota.
- En morada IV o tiles de silencio, si no hay enemigo adyacente, recupera luz.
- En moradas de llamada/confianza, da senal hacia llave o salida.

## Enemigos

### Duda

- Lenta, directa, 1 HP.
- Ensena posicionamiento basico.

### Sombra

- Rapida, 2 HP.
- Evita santuarios.
- Castiga pasillos estrechos.

### Ruido

- Erratica.
- Si esta cerca, apaga luz.
- La oracion/pacificacion es su contrajuego natural.

### Prisa

- Rapida si el jugador no ha usado silencio.
- Se ralentiza cuando el jugador espera/calla.
- Ensenanza: no todo se resuelve corriendo.

### Bestia

- Lenta, 3 HP, 2 de dano.
- Puede actuar como guardiana de salida.
- El contrajuego es posicionamiento, luz y paciencia.

## Puertas

- **Umbral final**: requiere llave interior.
- **Puerta falsa**: parece salida/atajo, pero dana y puede invocar sombra/ruido. La oracion la discierne.
- **Puerta de silencio**: se abre con `R` orientado hacia ella o con oracion cercana. Suele esconder un pequeno recurso.
- **Puerta abierta**: paso normal.

## Objetos

- **Pan**: cura HP.
- **Vela**: restaura luz.
- **Llave interior**: abre el umbral de la morada.
- **Memoria**: desbloquea una frase persistente durante la run.
- **Santuario**: restauracion completa tras tres turnos de silencio; se agota al usarse.

## Memorias

Las memorias son el corazon narrativo. No son loot de poder; son comprensiones breves.

1. No estaba lejos.
2. La voz no grita.
3. La regla no encierra.
4. Callar tambien anda.
5. La luz basta cerca.
6. El deseo se limpia.
7. Era hondo.

## Boss: La Puerta Falsa

- Aparece tras la morada VII.
- Tiene ciclos de ruido y ventanas de silencio.
- `A` la alimenta y hiere al jugador.
- `R` durante una ventana de silencio pone un sello.
- `B` durante una ventana de silencio tambien pone un sello, consumiendo luz.
- Tres sellos abren el regreso.

## Proceduralidad

El generador actual usa salas y corredores conectados. Luego coloca:

1. Inicio seguro.
2. Salida mas lejana por BFS.
3. Llave en posicion significativa, separada de inicio/salida.
4. Santuario.
5. Tiles de silencio.
6. Memoria opcional.
7. Recursos.
8. Puertas falsas y de silencio en paredes adyacentes.
9. Enemigos segun morada.
10. Bestia guardiana en moradas altas.

Principio: la aleatoriedad debe crear variacion, no injusticia.

## Lenguaje

El texto debe ser corto, sobrio y no explicativo. Evitar sermonear. Usar vocabulario recurrente: puerta, umbral, morada, silencio, sombra, llave, casa, Padre, pan, luz, noche, gracia, memoria.

## Proximas decisiones de diseno

- Definir si las memorias modifican mecanicas o solo final/lore.
- Crear room grammar con salas prefabricadas.
- Introducir mapa abstracto/revelacion limitada.
- Convertir las puertas en puzzles por morada.
- Balancear duracion: 2-4 minutos por morada, 20-30 minutos por run completa.
