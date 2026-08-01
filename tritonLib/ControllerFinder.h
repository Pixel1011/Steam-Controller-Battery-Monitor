#pragma once
#include "TritonController.h"
#include <iostream>
#include <cstdint>

enum class ControllerType {
	None,
	Original,
	Triton,
	Jupiter,
	Galileo
};

class ControllerFinder {
  private:
  hid_device* open_steam_controller_hid(uint16_t pid);
  public:
  ControllerFinder();
  ~ControllerFinder() = default;

  TritonController* getController();
  
};