# Balance Notes

## Intended Run Length

- Single dwelling: 2-4 minutes.
- Vertical slice: 3-5 minutes.
- Full run: 20-30 minutes.

## Starting Values

- HP: 9.
- Light/fervor: 4 base.
- Faith max light: 10.
- Peace starts with +1 light.

## Resource Economy

- Prayer costs 2 light.
- Bread heals 2 HP.
- Candle restores 2 light.
- Shrine restores full HP/light only after three silence turns and then becomes spent.

## Enemy Pressure

- Dwelling I should rarely show more than two threats at once.
- Dwelling III begins serious resource pressure.
- Dwelling V introduces more false doors and haste enemies.
- Dwelling VI uses beast guardians as local problems, not global chasers.

## Design Tests

Ask these questions during playtest:

1. Did the player understand why they took damage?
2. Did silence feel like a real option?
3. Did prayer feel useful beyond damage?
4. Did false doors feel readable after the first encounter?
5. Did the shrine create a decision rather than a free refill?
6. Did each dwelling feel mechanically distinct?
7. Did the boss teach discernment instead of DPS?

## Quick Tuning Levers

- `enemy_count = 2 + _dwelling` in `src/gbh_dungeon_generation.cpp`.
- Prayer cost and range in `prayer_action()` / `prayer_range()`.
- Shrine channel length in `silence_action()`.
- False door count in `generate_dungeon()`.
- Enemy move periods in `enemy_move_period()`.
- Boss cycle in `boss_silence_window()`.
