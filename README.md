# WatchBoy Games

Original games and distributable GBA ROMs, maintained separately from the emulator. The first buildable cartridge is **Trash Lid Dash**.

## Trash Lid Dash

An original side-scrolling platformer. Ride a salvaged trash-can lid across rooftops, clear gaps, avoid junk obstacles, and reach the end of the block. The player sprite, scene, palette, and game code are original; no commercial ROM, BIOS, character, or artwork is included.

**GBA controls:** D-pad left/right to move, A or B to jump, Start to retry after a wipeout or finish, and Select to restart during a run.

**WatchBoy touch mapping:** horizontal swipes send left/right, a downward swipe sends Down, upper-left tap sends Up, and an upward flick / upper-center tap sends A. Lower corners send B/A, lower-center tap/hold sends Start/Select, upper-right tap sends R, and upper-left long-press sends L. Holding either lower side sends continuous left/right input.

The game uses fixed-point movement, platform landing checks, jump buffering, coyote time, hazard collisions, fall detection, a finish line, and immediate retry. Its physics model is also built and tested with host GCC independently of the GBA toolchain.

The build links devkitPro's libgba; its license and static-linking exception are included in `Trash-Lid-Dash/LIBGBA-LICENSE.txt`.

## Build and test

With devkitPro/devkitARM and libgba installed:

```sh
cd Trash-Lid-Dash
make clean all
gcc -std=c11 -Wall -Wextra -Werror -O2 source/game_model.c tests/game_model_test.c -o /tmp/trash-lid-dash-test
/tmp/trash-lid-dash-test
python3 tools/verify_gba.py TrashLidDash.gba
```

The project workflow builds with the devkitPro devkitARM container, checks the GBA header, runs model tests, and boots the ROM for 180 frames in the desktop mGBA performance runner. The `.gba` is uploaded as the `Trash-Lid-Dash-GBA` workflow artifact.

To build through Docker from the repository root:

```sh
docker run --rm -v "$PWD/Trash-Lid-Dash:/work" -w /work devkitpro/devkitarm:20241104 make clean all
```

See [Trash-Lid-Dash/README.md](Trash-Lid-Dash/README.md) for first-build status and verification boundaries.
