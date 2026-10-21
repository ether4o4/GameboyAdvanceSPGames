# Trash Lid Dash

Playable GBA side-scrolling platformer built as an original game for the WatchBoy Advance project.

## Controls

- D-pad left/right: move.
- A or B: jump. Jump buffering and coyote time help short watch taps register.
- Start: begin from the title screen or retry after a wipeout / finish.
- Select: restart during a run.

## Build

Requires devkitARM and libgba from devkitPro:

```sh
make clean all
gcc -std=c11 -Wall -Wextra -Werror -O2 source/game_model.c tests/game_model_test.c -o /tmp/trash-lid-dash-test
/tmp/trash-lid-dash-test
python3 tools/verify_gba.py TrashLidDash.gba
```

The workflow builds the `.gba` inside the official devkitARM container and boots it for 180 frames with the desktop mGBA performance runner. It also tests movement, jumping/landing, hazard collision, falling, end-of-level handling, and restart on the host C compiler.

## Current verification

The source and host physics tests are ready. The GBA cartridge and desktop mGBA boot result are produced by GitHub Actions; a `.gba` file is not checked into the source tree. Physical Galaxy Watch7 input, sound, and frame pacing still require installation of the separately built WatchBoy APK on a watch.

All game code, visuals, and characters are original. No commercial ROM, Nintendo BIOS, or third-party game asset is bundled.

The build links devkitPro's libgba. Its license and static-linking exception are included in `LIBGBA-LICENSE.txt`.
