# WatchBoy Games

Original watch-friendly games, maintained separately from the emulator.

## First title: Trash Lid Dash
An original fast side-scrolling platformer with a trash-can-lid-riding hero and original environments and art. No proprietary character assets.

## Controls
- Swipe left/right: movement
- Flick up or tap upper display: jump
- Watch circular viewport with safe margins

## Delivery contract
Build a genuine GBA ROM with an open toolchain (e.g. devkitARM/libgba or another verified GBA SDK), compatible with WatchBoy Advance and independently testable on desktop GBA emulators. Avoid Wear OS-specific APIs inside ROM gameplay.

## Planned refinements
- Responsive jump buffering and coyote time
- Persistent best-run records when supported by save hardware
- Short sessions and instant retry

## Acceptance checks
ROM compiles, boots in a verified GBA emulator, movement and jump respond correctly, collisions and restarts work, and no third-party game assets are bundled.

**Status:** Design specification only; no ROM or verified game build yet.
