# Triton Lib
A general purpose library for anything to do with the 2026 steam controller.

Mainly for use by me for my steam controller projects
[SteamHapticsPlayer](https://github.com/Pixel1011/SteamHapticsPlayer)
[Steam Controller Battery Monitor](https://github.com/Pixel1011/Steam-Controller-Battery-Monitor)


I am happy for anyone to use this library, as long as credit is given, though,
assume nothing is stable and everything could change at any time


**Usage:**
  Requires libhid.
  Use ControllerFinder to get the currently connected controller (only supports 1 at a time). It will return an instance of TritonController if found, and exit if libhid has a problem.
  If TritonController.disconnected becomes true, you must delete the object and attempt to get a new one from ControllerFinder to reconnect.
  Most things will run on the thread it is accessed from, aside from polling data, I may add some extra functions, especially for audio streaming to do that on another thread.
  You shouldnt need to mess with any mutexes yourself, just use the getters in TritonController and all will be well. 
