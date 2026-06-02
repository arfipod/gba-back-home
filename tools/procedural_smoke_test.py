#!/usr/bin/env python3
"""Host-side smoke test for the gba-back-home procedural model.

It mirrors the current high-level generator enough to catch broken reachability
rules before spending time in the GBA build. The C++ generator is still the game
authority; keep this test updated when the generator changes.
"""
from __future__ import annotations

import argparse
from collections import deque
from dataclasses import dataclass

MAP_W = 31
MAP_H = 23
MAX_ROOMS = 24

WALL = 0
FLOOR = 1
SILENCE = 2
DOOR_OPEN = 3
DOOR_FALSE = 4
DOOR_SILENCE = 5
SHRINE = 6
SHRINE_SPENT = 7
EXIT = 8

WALKABLE = {FLOOR, SILENCE, DOOR_OPEN, SHRINE, SHRINE_SPENT, EXIT}


@dataclass
class Lcg:
    state: int

    def u32(self) -> int:
        self.state = ((self.state * 1664525) + 1013904223) & 0xFFFFFFFF
        return self.state

    def range(self, limit: int) -> int:
        return ((self.u32() >> 16) % limit)


def clamp(v: int, lo: int, hi: int) -> int:
    return max(lo, min(hi, v))


def dist(ax: int, ay: int, bx: int, by: int) -> int:
    return abs(ax - bx) + abs(ay - by)


def carve_room(grid: list[list[int]], cx: int, cy: int, rx: int, ry: int) -> None:
    for y in range(cy - ry, cy + ry + 1):
        for x in range(cx - rx, cx + rx + 1):
            if 0 < x < MAP_W - 1 and 0 < y < MAP_H - 1:
                grid[y][x] = FLOOR


def carve_corridor(grid: list[list[int]], ax: int, ay: int, bx: int, by: int) -> None:
    x, y = ax, ay
    while x != bx:
        grid[y][x] = FLOOR
        x += 1 if bx > x else -1
    while y != by:
        grid[y][x] = FLOOR
        y += 1 if by > y else -1
    grid[y][x] = FLOOR


def bfs(grid: list[list[int]], start: tuple[int, int]) -> list[list[int]]:
    sx, sy = start
    out = [[-1 for _ in range(MAP_W)] for _ in range(MAP_H)]
    if grid[sy][sx] not in WALKABLE:
        return out
    q = deque([(sx, sy)])
    out[sy][sx] = 0
    while q:
        x, y = q.popleft()
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nx, ny = x + dx, y + dy
            if 0 <= nx < MAP_W and 0 <= ny < MAP_H and out[ny][nx] < 0 and grid[ny][nx] in WALKABLE:
                out[ny][nx] = out[y][x] + 1
                q.append((nx, ny))
    return out


def generate(seed: int, morada: int) -> tuple[list[list[int]], tuple[int, int], tuple[int, int], tuple[int, int]]:
    rng = Lcg(seed)
    grid = [[WALL for _ in range(MAP_W)] for _ in range(MAP_H)]
    px, py = MAP_W // 2, MAP_H // 2

    rooms: list[tuple[int, int]] = [(px, py)]
    carve_room(grid, px, py, 2, 1)
    cx, cy = px, py
    critical_rooms = min(6 + morada, 13)

    for _ in range(critical_rooms):
        if len(rooms) >= MAX_ROOMS:
            break
        nx = 3 + rng.range(MAP_W - 6)
        ny = 3 + rng.range(MAP_H - 6)
        if dist(nx, ny, cx, cy) < 6:
            nx = clamp(cx + (-7 if rng.range(2) == 0 else 7), 3, MAP_W - 4)
            ny = clamp(cy + (-5 if rng.range(2) == 0 else 5), 3, MAP_H - 4)
        carve_corridor(grid, cx, cy, nx, ny)
        carve_room(grid, nx, ny, 1 + rng.range(2), 1 + rng.range(2))
        rooms.append((nx, ny))
        cx, cy = nx, ny

    branch_count = 3 + morada // 2
    for _ in range(branch_count):
        if len(rooms) >= MAX_ROOMS:
            break
        bx0, by0 = rooms[rng.range(len(rooms))]
        bx = clamp(bx0 + (-6 if rng.range(2) == 0 else 6), 3, MAP_W - 4)
        by = clamp(by0 + (-4 if rng.range(2) == 0 else 4), 3, MAP_H - 4)
        carve_corridor(grid, bx0, by0, bx, by)
        carve_room(grid, bx, by, 1 + rng.range(2), 1)
        rooms.append((bx, by))

    d0 = bfs(grid, (px, py))
    ex, ey, best = px, py, -1
    for y in range(1, MAP_H - 1):
        for x in range(1, MAP_W - 1):
            if grid[y][x] == FLOOR and d0[y][x] > best:
                ex, ey, best = x, y, d0[y][x]
    grid[ey][ex] = EXIT

    de = bfs(grid, (ex, ey))
    kx, ky, best_score = px, py, -1
    for y in range(1, MAP_H - 1):
        for x in range(1, MAP_W - 1):
            if grid[y][x] == FLOOR and d0[y][x] >= 7 and de[y][x] >= 5:
                score = d0[y][x] + de[y][x] + rng.range(4)
                if score > best_score:
                    kx, ky, best_score = x, y, score

    if best_score < 0:
        fallback = max(
            ((d0[y][x], x, y) for y in range(1, MAP_H - 1) for x in range(1, MAP_W - 1)
             if grid[y][x] == FLOOR and (x, y) != (ex, ey)),
            default=(-1, px, py),
        )
        _, kx, ky = fallback

    return grid, (px, py), (kx, ky), (ex, ey)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--seeds", type=int, default=1000)
    parser.add_argument("--base", type=lambda s: int(s, 0), default=0xA7C00100)
    args = parser.parse_args()

    failures: list[str] = []
    for profile in range(4):
        for morada in range(1, 8):
            for index in range(args.seeds):
                seed = (args.base + profile * 0x1F123BB5 + index * 97 + morada * 131) & 0xFFFFFFFF
                grid, start, key, exit_ = generate(seed, morada)
                d = bfs(grid, start)
                if d[key[1]][key[0]] < 0:
                    failures.append(f"profile={profile} morada={morada} seed={seed:#x}: key unreachable")
                if d[exit_[1]][exit_[0]] < 0:
                    failures.append(f"profile={profile} morada={morada} seed={seed:#x}: exit unreachable")
                if d[key[1]][key[0]] < 5:
                    failures.append(f"profile={profile} morada={morada} seed={seed:#x}: key too close")
                if failures:
                    break
            if failures:
                break
        if failures:
            break

    if failures:
        print("FAILED")
        for item in failures[:20]:
            print(item)
        return 1

    print(f"OK: validated {args.seeds} seeds x 7 moradas x 4 profiles")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
