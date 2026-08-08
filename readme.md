# Steam Controller Battery Monitor
  A lightweight, cross-platform tray icon to get information about the steam controller's battery

  I don't know why there isnt a good place to get this information normally

![](https://img.pixelator.xyz/V5hxYuYw.png)

## How To


**Supported device:** Steam Controller (2026). Other controllers are unsupported.
[**Download the latest build here!**](https://github.com/Pixel1011/Steam-Controller-Battery-Monitor/releases/latest)

#### Windows
1. Extract the files and run executable
2. Observe battery percentage in tray
3. Yippee!

#### On Linux
1. Install libhid and Qt6 libs if not already installed with either (exclude wayland package if not using wayland),
      `sudo apt install libhidapi-dev libqt6core6 libqt6core6 libqt6gui6 qt6-wayland` 

      or 
      
      `sudo pacman -S hidapi qt6-base qt6-wayland`


2. Extract files and run executable
3. Observe battery percentage in tray
4. Yippee!


## Compiling

**Clone with submodules:**
``git clone --recurse-submodules https://github.com/Pixel1011/Steam-Controller-Battery-Monitor.git``


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

[v1.0.0]
Initial Release

## TODO in future:
- battery monitoring functionality to measure capacity and battery loss over time
- with above, also add time till charged metric

I would appreciate stars, they make me happy :3c

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/N4N6145I0V)

If you find this cool, feel free to check out [SteamHapticsPlayer](https://github.com/Pixel1011/SteamHapticsPlayer) to play audio files on your controller!