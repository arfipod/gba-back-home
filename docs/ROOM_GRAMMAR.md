# Room Grammar Notes

The current generator is still algorithmic rather than hand-authored, but future work should move toward composable room grammar. This document describes the intended shape.

## Critical Path

```text
Inicio seguro
  -> Sala de orientacion
  -> Bifurcacion
  -> Prueba de morada
  -> Llave interior
  -> Descanso / santuario
  -> Umbral
```

## Optional Branches

- Memoria: should be reachable but not mandatory.
- Silence door: should hide a resource or shortcut, never the required key.
- False door: should be placed as temptation or trap, not as unavoidable punishment.
- Shrine: should be close enough to matter but not placed after all danger.

## Room Archetypes

1. **Sala de limite**: teaches walls and movement.
2. **Cruce de llamada**: multiple exits; silence gives direction.
3. **Pasillo de sombra**: one fast enemy, enough space to retreat.
4. **Camara de vela**: resource in visible but risky position.
5. **Umbral falso**: false door near a plausible route.
6. **Sala de quietud**: silence tile and low immediate danger.
7. **Santuario estrecho**: resting is possible but can be interrupted.
8. **Guardian de llave**: beast or doubt near the key.
9. **Rama de memoria**: optional lore reward.
10. **Antesala final**: short read before exit/boss.

## Fairness Rules

- No enemy adjacent to start.
- No false door required for progression.
- Key and exit must be reachable by BFS.
- Shrine should not be the only safe answer to unavoidable damage.
- Every high-damage enemy needs a legible movement period.
- A room should ask one clear question, not five at once.
