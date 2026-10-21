#!/usr/bin/env python3
"""Pad a built GBA ROM to a valid power-of-two cartridge size."""

import pathlib
import sys

rom = pathlib.Path(sys.argv[1])
size = rom.stat().st_size
target_size = max(32_768, 1 << (size - 1).bit_length())
if target_size > 32 * 1024 * 1024:
    raise SystemExit(f"ROM exceeds the 32 MB GBA cartridge limit: {size} bytes")
if target_size != size:
    with rom.open("ab") as output:
        output.write(b"\xff" * (target_size - size))
print(f"Padded GBA ROM to {target_size} bytes")
