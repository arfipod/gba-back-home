#!/usr/bin/env python3
"""Regenerate placeholder indexed 4bpp BMP sprites for gba-back-home.

This is intentionally simple. Production art should come from a real indexed
palette tool, but this script keeps the repository reproducible for Codex.
"""
from pathlib import Path
from PIL import Image, ImageDraw
import json
import struct

ROOT = Path(__file__).resolve().parents[1]
GRAPHICS = ROOT / "graphics"
GRAPHICS.mkdir(exist_ok=True)

PALETTE = [
    (2, 1, 4), (18, 15, 28), (42, 38, 58), (76, 71, 91),
    (206, 176, 82), (236, 229, 190), (72, 112, 176), (158, 58, 62),
    (64, 154, 101), (128, 128, 142), (220, 124, 58), (10, 9, 14),
    (57, 168, 172), (162, 88, 170), (244, 205, 112), (101, 48, 86),
]


def write_bmp4(path: Path, img: Image.Image) -> None:
    """Write a palettized image as Windows BMP 4bpp with 16 palette entries."""
    img = img.convert("P")
    w, h = img.size
    pix = img.load()
    row_bytes = (w + 1) // 2
    row_padded = (row_bytes + 3) & ~3
    offset = 14 + 40 + 16 * 4
    image_size = row_padded * h
    file_size = offset + image_size
    with path.open("wb") as f:
        f.write(b"BM")
        f.write(struct.pack("<IHHI", file_size, 0, 0, offset))
        f.write(struct.pack("<IIIHHIIIIII", 40, w, h, 1, 4, 0, image_size, 2835, 2835, 16, 16))
        for r, g, b in PALETTE:
            f.write(struct.pack("BBBB", b, g, r, 0))
        for y in range(h - 1, -1, -1):
            row = bytearray()
            for x in range(0, w, 2):
                a = pix[x, y] & 0x0F
                b = (pix[x + 1, y] & 0x0F) if x + 1 < w else 0
                row.append((a << 4) | b)
            row.extend(b"\x00" * (row_padded - len(row)))
            f.write(row)


def save_sprite(name: str, w: int, h: int, draw_fn) -> None:
    img = Image.new("P", (w, h), 0)
    flat_palette = []
    for rgb in PALETTE:
        flat_palette += list(rgb)
    flat_palette += [0] * (768 - len(flat_palette))
    img.putpalette(flat_palette)
    draw_fn(img, ImageDraw.Draw(img))
    write_bmp4(GRAPHICS / f"{name}.bmp", img)
    with (GRAPHICS / f"{name}.json").open("w") as f:
        json.dump({"type": "sprite", "bpp_mode": "bpp_4", "colors_count": 16, "width": w, "height": h}, f, indent=4)
        f.write("\n")


def rect(d, xy, c):
    d.rectangle(xy, fill=c)


def line(d, xy, c, width=1):
    d.line(xy, fill=c, width=width)


def player(direction):
    def fn(_img, d):
        rect(d, (6, 2, 9, 4), 5)
        rect(d, (5, 5, 10, 10), 6)
        rect(d, (4, 10, 11, 13), 1)
        rect(d, (6, 12, 7, 15), 3)
        rect(d, (9, 12, 10, 15), 3)
        rect(d, (7, 6, 8, 7), 14)
        if direction == "down":
            rect(d, (6, 3, 6, 3), 11); rect(d, (9, 3, 9, 3), 11); rect(d, (7, 0, 8, 1), 14)
        elif direction == "up":
            rect(d, (6, 1, 9, 2), 3); line(d, (7, 0, 8, 0), 14)
        elif direction == "left":
            rect(d, (5, 3, 5, 3), 11); line(d, (2, 7, 4, 7), 14); rect(d, (3, 6, 3, 8), 14)
        else:
            rect(d, (10, 3, 10, 3), 11); line(d, (11, 7, 13, 7), 14); rect(d, (12, 6, 12, 8), 14)
    return fn


def tile_floor(_img, d):
    rect(d, (0, 0, 15, 15), 2); rect(d, (0, 0, 15, 0), 1); rect(d, (0, 15, 15, 15), 1)


def tile_wall(_img, d):
    rect(d, (0, 0, 15, 15), 3)
    for y in (3, 7, 11): line(d, (0, y, 15, y), 9)
    for x in (4, 10): line(d, (x, 0, x, 15), 9)


def tile_door(_img, d):
    rect(d, (0, 0, 15, 15), 1); rect(d, (3, 1, 12, 15), 4); rect(d, (5, 3, 10, 15), 3); rect(d, (9, 8, 10, 9), 14)


def tile_exit(_img, d):
    rect(d, (0, 0, 15, 15), 1); rect(d, (2, 2, 13, 13), 14); rect(d, (4, 4, 11, 11), 5); rect(d, (6, 6, 9, 9), 12)


def tile_shrine(_img, d):
    rect(d, (0, 0, 15, 15), 1); rect(d, (1, 1, 14, 14), 2); rect(d, (6, 3, 9, 10), 4); rect(d, (4, 10, 11, 12), 4); rect(d, (7, 4, 8, 5), 14)


def tile_silence(_img, d):
    rect(d, (0, 0, 15, 15), 1); rect(d, (1, 1, 14, 14), 2)
    for y in (4, 8, 12): line(d, (3, y, 12, y), 12)
    rect(d, (7, 7, 8, 8), 5)


def tile_door_false(_img, d):
    rect(d, (0, 0, 15, 15), 1); rect(d, (3, 1, 12, 15), 15); rect(d, (4, 2, 11, 15), 7)
    line(d, (4, 2, 11, 15), 11); line(d, (11, 2, 4, 15), 11); rect(d, (9, 8, 10, 9), 14)


