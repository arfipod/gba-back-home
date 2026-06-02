# Emulator Screenshots And Visual Checks

This guide documents the repeatable process used to capture README screenshots and to perform quick visual checks of the GBA ROM in the bundled Docker/noVNC environment.

Use it when an agent needs to update documentation screenshots, verify that the ROM actually renders in mGBA, or drive the title/menu flow without manual browser interaction.

## Required Tools

- Docker Compose service: `gba-dev`
- ROM: `gba-back-home.gba`
- noVNC: `http://localhost:6080/vnc.html`
- Password: `gba`
- Browser automation command, when available: `agent-browser`
- X automation inside the container: `xdotool`

`xdotool` is installed by the project Dockerfile. If an old container was created before that package was added, rebuild the image:

```bash
docker compose build gba-dev
docker compose up -d gba-dev
```

## Build The ROM

Always build before capturing screenshots:

```bash
docker compose up -d gba-dev
docker compose exec gba-dev scripts/doctor.sh
docker compose exec gba-dev scripts/build.sh
```

## Launch mGBA For Captures

For automated captures, prefer the SDL mGBA binary with software rendering and dummy audio. This avoids black noVNC captures that can happen with the default Qt/OpenGL path.

```bash
docker compose exec -T gba-dev sh -lc '
for p in $(pgrep -x mgba) $(pgrep -x mgba-qt); do
    kill "$p" 2>/dev/null || true
done
sleep 1
DISPLAY=:1 SDL_AUDIODRIVER=dummy nohup /usr/games/mgba -3 -C videoRenderer=software gba-back-home.gba >/tmp/mgba-screenshots.log 2>&1 &
sleep 4
pgrep -af "[m]gba"
'
```

Notes:

- `-3` starts mGBA at a larger scale that is readable in documentation.
- `SDL_AUDIODRIVER=dummy` prevents audio initialization from blocking or behaving differently under Xvfb.
- `-C videoRenderer=software` makes noVNC canvas captures reliable.
- Defunct `mgba` or `mgba-qt` processes can appear after previous experiments. They are harmless zombies owned by PID 1, but there should be only one visible mGBA window.

Check the visible window:

```bash
docker compose exec -T gba-dev sh -lc '
DISPLAY=:1 xdotool search --onlyvisible --name mGBA getwindowpid %@ getwindowname %@ 2>/dev/null || true
'
```

## Connect noVNC

Open and authenticate:

```bash
agent-browser open http://localhost:6080/vnc.html
agent-browser snapshot -i
agent-browser fill @e1 "gba"
agent-browser click @e2
```

The refs can change. If `@e1` and `@e2` do not match, run `agent-browser snapshot -i` again and use the current password field and send button refs.

## Drive The Emulator

Send keys directly to the mGBA X window from inside the container. This is more reliable than sending keys through the browser/noVNC canvas.

```bash
docker compose exec -T gba-dev sh -lc '
wid=$(DISPLAY=:1 xdotool search --onlyvisible --name mGBA | head -n 1)
DISPLAY=:1 xdotool windowactivate "$wid" key BackSpace
'
```

Useful key mapping for this project:

| Action | X key used in capture workflow |
|---|---|
| SELECT / language toggle | `BackSpace` |
| Confirm / enter next screen | `Return` |
| Move | `Up`, `Down`, `Left`, `Right` |

Examples:

```bash
# Toggle title screen language.
docker compose exec -T gba-dev sh -lc '
wid=$(DISPLAY=:1 xdotool search --onlyvisible --name mGBA | head -n 1)
DISPLAY=:1 xdotool windowactivate "$wid" key BackSpace
sleep 1
'

# Confirm twice: title -> pilgrim or pilgrim -> dwelling, depending on current state.
docker compose exec -T gba-dev sh -lc '
wid=$(DISPLAY=:1 xdotool search --onlyvisible --name mGBA | head -n 1)
DISPLAY=:1 xdotool windowactivate "$wid" key Return
sleep 1
DISPLAY=:1 xdotool key Return
sleep 2
'

# Move around a bit in the dungeon.
docker compose exec -T gba-dev sh -lc '
wid=$(DISPLAY=:1 xdotool search --onlyvisible --name mGBA | head -n 1)
DISPLAY=:1 xdotool windowactivate "$wid"
for k in Right Right Right Down Down Left Left Up; do
    DISPLAY=:1 xdotool key "$k"
    sleep 0.25
done
sleep 1
'
```

## Capture Images

Capture the noVNC canvas:

```bash
agent-browser screenshot canvas docs/screenshots/title-es-vnc.png
```

Recommended capture sequence for README-style documentation:

