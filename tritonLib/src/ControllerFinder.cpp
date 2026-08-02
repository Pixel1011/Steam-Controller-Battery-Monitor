#include "ControllerFinder.h"

hid_device* ControllerFinder::open_steam_controller_hid(uint16_t pid) {
  struct hid_device_info *devs = hid_enumerate(0x28DE, pid);
  hid_device *handle = NULL;
  uint8_t buf[64];
  for (struct hid_device_info *cur = devs; cur != NULL; cur = cur->next) {
    // printf("%u\n", cur->usage_page);
    if (cur->usage_page == 0xFF00) {
      handle = hid_open_path(cur->path);
      if (handle) {
        int r = hid_read_timeout(handle, buf, sizeof(buf), 100);
        if (r > 0) { return handle; }
      }
    }
    handle = NULL;
  }
  hid_free_enumeration(devs);
  return handle;
}

ControllerFinder::ControllerFinder() {
  // init hidAPI
  if (int r = hid_init() != 0) {
    std::cerr << "HIDAPI Init Error: " << r << std::endl;
    std::cin.ignore();
    exit(1);
  }
}

// basically completely stolen from SteamHapticsSinger
TritonController* ControllerFinder::getController() {
  ControllerType type;

  hid_device* hid_handle;

  // could also be ble but bleh
  bool tritonWired = false;
  // Open Steam Controller device

   if ((hid_handle = this->open_steam_controller_hid(0x1302)) != NULL) { // Steam Controller (2026)
    std::cout << "Found wired Steam Controller (2026)" << std::endl;
    type = ControllerType::Triton;
    tritonWired = true;

  } else if ((hid_handle = this->open_steam_controller_hid(0x1304)) != NULL) { // Steam Puck
    std::cout << "Found Steam Puck, will attempt to use the first Steam Controller (2026)" << std::endl;
    type = ControllerType::Triton;

  } else {
    std::cout << "No device found" << std::endl;
    type = ControllerType::None;
  }

  switch (type)
  {
  case ControllerType::None:
    return nullptr;
    break;
  case ControllerType::Triton:
    return new TritonController(hid_handle, static_cast<TritonInterface>(tritonWired));
    break; 
  default:
    return nullptr;
    break;
  }

};