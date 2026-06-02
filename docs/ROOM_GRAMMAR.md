# Room Grammar Notes

The current generator is still algorithmic rather than hand-authored, but future work should move toward composable room grammar. This document describes the intended shape.

## Critical Path

```text
Safe start
  -> Orientation room
  -> Branch
  -> Dwelling trial
  -> Inner key
  -> Rest / shrine
  -> Threshold
```

## Optional Branches

- Memory: should be reachable but not mandatory.
- Silence door: should hide a resource or shortcut, never the required key.
- False door: should be placed as temptation or trap, not as unavoidable punishment.
- Shrine: should be close enough to matter but not placed after all danger.

## Room Archetypes

1. **Limit room**: teaches walls and movement.
2. **Calling crossroads**: multiple exits; silence gives direction.
3. **Shadow corridor**: one fast enemy, enough space to retreat.
4. **Candle chamber**: resource in a visible but risky position.
5. **False threshold**: false door near a plausible route.
6. **Stillness room**: silence tile and low immediate danger.
7. **Narrow shrine**: resting is possible but can be interrupted.
8. **Key guardian**: beast or doubt near the key.
9. **Memory branch**: optional lore reward.
10. **Final antechamber**: short read before exit/boss.

## Fairness Rules

- No enemy adjacent to start.
- No false door required for progression.
- Key and exit must be reachable by BFS.
- Shrine should not be the only safe answer to unavoidable damage.
- Every high-damage enemy needs a legible movement period.
- A room should ask one clear question, not five at once.
