#!/usr/bin/env python3
"""Regenerates placeholder assets for gba-back-home.

The zip already includes generated graphics/audio. Keep this file as a stable
entry point for future Codex work; replace internals with aseprite/usenti/export
pipeline as art matures.
"""
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
print("Assets are already checked in under graphics/ and audio/.")
print("For production art, export indexed BMP files with transparent color at palette index 0.")
print("Project root:", ROOT)
