# gba-back-home Design

## Vision

The game is not about conquering a castle, but about **returning**. Its architecture is inward: each map represents an interior dwelling. The player advances, keeps silence, discerns, remembers, and crosses thresholds. Killing enemies can be useful, but it should not always be the optimal path.

## Pillars

1. **Return, do not conquer**: the goal is to recognize the true threshold.
2. **Light is resource, information, and combat**: light/fervor powers prayer, pacification, and discernment.
3. **Each dwelling changes a rule**: difficulty should not come only from bigger numbers.
4. **The dungeon must have intention**: procedural generation includes rooms, branches, key, shrine, memories, and exit.
5. **Defeat teaches**: falling does not mean being lost.

## Run Structure

1. Select a pilgrim.
2. Read the dwelling lore/rule screen.
3. Explore a procedural dungeon with a key-and-threshold objective.
4. Collect an optional memory.
5. Transition to the next dwelling.
6. In dwelling VII, face **The False Door**.
7. Win by returning home.

## Pilgrims

- **Faith**: increases maximum light and prayer range.
- **Hope**: once per run, falling restores the player to 1 HP.
- **Charity**: prayer pacifies non-beast enemies and increases the mercy counter.
- **Peace**: reduces enemy detection range and starts with more light.

## Dwellings

### I. Self-Knowledge

- Introduces movement, strike, light, key, and threshold.
- Main enemy: Doubt.
- Emotional rule: walls teach limits.

### II. Calling

- `R` gives a signal toward the key or exit.
- Maps lean toward more corridors.
- Suggested enemy pressure: Noise, which dims nearby light.

### III. Discipline

- Fewer resources.
- Shadows become more common.
- Poor light economy is punished.

### IV. Stillness

- `R` restores light when the player is safe.
- Silence tiles and silence doors appear more often.
- Waiting becomes a positive action.

### V. Trust

- More false doors.
- Haste enemies punish movement without silence.
- The player learns to discern before striking.

### VI. Purification

- Beast guardians near thresholds.
- Higher damage, but telegraphed by role: beasts are slow.
- The shrine becomes a risk decision.

### VII. Home

- The threshold leads to **The False Door**.
- The boss is overcome with silence/prayer during correct windows, not by striking.

## Combat

### Basic Strike: Resolve

- Button: `A`.
- Hits one tile in the last faced direction.
- Moving into an enemy also attacks it.
- Works against Doubt, Shadow, and Haste.
- Does not discern false doors.

### Prayer

- Button: `B`.
- Costs 2 light.
- Affects enemies in Manhattan range 2; Faith increases this to 3.
- With Charity, pacifies non-beast enemies.
- Also reveals/disables false doors and opens nearby silence doors.

### Silence

- Button: `R`.
- Consumes one turn.
- Opens silence doors when facing them.
- At shrines, requires three turns to restore HP/light; the shrine is then spent.
- In dwelling IV or on silence tiles, restores light if no enemy is adjacent.
- In calling/trust dwellings, gives a signal toward the key or exit.

## Enemies

### Doubt

- Slow, direct, 1 HP.
- Teaches basic positioning.

### Shadow

- Fast, 2 HP.
- Avoids shrines.
- Punishes narrow corridors.

### Noise

- Erratic.
- Dims light when nearby.
- Prayer/pacification is its natural counterplay.

### Haste

- Fast if the player has not used silence.
- Slows down when the player waits.
- Lesson: not everything is solved by rushing.

### Beast

- Slow, 3 HP, 2 damage.
- Can act as an exit guardian.
- Counterplay is positioning, light, and patience.

## Doors

- **Final threshold**: requires the inner key.
- **False door**: looks like an exit or shortcut, but hurts the player and can summon shadow/noise. Prayer discerns it.
- **Silence door**: opens with `R` while facing it or with nearby prayer. Usually hides a small resource.
- **Open door**: normal passage.

## Objects

- **Bread**: heals HP.
- **Candle**: restores light.
- **Inner key**: opens the dwelling threshold.
- **Memory**: unlocks a short phrase during the run.
- **Shrine**: fully restores HP/light after three turns of silence; becomes spent after use.

## Memories

Memories are the narrative core. They are not power loot; they are brief understandings.

1. It was not far.
2. The voice is not loud.
3. The rule does not cage.
4. Silence also walks.
5. Near light is enough.
6. Desire is cleansed.
7. It was deep.

## Boss: The False Door

- Appears after dwelling VII.
- Alternates between noise and silence windows.
- `A` feeds it and hurts the player.
- `R` during a silence window adds a seal.
- `B` during a silence window also adds a seal, consuming light.
- Three seals open the return.

## Procedural Generation

The current generator uses connected rooms and corridors. It then places:

1. Safe start.
2. Farthest exit by BFS.
3. Meaningfully separated key.
4. Shrine.
5. Silence tiles.
6. Optional memory.
7. Resources.
8. False doors and silence doors in adjacent walls.
9. Enemies by dwelling.
10. Beast guardian in higher dwellings.

Principle: randomness should create variation, not unfairness.

## Language

In-game text must stay short, restrained, and non-expository. Avoid sermonizing. Reuse a stable vocabulary: door, threshold, dwelling, silence, shadow, key, house, Father, bread, light, night, grace, memory.

The game ships with Spanish and English text. Documentation remains English.

## Upcoming Design Decisions

- Decide whether memories modify mechanics or remain final/lore context.
- Create room grammar with authored room fragments.
- Introduce an abstract map or limited revelation.
- Turn doors into dwelling-specific puzzles.
- Balance length: 2-4 minutes per dwelling, 20-30 minutes for a full run.
