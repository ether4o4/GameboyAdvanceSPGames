#!/usr/bin/env python3
import pathlib
import sys

rom = pathlib.Path(sys.argv[1])
data = rom.read_bytes()
if not 32_768 <= len(data) <= 32 * 1024 * 1024:
    raise SystemExit(f"unexpected GBA ROM size: {len(data)} bytes")
if len(data) & (len(data) - 1):
    raise SystemExit(f"GBA ROM size should be a power of two: {len(data)} bytes")
if data[3] != 0xEA:
    raise SystemExit(f"GBA cartridge entry should begin with ARM branch, found top byte 0x{data[3]:02x}")
title = data[0xA0:0xAC].decode("ascii", errors="replace").rstrip("\0 ")
code = data[0xAC:0xB0].decode("ascii", errors="replace")
maker = data[0xB0:0xB2].decode("ascii", errors="replace")
if title != "TRASHLIDDASH":
    raise SystemExit(f"ROM header title mismatch: {title!r}")
if code != "TLD1" or maker != "WT":
    raise SystemExit(f"ROM header code mismatch: {code!r} / {maker!r}")
if data[0xB2] != 0x96:
    raise SystemExit(f"ROM fixed header byte should be 0x96, found 0x{data[0xB2]:02x}")
print(f"GBA image valid: {rom} ({len(data)} bytes, {title}, {code}/{maker})")