```bash
# 1. Spanish title, immediately after launching mGBA.
agent-browser screenshot canvas docs/screenshots/title-es-vnc.png

# 2. English title.
docker compose exec -T gba-dev sh -lc '
wid=$(DISPLAY=:1 xdotool search --onlyvisible --name mGBA | head -n 1)
DISPLAY=:1 xdotool windowactivate "$wid" key BackSpace
sleep 1
'
agent-browser screenshot canvas docs/screenshots/title-en-vnc.png

# 3. Pilgrim selection.
docker compose exec -T gba-dev sh -lc '
wid=$(DISPLAY=:1 xdotool search --onlyvisible --name mGBA | head -n 1)
DISPLAY=:1 xdotool windowactivate "$wid" key Return
sleep 1
'
agent-browser screenshot canvas docs/screenshots/pilgrim-selection-en-vnc.png

# 4. Dwelling intro.
docker compose exec -T gba-dev sh -lc '
wid=$(DISPLAY=:1 xdotool search --onlyvisible --name mGBA | head -n 1)
DISPLAY=:1 xdotool windowactivate "$wid" key Return
sleep 1
'
agent-browser screenshot canvas docs/screenshots/dwelling-intro-en-vnc.png

# 5. Dungeon exploration.
docker compose exec -T gba-dev sh -lc '
wid=$(DISPLAY=:1 xdotool search --onlyvisible --name mGBA | head -n 1)
DISPLAY=:1 xdotool windowactivate "$wid" key Return
sleep 2
for k in Right Right Right Down Down Left Left Up; do
    DISPLAY=:1 xdotool key "$k"
    sleep 0.25
done
sleep 1
'
agent-browser screenshot canvas docs/screenshots/dungeon-exploration-en-vnc.png
```

## Crop Documentation Screenshots

The raw noVNC canvas includes the desktop, mGBA title bar, and unused noVNC area. For the current 1024x768 agent-browser canvas and mGBA `-3` launch, crop the visible game frame with this box:

```text
left=153, top=164, right=873, bottom=633
```

Generate final README images:

```bash
python3 - <<'PY'
from pathlib import Path
from PIL import Image

root = Path("docs/screenshots")
crop_box = (153, 164, 873, 633)
images = {
    "title-es-vnc.png": "title-es.png",
    "title-en-vnc.png": "title-en.png",
    "pilgrim-selection-en-vnc.png": "pilgrim-selection-en.png",
    "dwelling-intro-en-vnc.png": "dwelling-intro-en.png",
    "dungeon-exploration-en-vnc.png": "dungeon-exploration-en.png",
}

for source, dest in images.items():
    img = Image.open(root / source).convert("RGB")
    img.crop(crop_box).save(root / dest, optimize=True)
    print(dest)
PY
```

Remove raw VNC captures after checking the cropped images:

```bash
rm -f docs/screenshots/*-vnc.png
```

## Verification Checklist

After updating screenshots or visual docs:

```bash
docker compose exec -T gba-dev scripts/build.sh
python3 tools/procedural_smoke_test.py --seeds 1000
find docs/screenshots -maxdepth 1 -type f -printf "%f\n" | sort
rg -n "docs/screenshots|Screenshots" README.md
```

For visual inspection, open the generated PNGs or use the local image viewer available to the agent.

## Troubleshooting

### noVNC Capture Is Black Or White

Relaunch mGBA with SDL software rendering:

```bash
DISPLAY=:1 SDL_AUDIODRIVER=dummy /usr/games/mgba -3 -C videoRenderer=software gba-back-home.gba
```

Avoid relying on the default Qt/OpenGL window when the goal is an automated screenshot.

### Browser Key Presses Do Not Affect The Game

Send keys with `xdotool` to the mGBA window instead of pressing keys through noVNC:

```bash
docker compose exec -T gba-dev sh -lc '
wid=$(DISPLAY=:1 xdotool search --onlyvisible --name mGBA | head -n 1)
DISPLAY=:1 xdotool windowactivate "$wid" key Return
'
```

### Multiple mGBA Windows Are Visible

Close old emulator windows before launching a new capture session:

```bash
docker compose exec -T gba-dev sh -lc '
for p in $(pgrep -x mgba) $(pgrep -x mgba-qt); do
    kill "$p" 2>/dev/null || true
done
sleep 1
'
```

If `ps` shows old entries as `Z` or `<defunct>`, they are zombies and do not have active windows. Focus the visible window returned by `xdotool search --onlyvisible --name mGBA`.

### Crop No Longer Matches

The crop box depends on the browser canvas size and mGBA window placement. If it changes, capture one raw screenshot and find the game background bounding box or adjust the crop manually. Keep the final README images free of the mGBA title bar and noVNC desktop.