def tile_door_silence(_img, d):
    rect(d, (0, 0, 15, 15), 1); rect(d, (3, 1, 12, 15), 3); rect(d, (4, 2, 11, 15), 12)
    for y in (5, 8, 11): line(d, (5, y, 10, y), 5)
    rect(d, (7, 3, 8, 4), 14)


def tile_shrine_spent(_img, d):
    rect(d, (0, 0, 15, 15), 1); rect(d, (1, 1, 14, 14), 2); rect(d, (6, 3, 9, 10), 9); rect(d, (4, 10, 11, 12), 9); line(d, (5, 13, 10, 13), 3)


def enemy_doubt(_img, d):
    rect(d, (0, 0, 15, 15), 0); rect(d, (5, 4, 10, 12), 13); rect(d, (6, 6, 6, 6), 11); rect(d, (9, 6, 9, 6), 11); line(d, (4, 13, 11, 13), 15)


def enemy_shadow(_img, d):
    rect(d, (0, 0, 15, 15), 0); rect(d, (5, 3, 10, 13), 11); rect(d, (4, 7, 11, 12), 15); rect(d, (6, 5, 6, 5), 7); rect(d, (9, 5, 9, 5), 7)


def enemy_beast(_img, d):
    rect(d, (0, 0, 15, 15), 0); rect(d, (3, 5, 12, 13), 7); rect(d, (5, 2, 10, 7), 10); rect(d, (3, 2, 4, 4), 14); rect(d, (11, 2, 12, 4), 14); rect(d, (6, 5, 6, 5), 11); rect(d, (9, 5, 9, 5), 11)


def enemy_noise(_img, d):
    rect(d, (0, 0, 15, 15), 0); rect(d, (5, 4, 10, 11), 13); rect(d, (4, 6, 11, 9), 15)
    line(d, (2, 4, 0, 2), 13); line(d, (13, 4, 15, 2), 13); line(d, (2, 12, 0, 14), 13); line(d, (13, 12, 15, 14), 13)


def enemy_haste(_img, d):
    rect(d, (0, 0, 15, 15), 0); rect(d, (6, 3, 10, 11), 10); rect(d, (4, 6, 7, 13), 14); rect(d, (8, 6, 12, 13), 7)
    line(d, (1, 5, 5, 5), 14); line(d, (0, 8, 5, 8), 14); line(d, (2, 11, 6, 11), 14)


def boss_false_door(_img, d):
    rect(d, (0, 0, 31, 31), 0); rect(d, (5, 1, 26, 31), 15); rect(d, (7, 3, 24, 31), 7); rect(d, (9, 5, 22, 29), 11)
    line(d, (9, 5, 22, 29), 14, 2); line(d, (22, 5, 9, 29), 14, 2); rect(d, (13, 12, 14, 13), 5); rect(d, (18, 12, 19, 13), 5); rect(d, (15, 20, 17, 22), 13)


def item_bread(_img, d):
    rect(d, (0, 0, 15, 15), 0); rect(d, (3, 6, 12, 11), 10); rect(d, (4, 5, 11, 8), 14); line(d, (5, 9, 10, 9), 4)


def item_candle(_img, d):
    rect(d, (0, 0, 15, 15), 0); rect(d, (6, 7, 9, 14), 5); rect(d, (7, 3, 8, 6), 14); rect(d, (6, 4, 9, 5), 10)


def item_key(_img, d):
    rect(d, (0, 0, 15, 15), 0); rect(d, (3, 5, 7, 9), 14); rect(d, (4, 6, 6, 8), 0); line(d, (7, 7, 13, 7), 14, 2); rect(d, (11, 8, 12, 10), 14)


def item_relic(_img, d):
    rect(d, (0, 0, 15, 15), 0); rect(d, (5, 2, 10, 13), 4); rect(d, (6, 3, 9, 12), 14); rect(d, (7, 5, 8, 6), 5); line(d, (4, 14, 11, 14), 3)


def attack_wave(_img, d):
    rect(d, (0, 0, 31, 31), 0)
    for box, c in [((5, 5, 26, 26), 12), ((9, 9, 22, 22), 14), ((13, 13, 18, 18), 5)]:
        d.ellipse(box, outline=c)


SPRITES = {
    "tile_floor": (16, 16, tile_floor), "tile_wall": (16, 16, tile_wall), "tile_door": (16, 16, tile_door),
    "tile_exit": (16, 16, tile_exit), "tile_shrine": (16, 16, tile_shrine), "tile_silence": (16, 16, tile_silence),
    "tile_door_false": (16, 16, tile_door_false), "tile_door_silence": (16, 16, tile_door_silence), "tile_shrine_spent": (16, 16, tile_shrine_spent),
    "player_down": (16, 16, player("down")), "player_up": (16, 16, player("up")), "player_left": (16, 16, player("left")), "player_right": (16, 16, player("right")),
    "enemy_doubt": (16, 16, enemy_doubt), "enemy_shadow": (16, 16, enemy_shadow), "enemy_beast": (16, 16, enemy_beast), "enemy_noise": (16, 16, enemy_noise), "enemy_haste": (16, 16, enemy_haste),
    "boss_false_door": (32, 32, boss_false_door),
    "item_bread": (16, 16, item_bread), "item_candle": (16, 16, item_candle), "item_key": (16, 16, item_key), "item_relic": (16, 16, item_relic),
    "attack_wave": (32, 32, attack_wave),
}


def main() -> None:
    for name, (w, h, fn) in SPRITES.items():
        save_sprite(name, w, h, fn)
    print(f"Generated {len(SPRITES)} placeholder sprites in {GRAPHICS}")


if __name__ == "__main__":
    main()
