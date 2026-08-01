# Steam Controller Battery Monitor

Lightweight program that creates a tray icon that will allow you to monitor the steam controller's (2026) battery.

(very very WIP, but it does technically do what it says now, albeit i wont upload an executable until its at a level i'm happy with)

## TODO:
- Keep program running if steam controller is disconnected.
  - rework some parts of triton lib to not kill program if hid error occurs
- Option to start with pc
- Icons (either procedural or something silly)
- battery monitoring functionality to measure capacity and battery loss over time

## How To


**Supported device:** Steam Controller (2026). Other controllers are unsupported.

---
[**Download the latest build here!**]()

#### Windows
1. Extract the files and run executable
2. Observe battery percentage in tray
3. Yippee!

#### On Linux
1. Install libhid (with i believe either, `sudo apt install libhidapi-dev` or `sudo pacman -S hidapi`)
2. Extract files and run executable
3. Observe battery percentage in tray
4. Yippee!


## Compiling
**Build (Linux / WSL)**

```bash
sudo apt update
sudo apt install build-essential pkg-config libhidapi-dev qt6-base-dev qt6-base-dev-tools
make
```

**Build (Windows, MSYS2 UCRT64)**
```bash
pacman -Syu

pacman -S mingw-w64-ucrt-x86_64-gcc \
          mingw-w64-ucrt-x86_64-hidapi \
          mingw-w64-ucrt-x86_64-ffmpeg \
          mingw-w64-ucrt-x86_64-make \
          mingw-w64-ucrt-x86_64-pkgconf \
          mingw-w64-ucrt-x86_64-ninja \
          mingw-w64-ucrt-x86_64-qt6-base

make
```
## Changelog

I would appreciate stars, they make me happy :3

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/N4N6145I0V)

If you find this cool, feel free to check out [SteamHapticsPlayer]()